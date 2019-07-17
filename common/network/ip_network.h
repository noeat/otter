#ifndef __ip_network_h__
#define __ip_network_h__
#include "define.h"
#include "ip_adddress.h"
#include <boost/version.hpp>
#if BOOST_VERSION >= 106600
#include <boost/asio/ip/network_v4.hpp>
#include <boost/asio/ip/network_v6.hpp>
#endif

namespace common
{
  namespace net
  {
    inline bool is_in_network(boost::asio::ip::address_v4 const& network_address,
      boost::asio::ip::address_v4 const& mask,
      boost::asio::ip::address_v4 const& client_address)
    {
#if BOOST_VERSION >= 106600
      boost::asio::ip::network_v4 network = boost::asio::ip::make_network_v4(network_address, mask);
      boost::asio::ip::address_v4_range hosts = network.hosts();
      return hosts.find(client_address) != hosts.end();
#else
      return (client_address.to_ulong() & mask.to_ulong()) == (network_address.to_ulong() & mask.to_ulong());
#endif
    }

    inline boost::asio::ip::address_v4 get_default_netmask_v4(
      boost::asio::ip::address_v4 const& network_address)
    {
      if ((address_to_uint(network_address) & 0x80000000) == 0)
        return boost::asio::ip::address_v4(0xFF000000);
      if ((address_to_uint(network_address) & 0xC0000000) == 0x80000000)
        return boost::asio::ip::address_v4(0xFFFF0000);
      if ((address_to_uint(network_address) & 0xE0000000) == 0xC0000000)
        return boost::asio::ip::address_v4(0xFFFFFF00);
      return boost::asio::ip::address_v4(0xFFFFFFFF);
    }

    inline bool is_in_network(boost::asio::ip::address_v6 const& network_address,
      uint16 prefix_length,
      boost::asio::ip::address_v6 const& client_address)
    {
#if BOOST_VERSION >= 106600
      boost::asio::ip::network_v6 network = boost::asio::ip::make_network_v6(network_address, prefix_length);
      boost::asio::ip::address_v6_range hosts = network.hosts();
      return hosts.find(client_address) != hosts.end();
#else
      (void)network_address;
      (void)prefix_length;
      (void)client_address;
      return false;
#endif
    }
  }
}

#endif // __ip_network_h__
