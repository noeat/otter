#ifndef __async_accpetor_h__
#define __async_accpetor_h__
#include "io_context.h"
#include "ip_adddress.h"
#include "log.h"
#include <boost/asio/ip/tcp.hpp>
#include <functional>
#include <atomic>

using boost::asio::ip::tcp;

#if BOOST_VERSION >= 106600
#define MAX_LISTEN_CONNECTIONS boost::asio::socket_base::max_listen_connections
#else
#define MAX_LISTEN_CONNECTIONS boost::asio::socket_base::max_connections
#endif

class TCPAsyncAcceptor
{
public:
  typedef void(*AcceptCallback)(tcp::socket&& newsocket_, uint32 thread_index);
  TCPAsyncAcceptor(common::asio::IoContext& io_context,
    std::string const& bind_ip, uint16 port) :
    acceptor_(io_context),
    endpoint_(common::net::make_address(bind_ip), port),
    socket_(io_context),
    closed_(false),
    socket_Factory_(std::bind(&TCPAsyncAcceptor::defeault_socket_factory, this))
  {

  }

  template<class T>
  void async_accept();

  template<AcceptCallback acceptCallback>
  void async_accept_with_callback()
  {
    tcp::socket* socket;
    uint32 thread_index;
    std::tie(socket, thread_index) = socket_Factory_();
    acceptor_.async_accept(*socket,
      [this, socket, thread_index](boost::system::error_code error)
      {
        if (!error)
        {
          try
          {
            socket->non_blocking(true);
            acceptCallback(std::move(*socket), thread_index);
          }
          catch (boost::system::system_error const& err)
          {
            LOG_INFO("network", "Failed to initialize client's socket {}",
              err.what());
          }
        }

        if (!closed_)
          this->async_accept_with_callback<acceptCallback>();
      });
  }

  bool bind()
  {
    boost::system::error_code error_code;
    acceptor_.open(endpoint_.protocol(), error_code);
    if (error_code)
    {
      LOG_INFO("network", "Failed to open acceptor {}",
        error_code.message().c_str());
      return false;
    }

    acceptor_.bind(endpoint_, error_code);
    if (error_code)
    {
      LOG_INFO("network", "Could not bind to {}:{} {}",
        endpoint_.address().to_string().c_str(),
        endpoint_.port(),
        error_code.message().c_str());
      return false;
    }

    acceptor_.listen(MAX_LISTEN_CONNECTIONS, error_code);
    if (error_code)
    {
      LOG_INFO("network", "Failed to start listening on {}:{} {}",
        endpoint_.address().to_string().c_str(),
        endpoint_.port(),
        error_code.message().c_str());
      return false;
    }

    return true;
  }

  void close()
  {
    if (closed_.exchange(true))
      return;

    boost::system::error_code err;
    acceptor_.close(err);
  }

  void set_socket_factory(
    std::function<std::pair<tcp::socket*, uint32>()> func)
  {
    socket_Factory_ = func;
  }

private:
  std::pair<tcp::socket*, uint32> defeault_socket_factory()
  {
    return std::make_pair(&socket_, 0);
  }

  tcp::acceptor acceptor_;
  tcp::endpoint endpoint_;
  tcp::socket socket_;
  std::atomic<bool> closed_;
  std::function<std::pair<tcp::socket*, uint32>()> socket_Factory_;
};

template<class T>
void TCPAsyncAcceptor::async_accept()
{
  acceptor_.async_accept(socket_, [this](boost::system::error_code error)
    {
      if (!error)
      {
        try
        {
          std::make_shared<T>(std::move(this->socket_))->Start();
        }
        catch (boost::system::system_error const& err)
        {
          LOG_INFO("network", "Failed to retrieve client's remote address {}",
            err.what());
        }
      }

      if (!closed_)
        this->async_accept<T>();
    });
}

#endif /* __async_accpetor_h__ */
