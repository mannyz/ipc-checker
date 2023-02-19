#pragma once

#include "interface/connection/ibuffer.hpp"

#include "connection/connectors/common/file_descriptor.hpp"

class NetSocketServer {
public:
  NetSocketServer(int port);
  NetSocketServer(NetSocketServer &&) = default;
  ~NetSocketServer();

public:
  void Accept();

  void SetBufferSize(std::size_t buffer_size);
  std::size_t GetBufferSize() const;

public:
  template <typename TBufferImpl> void Receive(IBuffer<TBufferImpl> &buffer);

private:
  FileDescriptor _inet_socket_fd;

private:
  void Close();
};
