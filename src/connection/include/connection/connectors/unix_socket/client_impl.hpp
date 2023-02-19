#pragma once

#include "client.hpp"

#include <unistd.h>

template <typename TBufferImpl>
void UnixSocketClient::Send(const IBuffer<TBufferImpl> &buffer) {
  int64_t write_bytes = write(_ux_socket_fd, buffer.Data(), buffer.UsedBytes());
  if (write_bytes != buffer.UsedBytes()) {
    throw std::runtime_error("write() failed");
  }
}
