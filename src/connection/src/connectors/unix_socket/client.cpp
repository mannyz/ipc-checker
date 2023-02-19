#include "connection/connectors/unix_socket/client.hpp"

#include <stdexcept>

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

UnixSocketClient::UnixSocketClient(const std::string &connection_id) {
  _ux_socket_name = "/tmp/" + connection_id;
  _ux_socket_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (_ux_socket_fd == -1) {
    throw std::runtime_error("socket() failed");
  }
}

UnixSocketClient::~UnixSocketClient() { Close(); }

void UnixSocketClient::Close() { close(_ux_socket_fd); }

bool UnixSocketClient::Connect() {
  auto fd = open(_ux_socket_name.c_str(), O_RDWR);
  if (fd == -1 && errno != ENXIO) {
    return false;
  }

  sockaddr_un ux_socket_addr;
  ux_socket_addr.sun_family = AF_UNIX;
  strncpy(ux_socket_addr.sun_path, _ux_socket_name.c_str(),
          sizeof(ux_socket_addr.sun_path) - 1);

  if (connect(_ux_socket_fd, (const sockaddr *)&ux_socket_addr,
              sizeof(ux_socket_addr)) == -1) {
    return false;
  }

  return true;
}

std::string UnixSocketClient::GetConnectionAddress() const {
  return "uxsock:/" + _ux_socket_name;
}

void UnixSocketClient::SetBufferSize(std::size_t buffer_size) {
  if (setsockopt(_ux_socket_fd, SOL_SOCKET, SO_SNDBUF, &buffer_size,
                 sizeof(buffer_size)) == -1) {
    throw std::runtime_error("setsockopt() failed");
  }
}

std::size_t UnixSocketClient::GetBufferSize() const {
  unsigned int actual_buffer_size = 0,
               actual_buffer_size_len = sizeof(actual_buffer_size);

  if (getsockopt(_ux_socket_fd, SOL_SOCKET, SO_SNDBUF, &actual_buffer_size,
                 &actual_buffer_size_len) == -1) {
    throw std::runtime_error("getsockopt() failed");
  }

  return actual_buffer_size;
}