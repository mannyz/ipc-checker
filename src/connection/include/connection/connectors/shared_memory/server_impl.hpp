#pragma once

#include "server.hpp"

#include "ring_buffer.hpp"

template <typename TBufferImpl>
void SharedMemoryServer::Receive(IBuffer<TBufferImpl> &buffer) {
  _rbh->Pop(buffer.Data(), buffer.Capacity());
}
