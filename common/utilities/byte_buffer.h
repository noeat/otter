#ifndef __byte_buffer_h__
#define __byte_buffer_h__
#include "define.h"
#include "byte_converter.h"
#include <string>
#include <vector>
#include <cstring>

class MessageBuffer;
class ByteBufferException : public std::exception
{
public:
  ~ByteBufferException() noexcept { }

  char const* what() const throw() override { return msg_.c_str(); }

protected:
  std::string & message() throw() { return msg_; }

private:
  std::string msg_;
};

class ByteBufferPositionException : public ByteBufferException
{
public:
  ByteBufferPositionException(size_t pos, size_t size, size_t valueSize);

  ~ByteBufferPositionException() noexcept { }
};

class ByteBuffer
{
public:
  static size_t const DEFAULT_SIZE = 0x1000;
  static uint8 const InitialBitPos = 8;


  ByteBuffer() : rpos_(0), wpos_(0), bitpos_(InitialBitPos), curbitval_(0)
  {
    storage_.reserve(DEFAULT_SIZE);
  }

  ByteBuffer(size_t reserve) : rpos_(0), wpos_(0), bitpos_(InitialBitPos), curbitval_(0)
  {
    storage_.reserve(reserve);
  }

  ByteBuffer(ByteBuffer&& buf) noexcept : rpos_(buf.rpos_), wpos_(buf.wpos_),
    bitpos_(buf.bitpos_), curbitval_(buf.curbitval_), storage_(buf.move()) { }

  ByteBuffer(ByteBuffer const& right) : rpos_(right.rpos_), wpos_(right.wpos_),
    bitpos_(right.bitpos_), curbitval_(right.curbitval_), storage_(right.storage_) { }

  ByteBuffer(MessageBuffer&& buffer);

  std::vector<uint8>&& move() noexcept
  {
    rpos_ = 0;
    wpos_ = 0;
    bitpos_ = InitialBitPos;
    curbitval_ = 0;
    return std::move(storage_);
  }

  ByteBuffer& operator=(ByteBuffer const& right)
  {
    if (this != &right)
    {
      rpos_ = right.rpos_;
      wpos_ = right.wpos_;
      bitpos_ = right.bitpos_;
      curbitval_ = right.curbitval_;
      storage_ = right.storage_;
    }

    return *this;
  }

  ByteBuffer& operator=(ByteBuffer&& right)
  {
    if (this != &right)
    {
      rpos_ = right.rpos_;
      wpos_ = right.wpos_;
      bitpos_ = right.bitpos_;
      curbitval_ = right.curbitval_;
      storage_ = right.move();
    }
    return *this;
  }

  virtual ~ByteBuffer() { }

  void clear()
  {
    rpos_ = 0;
    wpos_ = 0;
    bitpos_ = InitialBitPos;
    curbitval_ = 0;
    storage_.clear();
  }
  template <typename T>
  void append(T value)
  {
    static_assert(std::is_trivially_copyable<T>::value, "append(T) must be used with trivially copyable types");
    endian_convert(value);
    append((uint8 *)&value, sizeof(value));
  }

  bool has_unfinished_bitpack() const
  {
    return bitpos_ != 8;
  }

  void flush_bits()
  {
    if (bitpos_ == 8)
      return;

    bitpos_ = 8;
    append((uint8 *)&curbitval_, sizeof(uint8));
    curbitval_ = 0;
  }

  void reset_bitpos()
  {
    if (bitpos_ > 7)
      return;

    bitpos_ = 8;
    curbitval_ = 0;
  }

  bool write_bit(bool bit)
  {
    --bitpos_;
    if (bit)   curbitval_ |= (1 << (bitpos_));

    if (bitpos_ == 0)
    {
      bitpos_ = 8;
      append((uint8 *)&curbitval_, sizeof(curbitval_));
      curbitval_ = 0;
    }

    return bit;
  }

  bool read_bit()
  {
    ++bitpos_;
    if (bitpos_ > 7)
    {
      curbitval_ = read<uint8>();
      bitpos_ = 0;
    }

    return ((curbitval_ >> (7 - bitpos_)) & 1) != 0;
  }

  void write_bits(std::size_t value, int32 bits)
  {
    for (int32 i = bits - 1; i >= 0; --i)
      write_bit((value >> i) & 1);
  }

  uint32 read_bits(int32 bits)
  {
    uint32 value = 0;
    for (int32 i = bits - 1; i >= 0; --i)
      if (read_bit())
        value |= (1 << (i));

    return value;
  }

  template <typename T>
  void put(std::size_t pos, T value)
  {
    static_assert(std::is_trivially_copyable<T>::value, "put(size_t, T) must be used with trivially copyable types");
    endian_convert(value);
    put(pos, (uint8 *)&value, sizeof(value));
  }

