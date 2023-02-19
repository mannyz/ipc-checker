#pragma once

#include <cstddef>

class Buffer {
public:
  static constexpr std::size_t kBufferCapacityInBytes = 10000;

public:
  Buffer();

public:
  void Reset();

public:
  char *Data();
  const char *Data() const;
  std::size_t Capacity() const;
  std::size_t UsedBytes() const;

public:
  void ReadBytes(char *const data, std::size_t size);
  void WriteBytes(const char *data, std::size_t size);

public:
  void ReadBytesInReverseOrder(char *const data, std::size_t size);
  void WriteBytesInReverseOrder(const char *data, std::size_t size);

private:
  char _buffer[kBufferCapacityInBytes];

  char *_buf;
  char *_buf_end;
  char *_buf_read_ptr;
  char *_buf_write_ptr;
};
