#include "connection/buffer/buffer.hpp"

#include <cstddef>
#include <cstring>
#include <stdexcept>

Buffer::Buffer()
    : _buf(_buffer), _buf_end(_buffer + kBufferCapacityInBytes),
      _buf_read_ptr(_buffer), _buf_write_ptr(_buffer) {}

void Buffer::Reset() {
  _buf_read_ptr = _buffer;
  _buf_write_ptr = _buffer;
}

char *Buffer::Data() {
  return reinterpret_cast<char *>(std::addressof(_buffer));
}

const char *Buffer::Data() const { return const_cast<Buffer *>(this)->Data(); }

std::size_t Buffer::Capacity() const { return kBufferCapacityInBytes; }

std::size_t Buffer::UsedBytes() const { return _buf_write_ptr - _buf; }

void Buffer::WriteBytes(const char *data, std::size_t size) {
  if (_buf_write_ptr + size > _buf_end) {
    throw std::runtime_error("message is not fit in buffer");
  }

  memcpy(_buf_write_ptr, data, size);
  _buf_write_ptr += size;
}

void Buffer::ReadBytes(char *const data, std::size_t size) {
  if (_buf_read_ptr + size > _buf_end) {
    throw std::runtime_error("not enough message in buffer");
  }

  memcpy(data, _buf_read_ptr, size);
  _buf_read_ptr += size;
}

void Buffer::WriteBytesInReverseOrder(const char *data, std::size_t size) {
  if (_buf_write_ptr + size > _buf_end) {
    throw std::runtime_error("message is not fit in buffer");
  }

  auto buf_ptr = _buf_write_ptr;
  auto data_ptr = &data[size - 1];
  while (size--) {
    *buf_ptr++ = *data_ptr--;
  }
  _buf_write_ptr = buf_ptr;
}

void Buffer::ReadBytesInReverseOrder(char *const data, std::size_t size) {
  if (_buf_read_ptr + size > _buf_end) {
    throw std::runtime_error("not enough message in buffer");
  }

  auto buf_ptr = _buf_read_ptr;
  auto data_ptr = &data[size - 1];
  while (size--) {
    *data_ptr-- = *buf_ptr++;
  }
  _buf_read_ptr = buf_ptr;
}
