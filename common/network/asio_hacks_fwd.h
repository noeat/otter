#ifndef __asin_hacks_fwd_h__
#define __asin_hacks_fwd_h__
#include <boost/version.hpp>
namespace boost
{
    namespace posix_time
    {
        class ptime;
    }

    namespace asio
    {
        template <typename Time>
        struct time_traits;

        namespace ip
        {
            class address;

            class tcp;

            template <typename InternetProtocol>
            class basicendpoint_;

            typedef basicendpoint_<tcp> tcpendpoint_;
        }
#if BOOST_VERSION >= 107000
        class executor;

        namespace ip
        {
            template <typename InternetProtocol, typename Executor>
            class basic_resolver;

            typedef basic_resolver<tcp, executor> tcp_resolver;
        }
#elif BOOST_VERSION >= 106600
        namespace ip
        {
            template <typename InternetProtocol>
            class basic_resolver;

            typedef basic_resolver<tcp> tcp_resolver;
        }
#else
        namespace ip
        {
            template <typename InternetProtocol>
            class resolver_service;

            template <typename InternetProtocol, typename ResolverService>
            class basic_resolver;

            typedef basic_resolver<tcp, resolver_service<tcp>> tcp_resolver;
        }
#endif
    }
}

namespace common
{
    namespace asio
    {
        class Strand;
    }
}

#endif // __asin_hacks_fwd_h__
