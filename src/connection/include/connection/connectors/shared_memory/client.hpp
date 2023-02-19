#pragma once

#include "interface/connection/ibuffer.hpp"

#include "connection/connectors/common/file_descriptor.hpp"

class SPSCRingBuffer;

class SharedMemoryClient {
public:
  SharedMemoryClient(std::string connection_id);
  SharedMemoryClient(SharedMemoryClient &&) = default;
  ~SharedMemoryClient();

public:
  bool Connect();
  std::string GetConnectionAddress() const;

public:
  template <typename TBufferImpl> void Send(const IBuffer<TBufferImpl> &buffer);

private:
  FileDescriptor _shmem_fd;
  std::string _shmem_name;
  std::size_t _shmem_size;
  void *_shmem = nullptr;
  SPSCRingBuffer *_rbh;

private:
  void Close();
};
