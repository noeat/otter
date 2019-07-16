#ifndef _common_socket__H__
#define _common_socket__H__
#include "message_buffer.h"
#include <atomic>
#include <queue>
#include <memory>
#include <functional>
#include <type_traits>
#include <boost/asio/ip/tcp.hpp>
#include "log.h"
using boost::asio::ip::tcp;

#define READ_BLOCK_SIZE 4096
#ifdef BOOST_ASIO_HAS_IOCP
#define ASIO__USE_IOCP
#endif
template<class T, class Stream = tcp::socket>
class Socket : public std::enable_shared_from_this<T>
{
public:
    explicit Socket(tcp::socket&& socket) : 
		socket_(std::move(socket)), 
		remote_address_(socket_.remote_endpoint().address()),
        remote_port_(socket_.remote_endpoint().port()), 
		read_buffer_(), closed_(false), closing_(false), is_writing_async_(false)
    {
        read_buffer_.resize(READ_BLOCK_SIZE);
    }

    virtual ~Socket()
    {
        closed_ = true;
        boost::system::error_code error;
        socket_.close(error);
    }

    virtual void start() = 0;

    virtual bool update()
    {
        if (closed_)
            return false;

#ifndef ASIO__USE_IOCP
        if (is_writing_async_ || (write_queue_.empty() && !closing_))
            return true;

        for (; handle_queue();)
            ;
#endif

        return true;
    }

    boost::asio::ip::address get_remote_ipaddress() const
    {
        return remote_address_;
    }

    uint16 get_remote_port() const
    {
        return remote_port_;
    }

    void async_read()
    {
        if (!is_open())
            return;

        read_buffer_.normalize();
        read_buffer_.ensure_free_space();
        socket_.async_read_some(
			boost::asio::buffer(read_buffer_.get_write_pointer(), 
				read_buffer_.get_remaining_space()),
            std::bind(&Socket<T, Stream>::read_handler_internal, 
				this->shared_from_this(), 
				std::placeholders::_1, std::placeholders::_2));
    }

    void async_read_with_callback(void (T::*callback)(boost::system::error_code, std::size_t))
    {
        if (!is_open())
            return;

        read_buffer_.normalize();
        read_buffer_.ensure_free_space();
        socket_.async_read_some(
			boost::asio::buffer(read_buffer_.get_write_pointer(), read_buffer_.get_remaining_space()),
            std::bind(callback, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }

    void queue_packet(MessageBuffer&& buffer)
    {
        write_queue_.push(std::move(buffer));

#ifdef ASIO__USE_IOCP
        async_process_queue();
#endif
    }

    bool is_open() const { return !closed_ && !closing_; }

    void close_socket()
    {
        if (closed_.exchange(true))
            return;

        boost::system::error_code err_code;
        socket_.shutdown(boost::asio::socket_base::shutdown_send, err_code);
        if (err_code)
            LOG_DEBUG("network", "Socket::CloseSocket: {} errored when shutting down socket: {} ({})", 
				get_remote_ipaddress().to_string().c_str(),
				err_code.value(), err_code.message().c_str());

        on_close();
    }

    void delayed_close_socket() { closing_ = true; }
    MessageBuffer& get_read_buffer() { return read_buffer_; }
protected:
    virtual void on_close() { }
    virtual void read_handler() = 0;
    bool async_process_queue()
    {
        if (is_writing_async_)
            return false;

        is_writing_async_ = true;

#ifdef ASIO__USE_IOCP
        MessageBuffer& buffer = write_queue_.front();
        socket_.async_write_some(boost::asio::buffer(buffer.get_read_pointer(), 
			buffer.get_active_size()), 
			std::bind(&Socket<T, Stream>::write_handler,
            this->shared_from_this(), 
			std::placeholders::_1, 
			std::placeholders::_2));
#else
        socket_.async_write_some(boost::asio::null_buffers(), std::bind(&Socket<T, Stream>::write_handler_wrapper,
            this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
#endif

        return false;
    }

    void set_no_delay(bool enable)
    {
        boost::system::error_code err_code;
        socket_.set_option(tcp::no_delay(enable), err_code);
        if (err_code)
            LOG_DEBUG("network", "Socket::SetNoDelay: failed to set_option(boost::asio::ip::tcp::no_delay) for {} - {} ({})",
				get_remote_ipaddress().to_string().c_str(),
				err_code.value(), err_code.message().c_str());
    }

    Stream& underlying_stream()
    {
        return socket_;
    }

private:
    void read_handler_internal(boost::system::error_code error, 
		size_t transferred_bytes)
    {
        if (error)
        {
            close_socket();
            return;
        }

        read_buffer_.write_completed(transferred_bytes);
        read_handler();
    }

#ifdef ASIO__USE_IOCP

    void write_handler(boost::system::error_code error, 
		std::size_t transferred_bytes)
    {
        if (!error)
        {
            is_writing_async_ = false;
            write_queue_.front().read_completed(transferred_bytes);
            if (!write_queue_.front().get_active_size())
                write_queue_.pop();

            if (!write_queue_.empty())
                async_process_queue();
            else if (closing_)
                close_socket();
        }
        else
            close_socket();
    }

#else

    void write_handler_wrapper(boost::system::error_code /*error*/, 
		std::size_t /*transferedBytes*/)
    {
        is_writing_async_ = false;
        HandleQueue();
    }

    bool handle_queue()
    {
        if (write_queue_.empty())
            return false;

        MessageBuffer& queued_message = write_queue_.front();
        std::size_t bytes_2_send = queued_message.get_active_size();
        boost::system::error_code error;
        std::size_t bytes_sented = socket_.write_some(
			boost::asio::buffer(queued_message.get_read_pointer(), bytes_2_send), error);
        if (error)
        {
            if (error == boost::asio::error::would_block || error == boost::asio::error::try_again)
                return async_process_queue();

            write_queue_.pop();
            if (closing_ && write_queue_.empty())
                close_socket();
            return false;
        }
        else if (bytes_sented == 0)
        {
            write_queue_.pop();
            if (closing_ && write_queue_.empty())
                close_socket();
            return false;
        }
        else if (bytes_sented < bytes_2_send) // now n > 0
        {
            queued_message.read_completed(bytes_sented);
            return async_process_queue();
        }

        write_queue_.pop();
        if (closing_ && write_queue_.empty())
            close_socket();
        return !write_queue_.empty();
    }

#endif

    Stream socket_;
    boost::asio::ip::address remote_address_;
    uint16 remote_port_;
    MessageBuffer read_buffer_;
    std::queue<MessageBuffer> write_queue_;
    std::atomic<bool> closed_;
    std::atomic<bool> closing_;
    bool is_writing_async_;
};

#endif // _common_socket__H__
