#pragma once

#include "interface/connection/ibuffer.hpp"

#include "connection/connectors/common/file_descriptor.hpp"

class UnixSocketServer {
public:
  UnixSocketServer(const std::string &connection_id);
  UnixSocketServer(UnixSocketServer &&) = default;
  ~UnixSocketServer();

public:
  void Accept();

public:
  template <typename TBufferImpl> void Receive(IBuffer<TBufferImpl> &buffer);

private:
  FileDescriptor _ux_socket_fd;
  std::string _ux_socket_name;

private:
  void Close();
};
