#pragma once

#include "interface/connection/ibuffer.hpp"

#include "connection/connectors/common/file_descriptor.hpp"

class UnixSocketClient {
public:
  UnixSocketClient(const std::string &connection_id);
  UnixSocketClient(UnixSocketClient &&) = default;
  ~UnixSocketClient();

public:
  bool Connect();
  std::string GetConnectionAddress() const;

  void SetBufferSize(std::size_t buffer_size);
  std::size_t GetBufferSize() const;

public:
  template <typename TBufferImpl> void Send(const IBuffer<TBufferImpl> &buffer);

private:
  FileDescriptor _ux_socket_fd;
  std::string _ux_socket_name;

private:
  void Close();
};
