#ifndef __socket_mgr_h__
#define __socket_mgr_h__
#include "async_acceptor.h"
#include "errors.h"
#include "ionetwork_thread.h"
#include <boost/asio/ip/tcp.hpp>
#include <memory>
using boost::asio::ip::tcp;
template<class SocketType>
class TCPSocketMgr
{
public:
    virtual ~TCPSocketMgr()
    {
        ASSERT(!threads_ && !acceptor_ && !thread_count_, "resource must free before ~SocketMgr");
    }

    virtual bool start_network(common::asio::IoContext& io_context,
		std::string const& bind_ip, uint16 port, int thread_count)
    {
        ASSERT(thread_count > 0);
		TCPAsyncAcceptor* acceptor = nullptr;
        try
        {
            acceptor = new TCPAsyncAcceptor(io_context, bind_ip, port);
        }
        catch (boost::system::system_error const& err)
        {
            LOG_ERROR("network", "Exception caught in SocketMgr.StartNetwork ({}:{}): {}", 
				bind_ip.c_str(), port, err.what());
            return false;
        }

        if (!acceptor->bind())
        {
            LOG_ERROR("network", "start_network failed to bind socket acceptor");
            return false;
        }

        acceptor_ = acceptor;
        thread_count_ = thread_count;
        threads_ = start_threads();
        ASSERT(threads_);

		for (int32 i = 0; i < thread_count_; ++i)
		{
			threads_[i].start();
		}
           
		LOG_INFO("network", "start network thread[{}] host:{{}:{}}",
			thread_count_, bind_ip.c_str(), port);
        return true;
    }

    virtual void stop_network()
    {
        acceptor_->close();
		if (thread_count_ != 0)
		{
			for (int32 i = 0; i < thread_count_; ++i)
			{
				threads_[i].stop();
			}				
		}            

        wait();
        delete acceptor_;
        acceptor_ = nullptr;
        delete[] threads_;
        threads_ = nullptr;
        thread_count_ = 0;
		LOG_INFO("network", "stoped network!");
    }

    void wait()
    {
		if (thread_count_ != 0)
		{
			for (int32 i = 0; i < thread_count_; ++i)
				threads_[i].wait();
		}            
    }

    virtual void on_socket_open(tcp::socket&& sock, uint32 thread_index)
    {
        try
        {
			ASSERT(thread_index < thread_count_, "thread index geater threadcount");
            std::shared_ptr<SocketType> new_socket = 
				std::make_shared<SocketType>(std::move(sock));
			new_socket->start();
            threads_[thread_index].add_socket(new_socket);
        }
        catch (boost::system::system_error const& err)
        {
            LOG_WARN("network", "failed to open socket client's remote address,{}", 
				err.what());
        }
    }

    int32 get_network_thread_count()const { return thread_count_; }
    uint32 select_thread_with_min_connections() const
    {
        uint32 min = 0;
		for (int32 i = 1; i < thread_count_; ++i)
		{
			if (threads_[i].get_connection_count() < 
				threads_[min].get_connection_count())
				min = i;
		}         

        return min;
    }

    std::pair<tcp::socket*, uint32> get_socket_for_accept()
    {
        uint32 thread_index = select_thread_with_min_connections();
        return std::make_pair(threads_[threadIndex].get_socket_for_accept(), thread_index);
    }

protected:
	TCPSocketMgr() : acceptor_(nullptr), threads_(nullptr), thread_count_(0)
    {
    }

    virtual IONetworkThread<SocketType>* start_threads() const = 0;
protected:
	TCPAsyncAcceptor* acceptor_;
	IONetworkThread<SocketType>* threads_;
    int32 thread_count_;
};

#endif // __socket_mgr_h__
