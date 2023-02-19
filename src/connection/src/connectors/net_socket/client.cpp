#include "connection/connectors/net_socket/client.hpp"

#include <stdexcept>
#include <string>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <sys/socket.h>
#include <unistd.h>

NetSocketClient::NetSocketClient(std::string host, int port)
    : _host(std::move(host)), _port(port) {
  _inet_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
  if (_inet_socket_fd == -1) {
    throw std::runtime_error("socket() failed");
  }
}

NetSocketClient::~NetSocketClient() { Close(); }

void NetSocketClient::Close() { close(_inet_socket_fd); }

bool NetSocketClient::Connect() {
  auto server = gethostbyname(_host.c_str());
  if (server == NULL) {
    throw std::runtime_error("gethostbyname() failed");
  }

  sockaddr_in serv_addr;
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
        server->h_length);
  serv_addr.sin_port = htons(_port);

  if (connect(_inet_socket_fd, (const sockaddr *)&serv_addr,
              sizeof(serv_addr)) == -1) {
    return false;
  }

  return true;
}

std::string NetSocketClient::GetConnectionAddress() const {
  return "netsock://" + _host + ":" + std::to_string(_port);
}

void NetSocketClient::SetBufferSize(std::size_t buffer_size) {
  if (setsockopt(_inet_socket_fd, SOL_SOCKET, SO_SNDBUF, &buffer_size,
                 sizeof(buffer_size)) == -1) {
    throw std::runtime_error("setsockopt() failed");
  }
}

std::size_t NetSocketClient::GetBufferSize() const {
  unsigned int actual_buffer_size = 0,
               actual_buffer_size_len = sizeof(actual_buffer_size);

  if (getsockopt(_inet_socket_fd, SOL_SOCKET, SO_SNDBUF, &actual_buffer_size,
                 &actual_buffer_size_len) == -1) {
    throw std::runtime_error("getsockopt() failed");
  }

  return actual_buffer_size;
}

void NetSocketClient::SetNoDelay() {
  int flag = 1;
  if (setsockopt(_inet_socket_fd, IPPROTO_SCTP, SCTP_NODELAY, &flag,
                 sizeof(flag)) == -1) {
    throw std::runtime_error("setsockopt() failed");
  }
}

bool NetSocketClient::IsNoDelay() const {
  int flag = -1;
  unsigned int flag_len = sizeof(flag);
  if (getsockopt(_inet_socket_fd, IPPROTO_SCTP, SCTP_NODELAY, &flag,
                 &flag_len) == -1) {
    throw std::runtime_error("setsockopt() failed");
  }

  return flag;
}
