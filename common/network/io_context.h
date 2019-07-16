#ifndef __io_context_h__
#define __io_context_h__
#include <boost/version.hpp>
#if BOOST_VERSION >= 106600
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#define IoContextBaseNamespace boost::asio
#define IoContextBase io_context
#else
#include <boost/asio/io_service.hpp>
#define IoContextBaseNamespace boost::asio
#define IoContextBase io_service
#endif

namespace common
{
    namespace asio
    {
        class IoContext
        {
        public:
            IoContext() : impl_() { }
            explicit IoContext(int concurrency_hint) : impl_(concurrency_hint) { }

            operator IoContextBaseNamespace::IoContextBase&() { return impl_; }
            operator IoContextBaseNamespace::IoContextBase const&() const { return impl_; }

            std::size_t run() { return impl_.run(); }
            void stop() { impl_.stop(); }

#if BOOST_VERSION >= 106600
            boost::asio::io_context::executor_type get_executor() noexcept { return impl_.get_executor(); }
#endif

        private:
            IoContextBaseNamespace::IoContextBase impl_;
        };

        template<typename T>
        inline decltype(auto) post(IoContextBaseNamespace::IoContextBase& ioContext, T&& t)
        {
#if BOOST_VERSION >= 106600
            return boost::asio::post(ioContext, std::forward<T>(t));
#else
            return ioContext.post(std::forward<T>(t));
#endif
        }

        template<typename T>
        inline decltype(auto) get_io_context(T&& ioObject)
        {
#if BOOST_VERSION >= 106600
            return ioObject.get_executor().context();
#else
            return ioObject.get_io_service();
#endif
        }
    }
}

#endif // __io_context_h__
