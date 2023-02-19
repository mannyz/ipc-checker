#include "connection/connectors/pipe/server.hpp"

#include <stdexcept>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

PipeServer::PipeServer(const std::string &connection_id) {
  _fifo_name = "/tmp/" + connection_id;

  auto fd = mkfifo(_fifo_name.c_str(), S_IRUSR | S_IWUSR);
  if (fd == -1 && errno != EEXIST) {
    throw std::runtime_error("mkfifo() failed");
  }

  _fifo_fd = open(_fifo_name.c_str(), O_RDWR);
  if (_fifo_fd == -1) {
    throw std::runtime_error("open() failed");
  }

  if (fcntl(_fifo_fd, F_SETFL, O_DIRECT) == -1) {
    throw std::runtime_error("fcntl() failed");
  }
}

PipeServer::~PipeServer() { Close(); }

void PipeServer::Close() {
  close(_fifo_fd);
  unlink(_fifo_name.c_str());
}