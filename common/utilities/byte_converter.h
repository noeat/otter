#ifndef __byte_converter_h__
#define __byte_converter_h__
#include "define.h"
#include <algorithm>

namespace ByteConverter
{
  template<size_t T>
  inline void convert(char *val)
  {
    std::swap(*val, *(val + T - 1));
    convert<T - 2>(val + 1);
  }

  template<> inline void convert<0>(char *) { }
  template<> inline void convert<1>(char *) { }

  template<typename T> inline void apply(T *val)
  {
    convert<sizeof(T)>((char *)(val));
  }
}

#if CPU_ENDIAN == _BIGENDIAN
template<typename T> inline void endian_convert(T& val) { ByteConverter::apply<T>(&val); }
template<typename T> inline void endian_convert_reverse(T&) { }
template<typename T> inline void endian_convertptr(void* val) { ByteConverter::apply<T>(val); }
template<typename T> inline void endian_converttr_reverse(void*) { }
#else
template<typename T> inline void endian_convert(T&) { }
template<typename T> inline void endian_convert_reverse(T& val) { ByteConverter::apply<T>(&val); }
template<typename T> inline void endian_convertptr(void*) { }
template<typename T> inline void endian_converttr_reverse(void* val) { ByteConverter::apply<T>(val); }
#endif

template<typename T> void endian_convert(T*);
template<typename T> void endian_convert_reverse(T*);

inline void endian_convert(uint8&) { }
inline void endian_convert(int8&) { }
inline void endian_convert_reverse(uint8&) { }
inline void endian_convert_reverse(int8&) { }

#endif //__byte_converter_h__
