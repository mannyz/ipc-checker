#include "connection/connectors/unix_socket/server.hpp"

#include <stdexcept>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

UnixSocketServer::UnixSocketServer(const std::string &connection_id) {
  _ux_socket_name = "/tmp/" + connection_id;

  _ux_socket_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (_ux_socket_fd == -1) {
    throw std::runtime_error("socket() failed");
  }

  sockaddr_un ux_socket_addr;
  ux_socket_addr.sun_family = AF_UNIX;
  strncpy(ux_socket_addr.sun_path, _ux_socket_name.c_str(),
          sizeof(ux_socket_addr.sun_path) - 1);

  if (bind(_ux_socket_fd, reinterpret_cast<sockaddr *>(&ux_socket_addr),
           sizeof(sockaddr_un)) == -1) {
    throw std::runtime_error("bind() failed");
  }

  if (listen(_ux_socket_fd, 0) == -1) {
    throw std::runtime_error("listen() failed");
  }
}

UnixSocketServer::~UnixSocketServer() { Close(); }

void UnixSocketServer::Accept() {
  auto client_socket_fd = accept(_ux_socket_fd, NULL, NULL);
  if (client_socket_fd == -1) {
    throw std::runtime_error("accept() failed");
  }

  if (close(_ux_socket_fd)) {
    throw std::runtime_error("close() failed");
  }

  _ux_socket_fd = client_socket_fd;
}

void UnixSocketServer::Close() {
  close(_ux_socket_fd);
  unlink(_ux_socket_name.c_str());
}