  /**
    * @name   put_bits
    * @brief  Places specified amount of bits of value at specified position in packet.
    *         To ensure all bits are correctly written, only call this method after
    *         bit flush has been performed

    * @param  pos Position to place the value at, in bits. The entire value must fit in the packet
    *             It is advised to obtain the position using bitwpos() function.

    * @param  value Data to write.
    * @param  bitCount Number of bits to store the value on.
  */
  void put_bits(std::size_t pos, std::size_t value, uint32 bitCount);

  ByteBuffer &operator<<(uint8 value)
  {
    append<uint8>(value);
    return *this;
  }

  ByteBuffer &operator<<(uint16 value)
  {
    append<uint16>(value);
    return *this;
  }

  ByteBuffer &operator<<(uint32 value)
  {
    append<uint32>(value);
    return *this;
  }

  ByteBuffer &operator<<(uint64 value)
  {
    append<uint64>(value);
    return *this;
  }

  // signed as in 2e complement
  ByteBuffer &operator<<(int8 value)
  {
    append<int8>(value);
    return *this;
  }

  ByteBuffer &operator<<(int16 value)
  {
    append<int16>(value);
    return *this;
  }

  ByteBuffer &operator<<(int32 value)
  {
    append<int32>(value);
    return *this;
  }

  ByteBuffer &operator<<(int64 value)
  {
    append<int64>(value);
    return *this;
  }

  // floating points
  ByteBuffer &operator<<(float value)
  {
    append<float>(value);
    return *this;
  }

  ByteBuffer &operator<<(double value)
  {
    append<double>(value);
    return *this;
  }

  ByteBuffer &operator<<(const std::string &value)
  {
    if (size_t len = value.length())
      append((uint8 const*)value.c_str(), len);
    append<uint8>(0);
    return *this;
  }

  ByteBuffer &operator<<(const char *str)
  {
    if (size_t len = (str ? strlen(str) : 0))
      append((uint8 const*)str, len);
    append<uint8>(0);
    return *this;
  }

  ByteBuffer &operator>>(bool &value)
  {
    value = read<char>() > 0 ? true : false;
    return *this;
  }

  ByteBuffer &operator>>(uint8 &value)
  {
    value = read<uint8>();
    return *this;
  }

  ByteBuffer &operator>>(uint16 &value)
  {
    value = read<uint16>();
    return *this;
  }

  ByteBuffer &operator>>(uint32 &value)
  {
    value = read<uint32>();
    return *this;
  }

  ByteBuffer &operator>>(uint64 &value)
  {
    value = read<uint64>();
    return *this;
  }

  //signed as in 2e complement
  ByteBuffer &operator>>(int8 &value)
  {
    value = read<int8>();
    return *this;
  }

  ByteBuffer &operator>>(int16 &value)
  {
    value = read<int16>();
    return *this;
  }

  ByteBuffer &operator>>(int32 &value)
  {
    value = read<int32>();
    return *this;
  }

  ByteBuffer &operator>>(int64 &value)
  {
    value = read<int64>();
    return *this;
  }

  ByteBuffer &operator>>(float &value);
  ByteBuffer &operator>>(double &value);

  ByteBuffer &operator>>(std::string& value)
  {
    value.clear();
    while (rpos() < size())
    {
      char c = read<char>();
      if (c == 0)
        break;
      value += c;
    }
    return *this;
  }

  uint8& operator[](size_t const pos)
  {
    if (pos >= size())
      throw ByteBufferPositionException(pos, 1, size());
    return storage_[pos];
  }

  uint8 const& operator[](size_t const pos) const
  {
    if (pos >= size())
      throw ByteBufferPositionException(pos, 1, size());
    return storage_[pos];
  }

  size_t rpos() const { return rpos_; }

  size_t rpos(size_t rpos_)
  {
    rpos_ = rpos_;
    return rpos_;
  }

  void rfinish()
  {
    rpos_ = wpos();
  }

  size_t wpos() const { return wpos_; }

  size_t wpos(size_t wpos_)
  {
    wpos_ = wpos_;
    return wpos_;
  }

  /// Returns position of last written bit
  size_t bitwpos() const { return wpos_ * 8 + 8 - bitpos_; }

  size_t bitwpos(size_t newPos)
  {
    wpos_ = newPos / 8;
    bitpos_ = 8 - (newPos % 8);
    return wpos_ * 8 + 8 - bitpos_;
  }

  template<typename T>
  void read_skip() { read_skip(sizeof(T)); }

  void read_skip(size_t skip)
  {
    if (rpos_ + skip > size())
      throw ByteBufferPositionException(rpos_, skip, size());

    reset_bitpos();
    rpos_ += skip;
  }

  template <typename T>
  T read()
  {
    reset_bitpos();
    T r = read<T>(rpos_);
    rpos_ += sizeof(T);
    return r;
  }

