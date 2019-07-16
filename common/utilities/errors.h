#ifndef __common_errors_h__
#define __common_errors_h__
#include "define.h"

namespace common
{
void Assert(char const* file, int line, char const* function, char const* message) ;
void Assert(char const* file, int line, char const* function, char const* message, char const* format, ...);
void Fatal(char const* file, int line, char const* function, char const* message, ...) ;
void Error(char const* file, int line, char const* function, char const* message) ;

void Abort(char const* file, int line, char const* function) ;
void Warning(char const* file, int line, char const* function, char const* message);
void AbortHandler(int sigval) ;
} // namespace common

#if _WIN32
#define ASSERT_BEGIN __pragma(warning(push)) __pragma(warning(disable: 4127))
#define ASSERT_END __pragma(warning(pop))
#else
#define ASSERT_BEGIN
#define ASSERT_END
#endif

#define WPAssert(cond, ...) ASSERT_BEGIN do { if (!(cond)) common::Assert(__FILE__, __LINE__, __FUNCTION__, #cond, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPFatal(cond, ...) ASSERT_BEGIN do { if (!(cond)) common::Fatal(__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); } while(0) ASSERT_END
#define WPError(cond, msg) ASSERT_BEGIN do { if (!(cond)) common::Error(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
#define WPWarning(cond, msg) ASSERT_BEGIN do { if (!(cond)) common::Warning(__FILE__, __LINE__, __FUNCTION__, (msg)); } while(0) ASSERT_END
#define WPAbort() ASSERT_BEGIN do { common::Abort(__FILE__, __LINE__, __FUNCTION__); } while(0) ASSERT_END

#define ASSERT WPAssert
#define ABORT WPAbort

template <typename T>
inline T* ASSERT_NOTNULL_IMPL(T* pointer, char const* expr)
{
    ASSERT(pointer, "%s", expr);
    return pointer;
}

#define ASSERT_NOTNULL(pointer) ASSERT_NOTNULL_IMPL(pointer, #pointer)

#endif //__common_errors_h__
