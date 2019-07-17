#ifndef __common_strand_h__
#define __common_strand_h__
#include "io_context.h"
#include <boost/asio/strand.hpp>

#if BOOST_VERSION >= 106600
#include <boost/asio/bind_executor.hpp>
#endif

namespace common
{
  namespace asio
  {
    class Strand : public IoContextBaseNamespace::IoContextBase::strand
    {
    public:
      Strand(IoContext& ioContext) : IoContextBaseNamespace::IoContextBase::strand(ioContext) { }
    };

#if BOOST_VERSION >= 106600
    using boost::asio::bind_executor;
#else
    template<typename T>
    inline decltype(auto) bind_executor(Strand& strand, T&& t)
    {
      return strand.wrap(std::forward<T>(t));
    }
#endif
  }
}

#endif // __common_strand_h__
