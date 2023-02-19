#pragma once

#include "interface/connection/ibuffer.hpp"

#include "connection/connectors/common/file_descriptor.hpp"

class PipeClient {
public:
  PipeClient(const std::string &connection_id);
  PipeClient(PipeClient &&) = default;
  ~PipeClient();

public:
  bool Connect();
  std::string GetConnectionAddress() const;

  void SetBufferSize(std::size_t buffer_size);
  std::size_t GetBufferSize() const;

public:
  template <typename TBufferImpl> void Send(const IBuffer<TBufferImpl> &buffer);

private:
  FileDescriptor _fifo_fd;
  std::string _fifo_name;

private:
  void Close();
};
