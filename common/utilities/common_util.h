#pragma once
#ifndef __common_util_h__
#define __common_util_h__
#include "define.h"
#include <vector>
#include <string>

class Tokenizer
{
public:
  typedef std::vector<char const*> StorageType;

  typedef StorageType::size_type size_type;

  typedef StorageType::const_iterator const_iterator;
  typedef StorageType::reference reference;
  typedef StorageType::const_reference const_reference;

public:
  Tokenizer(const std::string& src, char const sep, uint32 vectorReserve = 0, bool keepEmptyStrings = true);
  ~Tokenizer() { delete[] str_; }

  const_iterator begin() const { return storage_.begin(); }
  const_iterator end() const { return storage_.end(); }

  size_type size() const { return storage_.size(); }

  reference operator [] (size_type i) { return storage_[i]; }
  const_reference operator [] (size_type i) const { return storage_[i]; }

private:
  char* str_;
  StorageType storage_;
};

#endif //__common_util_h__