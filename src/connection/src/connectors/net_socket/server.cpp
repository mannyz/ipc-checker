#include "connection/connectors/net_socket/server.hpp"

#include <stdexcept>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

NetSocketServer::NetSocketServer(int port) {
  _inet_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
  if (_inet_socket_fd == -1) {
    throw std::runtime_error("socket() failed");
  }

  sockaddr_in serv_addr;
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(_inet_socket_fd, reinterpret_cast<sockaddr *>(&serv_addr),
           sizeof(serv_addr)) == -1) {
    throw std::runtime_error("bind() failed");
  }

  if (listen(_inet_socket_fd, 1) == -1) {
    throw std::runtime_error("listen() failed");
  }
}

NetSocketServer::~NetSocketServer() { Close(); }

void NetSocketServer::Accept() {
  auto client_socket_fd = accept(_inet_socket_fd, nullptr, nullptr);
  if (client_socket_fd == -1) {
    throw std::runtime_error("accept() failed");
  }

  if (close(_inet_socket_fd)) {
    throw std::runtime_error("close() failed");
  }

  _inet_socket_fd = client_socket_fd;
}

void NetSocketServer::SetBufferSize(std::size_t buffer_size) {
  if (setsockopt(_inet_socket_fd, SOL_SOCKET, SO_RCVBUF, &buffer_size,
                 sizeof(buffer_size)) == -1) {
    throw std::runtime_error("setsockopt() failed");
  }
}

std::size_t NetSocketServer::GetBufferSize() const {
  unsigned int actual_buffer_size = 0,
               actual_buffer_size_len = sizeof(actual_buffer_size);

  if (getsockopt(_inet_socket_fd, SOL_SOCKET, SO_RCVBUF, &actual_buffer_size,
                 &actual_buffer_size_len) == -1) {
    throw std::runtime_error("getsockopt() failed");
  }

  return actual_buffer_size;
}

void NetSocketServer::Close() { close(_inet_socket_fd); }