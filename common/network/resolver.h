#ifndef __common_resolver_h__
#define __common_resolver_h__
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <boost/optional.hpp>
#include <boost/utility/in_place_factory.hpp>
template <typename T>
using Optional = boost::optional<T>;

namespace common
{
  namespace net
  {
    inline Optional<boost::asio::ip::tcp::endpoint> resolve(boost::asio::ip::tcp::resolver& resolver,
      boost::asio::ip::tcp const& protocol,
      std::string const& host, std::string const& service)
    {
      boost::system::error_code ec;
#if BOOST_VERSION >= 106600
      boost::asio::ip::tcp::resolver::results_type results = resolver.resolve(protocol, host, service, ec);
      if (results.empty() || ec)
        return {};

      return results.begin()->endpoint();
#else
      boost::asio::ip::tcp::resolver::query query(std::move(protocol), std::move(host), std::move(service));
      boost::asio::ip::tcp::resolver::iterator itr = resolver.resolve(query, ec);
      boost::asio::ip::tcp::resolver::iterator end;
      if (itr == end || ec)
        return {};

      return itr->endpoint();
#endif
    }
  }
}

#endif // __common_resolver_h__
