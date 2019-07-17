#ifndef __message_buffer_h__
#define __message_buffer_h__
#include "define.h"
#include <vector>
#include <cstring>

class MessageBuffer
{
  typedef std::vector<uint8>::size_type size_type;

public:
  MessageBuffer() : wpos_(0), rpos_(0), storage_()
  {
    storage_.resize(1024);
  }

  explicit MessageBuffer(std::size_t initialSize) : wpos_(0), rpos_(0), storage_()
  {
    storage_.resize(initialSize);
  }

  MessageBuffer(MessageBuffer const& right) : wpos_(right.wpos_), rpos_(right.rpos_), storage_(right.storage_)
  {
  }

  MessageBuffer(MessageBuffer&& right) noexcept : wpos_(right.wpos_), rpos_(right.rpos_), storage_(right.move()) { }

  void reset()
  {
    wpos_ = 0;
    rpos_ = 0;
  }

  void resize(size_type bytes)
  {
    storage_.resize(bytes);
  }

  uint8* get_base_pointer() { return storage_.data(); }

  uint8* get_read_pointer() { return get_base_pointer() + rpos_; }

  uint8* get_write_pointer() { return get_base_pointer() + wpos_; }

  void read_completed(size_type bytes) { rpos_ += bytes; }

  void write_completed(size_type bytes) { wpos_ += bytes; }

  size_type get_active_size() const { return wpos_ - rpos_; }

  size_type get_remaining_space() const { return storage_.size() - wpos_; }

  size_type get_buffer_size() const { return storage_.size(); }

  void normalize()
  {
    if (rpos_)
    {
      if (rpos_ != wpos_)
        memmove(get_base_pointer(), get_read_pointer(), get_active_size());
      wpos_ -= rpos_;
      rpos_ = 0;
    }
  }

  void ensure_free_space()
  {
    if (get_remaining_space() == 0)
      storage_.resize(storage_.size() * 3 / 2);
  }

  void write(void const* data, std::size_t size)
  {
    if (size)
    {
      memcpy(get_write_pointer(), data, size);
      write_completed(size);
    }
  }

  std::vector<uint8>&& move()
  {
    wpos_ = 0;
    rpos_ = 0;
    return std::move(storage_);
  }

  MessageBuffer& operator=(MessageBuffer const& right)
  {
    if (this != &right)
    {
      wpos_ = right.wpos_;
      rpos_ = right.rpos_;
      storage_ = right.storage_;
    }

    return *this;
  }

  MessageBuffer& operator=(MessageBuffer&& right) noexcept
  {
    if (this != &right)
    {
      wpos_ = right.wpos_;
      rpos_ = right.rpos_;
      storage_ = right.move();
    }

    return *this;
  }

private:
  size_type wpos_;
  size_type rpos_;
  std::vector<uint8> storage_;
};

#endif /* __message_buffer_h__ */
