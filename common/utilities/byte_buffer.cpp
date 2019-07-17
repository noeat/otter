#include "byte_buffer.h"
#include "errors.h"
#include "message_buffer.h"
#include "log.h"
#include <sstream>
#include <time.h>
#include <ctime>

ByteBuffer::ByteBuffer(MessageBuffer&& buffer) : rpos_(0), wpos_(0), bitpos_(InitialBitPos), curbitval_(0), storage_(buffer.move())
{
}

ByteBufferPositionException::ByteBufferPositionException(size_t pos, size_t size, size_t valueSize)
{
  std::ostringstream ss;

  ss << "Attempted to get value with size: "
    << valueSize << " in ByteBuffer (pos: " << pos << " size: " << size
    << ")";

  message().assign(ss.str());
}

ByteBuffer& ByteBuffer::operator>>(float& value)
{
  value = read<float>();
  if (!std::isfinite(value))
    throw ByteBufferException();
  return *this;
}

ByteBuffer& ByteBuffer::operator>>(double& value)
{
  value = read<double>();
  if (!std::isfinite(value))
    throw ByteBufferException();
  return *this;
}

uint32 ByteBuffer::read_packed_time()
{
  uint32 packedDate = read<uint32>();
  tm lt = tm();
  lt.tm_min = packedDate & 0x3F;
  lt.tm_hour = (packedDate >> 6) & 0x1F;
  lt.tm_mday = ((packedDate >> 14) & 0x3F) + 1;
  lt.tm_mon = (packedDate >> 20) & 0xF;
  lt.tm_year = ((packedDate >> 24) & 0x1F) + 100;
  return uint32(mktime(&lt));
}

void ByteBuffer::append(const uint8 *src, size_t cnt)
{
  ASSERT(src, "Attempted to put a NULL-pointer in ByteBuffer (pos: " SZFMTD " size: " SZFMTD ")", wpos_, size());
  ASSERT(cnt, "Attempted to put a zero-sized value in ByteBuffer (pos: " SZFMTD " size: " SZFMTD ")", wpos_, size());
  ASSERT(size() < 10000000);

  flush_bits();
  storage_.insert(storage_.begin() + wpos_, src, src + cnt);
  wpos_ += cnt;
}

void ByteBuffer::append_packed_time(time_t time)
{
#ifdef _WIN32
  tm lt;
  localtime_s(&lt, &time);
#else
  tm lt;
  localtime_r(&time, &lt);
#endif
  append<uint32>((lt.tm_year - 100) << 24 | lt.tm_mon << 20 | (lt.tm_mday - 1) << 14 | lt.tm_wday << 11 | lt.tm_hour << 6 | lt.tm_min);
}

void ByteBuffer::put(size_t pos, const uint8 *src, size_t cnt)
{
  ASSERT(pos + cnt <= size(), "Attempted to put value with size: " SZFMTD " in ByteBuffer (pos: " SZFMTD " size: " SZFMTD ")", cnt, pos, size());
  ASSERT(src, "Attempted to put a NULL-pointer in ByteBuffer (pos: " SZFMTD " size: " SZFMTD ")", pos, size());
  ASSERT(cnt, "Attempted to put a zero-sized value in ByteBuffer (pos: " SZFMTD " size: " SZFMTD ")", pos, size());

  std::memcpy(&storage_[pos], src, cnt);
}

void ByteBuffer::put_bits(std::size_t pos, std::size_t value, uint32 bitCount)
{
  ASSERT(pos + bitCount <= size() * 8, "Attempted to put %u bits in ByteBuffer (bitpos: " SZFMTD " size: " SZFMTD ")", bitCount, pos, size());
  ASSERT(bitCount, "Attempted to put a zero bits in ByteBuffer");

  for (uint32 i = 0; i < bitCount; ++i)
  {
    std::size_t wp = (pos + i) / 8;
    std::size_t bit = (pos + i) % 8;
    if ((value >> (bitCount - i - 1)) & 1)
      storage_[wp] |= 1 << (7 - bit);
    else
      storage_[wp] &= ~(1 << (7 - bit));
  }
}

void ByteBuffer::print_storage() const
{
  if (!should_log("network", spdlog::level::trace))
    return;

  std::ostringstream o;
  o << "STORAGE_SIZE: " << size();
  for (uint32 i = 0; i < size(); ++i)
    o << read<uint8>(i) << " - ";
  o << " ";

  LOG_TRACE("network", "%s", o.str().c_str());
}

void ByteBuffer::textlike() const
{
  if (!should_log("network", spdlog::level::trace))
    return;

  std::ostringstream o;
  o << "STORAGE_SIZE: " << size();
  for (uint32 i = 0; i < size(); ++i)
  {
    char buf[2];
    snprintf(buf, 2, "%c", read<uint8>(i));
    o << buf;
  }
  o << " ";

  LOG_TRACE("network", "%s", o.str().c_str());
}

void ByteBuffer::hexlike() const
{
  if (!should_log("network", spdlog::level::trace))
    return;

  uint32 j = 1, k = 1;

  std::ostringstream o;
  o << "STORAGE_SIZE: " << size();

  for (uint32 i = 0; i < size(); ++i)
  {
    char buf[4];
    snprintf(buf, 4, "%2X ", read<uint8>(i));
    if ((i == (j * 8)) && ((i != (k * 16))))
    {
      o << "| ";
      ++j;
    }
    else if (i == (k * 16))
    {
      o << "\n";
      ++k;
      ++j;
    }

    o << buf;
  }
  o << " ";
  LOG_TRACE("network", "%s", o.str().c_str());
}