  template <typename T>
  T read(size_t pos) const
  {
    if (pos + sizeof(T) > size())
      throw ByteBufferPositionException(pos, sizeof(T), size());
    T val = *((T const*)&storage_[pos]);
    endian_convert(val);
    return val;
  }

  template<class T>
  void read(T* dest, size_t count)
  {
    static_assert(std::is_trivially_copyable<T>::value, "read(T*, size_t) must be used with trivially copyable types");
    return read(reinterpret_cast<uint8*>(dest), count * sizeof(T));
  }

  void read(uint8 *dest, size_t len)
  {
    if (rpos_ + len > size())
      throw ByteBufferPositionException(rpos_, len, size());

    reset_bitpos();
    std::memcpy(dest, &storage_[rpos_], len);
    rpos_ += len;
  }

  void read_packed_uint64(uint64& guid)
  {
    guid = 0;
    read_packed_uint64(read<uint8>(), guid);
  }

  void read_packed_uint64(uint8 mask, uint64& value)
  {
    for (uint32 i = 0; i < 8; ++i)
      if (mask & (uint8(1) << i))
        value |= (uint64(read<uint8>()) << (i * 8));
  }

  std::string read_string(uint32 length)
  {
    if (rpos_ + length > size())
      throw ByteBufferPositionException(rpos_, length, size());

    reset_bitpos();
    if (!length)
      return std::string();

    std::string str((char const*)&storage_[rpos_], length);
    rpos_ += length;
    return str;
  }

  //! Method for writing strings that have their length sent separately in packet
  //! without null-terminating the string
  void write_string(std::string const& str)
  {
    if (size_t len = str.length())
      append(str.c_str(), len);
  }

  void write_string(char const* str, size_t len)
  {
    if (len)
      append(str, len);
  }

  uint32 read_packed_time();

  uint8* contents()
  {
    if (storage_.empty())
      throw ByteBufferException();
    return storage_.data();
  }

  uint8 const* contents() const
  {
    if (storage_.empty())
      throw ByteBufferException();
    return storage_.data();
  }

  size_t size() const { return storage_.size(); }
  bool empty() const { return storage_.empty(); }

  void resize(size_t newsize)
  {
    storage_.resize(newsize, 0);
    rpos_ = 0;
    wpos_ = size();
  }

  void reserve(size_t ressize)
  {
    if (ressize > size())
      storage_.reserve(ressize);
  }

  void append(const char *src, size_t cnt)
  {
    return append((const uint8 *)src, cnt);
  }

  template<class T>
  void append(const T *src, size_t cnt)
  {
    return append((const uint8 *)src, cnt * sizeof(T));
  }

  void append(const uint8 *src, size_t cnt);

  void append(const ByteBuffer& buffer)
  {
    if (!buffer.empty())
      append(buffer.contents(), buffer.size());
  }

  void append_pack_xyz(float x, float y, float z)
  {
    uint32 packed = 0;
    packed |= ((int)(x / 0.25f) & 0x7FF);
    packed |= ((int)(y / 0.25f) & 0x7FF) << 11;
    packed |= ((int)(z / 0.25f) & 0x3FF) << 22;
    *this << packed;
  }

  void append_packed_uint64(uint64 guid)
  {
    uint8 mask = 0;
    size_t pos = wpos();
    *this << uint8(mask);

    uint8 packed[8];
    if (size_t packedSize = pack_uint64(guid, &mask, packed))
      append(packed, packedSize);

    put<uint8>(pos, mask);
  }

  static size_t pack_uint64(uint64 value, uint8* mask, uint8* result)
  {
    size_t resultSize = 0;
    *mask = 0;
    memset(result, 0, 8);

    for (uint8 i = 0; value != 0; ++i)
    {
      if (value & 0xFF)
      {
        *mask |= uint8(1 << i);
        result[resultSize++] = uint8(value & 0xFF);
      }

      value >>= 8;
    }

    return resultSize;
  }

  void append_packed_time(time_t time);

  void put(size_t pos, const uint8 *src, size_t cnt);

  void print_storage() const;

  void textlike() const;

  void hexlike() const;

protected:
  size_t rpos_, wpos_, bitpos_;
  uint8 curbitval_;
  std::vector<uint8> storage_;
};

template<> inline std::string ByteBuffer::read<std::string>()
{
  std::string tmp;
  *this >> tmp;
  return tmp;
}

template<>
inline void ByteBuffer::read_skip<char*>()
{
  std::string temp;
  *this >> temp;
}

template<>
inline void ByteBuffer::read_skip<char const*>()
{
  read_skip<char*>();
}

template<>
inline void ByteBuffer::read_skip<std::string>()
{
  read_skip<char*>();
}

#endif //__byte_buffer_h__
