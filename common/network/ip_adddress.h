#ifndef __ip_address_h__
#define __ip_address_h__
#include "define.h"
#include <boost/asio/ip/address.hpp>

namespace common
{
    namespace net
    {
#if BOOST_VERSION >= 106600
        using boost::asio::ip::make_address;
        using boost::asio::ip::make_address_v4;
        inline uint32 address_to_uint(boost::asio::ip::address_v4 const& address) { return address.to_uint(); }
#else
        inline boost::asio::ip::address make_address(char const* str) { return boost::asio::ip::address::from_string(str); }
        inline boost::asio::ip::address make_address(char const* str, boost::system::error_code& ec) { return boost::asio::ip::address::from_string(str, ec); }
        inline boost::asio::ip::address make_address(std::string const& str) { return boost::asio::ip::address::from_string(str); }
        inline boost::asio::ip::address make_address(std::string const& str, boost::system::error_code& ec) { return boost::asio::ip::address::from_string(str, ec); }
        inline boost::asio::ip::address_v4 make_address_v4(char const* str) { return boost::asio::ip::address_v4::from_string(str); }
        inline boost::asio::ip::address_v4 make_address_v4(char const* str, boost::system::error_code& ec) { return boost::asio::ip::address_v4::from_string(str, ec); }
        inline boost::asio::ip::address_v4 make_address_v4(std::string const& str) { return boost::asio::ip::address_v4::from_string(str); }
        inline boost::asio::ip::address_v4 make_address_v4(std::string const& str, boost::system::error_code& ec) { return boost::asio::ip::address_v4::from_string(str, ec); }
        inline uint32 address_to_uint(boost::asio::ip::address_v4 const& address) { return address.to_ulong(); }
#endif
    }
}

#endif // __ip_address_h__
