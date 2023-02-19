#include "connection/connectors/pipe/client.hpp"

#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>

PipeClient::PipeClient(const std::string &connection_id) {
  _fifo_name = "/tmp/" + connection_id;
}

PipeClient::~PipeClient() { Close(); }

void PipeClient::Close() { close(_fifo_fd); }

bool PipeClient::Connect() {
  _fifo_fd = open(_fifo_name.c_str(), O_RDWR);
  if (_fifo_fd == -1) {
    return false;
  }

  if (fcntl(_fifo_fd, F_SETFL, O_DIRECT) == -1) {
    throw std::runtime_error("fcntl() failed");
  }

  return true;
}

std::string PipeClient::GetConnectionAddress() const {
  return "pipe:/" + _fifo_name;
}

void PipeClient::SetBufferSize(std::size_t buffer_size) {
  if (fcntl(_fifo_fd, F_SETPIPE_SZ, buffer_size) == -1) {
    throw std::runtime_error("fcntl() failed");
  }
}

std::size_t PipeClient::GetBufferSize() const {
  auto buffer_size = fcntl(_fifo_fd, F_GETPIPE_SZ);
  if (buffer_size == -1) {
    throw std::runtime_error("fcntl() failed");
  }

  return buffer_size;
}
