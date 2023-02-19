#pragma once

#include "server.hpp"

#include <unistd.h>

template <typename TBufferImpl>
void NetSocketServer::Receive(IBuffer<TBufferImpl> &buffer) {
  int64_t read_bytes = read(_inet_socket_fd, buffer.Data(), buffer.Capacity());
  if (read_bytes == -1) {
    throw std::runtime_error("read() failed");
  }
}
