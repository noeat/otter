#ifndef __network_thread_h__
#define __network_thread_h__
#include "define.h"
#include "deadline_timer.h"
#include "errors.h"
#include "io_context.h"
#include "log.h"
#include <boost/asio/ip/tcp.hpp>
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <set>
#include <thread>

using boost::asio::ip::tcp;

template<class SocketType>
class IONetworkThread
{
public:
  IONetworkThread() : connections_(0), stopped_(false), thread_(nullptr), io_context_(1),
    accept_socket_(io_context_), update_timer_(io_context_)
  {
  }

  virtual ~IONetworkThread()
  {
    stop();
    if (thread_)
    {
      wait();
    }
  }

  void stop()
  {
    stopped_ = true;
    io_context_.stop();
  }

  bool start()
  {
    if (thread_)
      return false;

    thread_ = new std::thread(&IONetworkThread::run, this);
    return true;
  }

  void wait()
  {
    ASSERT(thread_);

    thread_->join();
    delete thread_;
    thread_ = nullptr;
  }

  int32 get_connection_count() const
  {
    return connections_;
  }

  virtual void add_socket(std::shared_ptr<SocketType> sock)
  {
    std::lock_guard<std::mutex> lock(new_sockets_lock_);
    ++connections_;
    new_sockets_.push_back(sock);
    socket_added(sock);
  }

  tcp::socket* get_socket_for_accept() { return &accept_socket_; }
protected:
  virtual void socket_added(std::shared_ptr<SocketType> /*sock*/) { }
  virtual void socket_removed(std::shared_ptr<SocketType> /*sock*/) { }

  void add_new_sockets()
  {
    SocketContainer new_sockets;
    {
      std::lock_guard<std::mutex> lock(new_sockets_lock_);
      new_sockets.swap(new_sockets_);
    }

    if (new_sockets.empty())
      return;

    for (std::shared_ptr<SocketType> sock : new_sockets)
    {
      if (!sock->is_open())
      {
        socket_removed(sock);
        --connections_;
      }
      else
      {
        sockets_.push_back(sock);
      }
    }
  }

  void run()
  {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    LOG_INFO("network", "network thread{} starting.", ss.str().c_str());
    update_timer_.expires_from_now(boost::posix_time::milliseconds(10));
    update_timer_.async_wait(std::bind(&IONetworkThread<SocketType>::update, this));
    io_context_.run();
    LOG_INFO("network", "network thread{} stoped.", ss.str().c_str());
    new_sockets_.clear();
    sockets_.clear();
  }

  void update()
  {
    if (stopped_)
      return;

    update_timer_.expires_from_now(boost::posix_time::milliseconds(10));
    update_timer_.async_wait(std::bind(&IONetworkThread<SocketType>::update, this));
    add_new_sockets();
    sockets_.erase(std::remove_if(sockets_.begin(), sockets_.end(), [this](std::shared_ptr<SocketType> sock)
      {
        if (!sock->update())
        {
          if (sock->is_open())
            sock->close_socket();

          this->socket_removed(sock);
          --this->connections_;
          return true;
        }

        return false;
      }), sockets_.end());
  }

private:
  typedef std::vector<std::shared_ptr<SocketType>> SocketContainer;
  std::atomic<int32> connections_;
  std::atomic<bool> stopped_;
  std::thread* thread_;
  SocketContainer sockets_;
  std::mutex new_sockets_lock_;
  SocketContainer new_sockets_;
  common::asio::IoContext io_context_;
  tcp::socket accept_socket_;
  common::asio::DeadlineTimer update_timer_;
};

#endif // __network_thread_h__
