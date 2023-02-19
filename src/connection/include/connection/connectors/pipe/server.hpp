#pragma once

#include "interface/connection/ibuffer.hpp"

#include "connection/connectors/common/file_descriptor.hpp"

class PipeServer {
public:
  PipeServer(const std::string &connection_id);
  PipeServer(PipeServer &&) = default;
  ~PipeServer();

public:
  template <typename TBufferImpl> void Receive(IBuffer<TBufferImpl> &buffer);

private:
  FileDescriptor _fifo_fd;
  std::string _fifo_name;

private:
  void Close();
};
