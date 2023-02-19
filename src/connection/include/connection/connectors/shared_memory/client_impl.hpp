#pragma once

#include "client.hpp"

#include "ring_buffer.hpp"

template <typename TBufferImpl>
void SharedMemoryClient::Send(const IBuffer<TBufferImpl> &buffer) {
  _rbh->Push(buffer.Data(), buffer.UsedBytes());
}
