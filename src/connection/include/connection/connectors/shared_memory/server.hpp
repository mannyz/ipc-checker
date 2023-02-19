#pragma once

#include <optional>
#include <string>

#include "interface/connection/ibuffer.hpp"

#include "connection/connectors/common/file_descriptor.hpp"

class SPSCRingBuffer;

class SharedMemoryServer {
  static constexpr std::size_t kDefaultBufferSizeInBytes = 4096;

public:
  SharedMemoryServer(std::string connection_id,
                     std::optional<std::size_t> buffer_size = std::nullopt);
  SharedMemoryServer(SharedMemoryServer &&) = default;
  ~SharedMemoryServer();

public:
  std::size_t GetBufferSize() const;

public:
  template <typename TBufferImpl> void Receive(IBuffer<TBufferImpl> &buffer);

private:
  FileDescriptor _shmem_fd;
  std::string _shmem_name;
  std::size_t _shmem_size;
  void *_shmem = nullptr;
  SPSCRingBuffer *_rbh = nullptr;

private:
  void Close();
};
