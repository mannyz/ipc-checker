#pragma once

#include "client.hpp"

#include <unistd.h>

template <typename TBufferImpl>
void PipeClient::Send(const IBuffer<TBufferImpl> &buffer) {
  int64_t write_bytes = write(_fifo_fd, buffer.Data(), buffer.UsedBytes());
  if (write_bytes != buffer.UsedBytes()) {
    throw std::runtime_error("write() failed");
  }
}
