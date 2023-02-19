#pragma once

#include "interface/connection/ibuffer.hpp"

#include "connection/connectors/common/file_descriptor.hpp"

class NetSocketClient {
public:
  NetSocketClient(std::string host, int port);
  NetSocketClient(NetSocketClient &&) = default;
  ~NetSocketClient();

public:
  bool Connect();
  std::string GetConnectionAddress() const;

  void SetBufferSize(std::size_t buffer_size);
  std::size_t GetBufferSize() const;

  void SetNoDelay();
  bool IsNoDelay() const;

public:
  template <typename TBufferImpl> void Send(const IBuffer<TBufferImpl> &buffer);

private:
  FileDescriptor _inet_socket_fd;
  std::string _host;
  int _port;

private:
  void Close();
};
