#include "connection/connectors/shared_memory/client.hpp"

#include <stdexcept>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "connection/connectors/shared_memory/ring_buffer.hpp"

SharedMemoryClient::SharedMemoryClient(std::string connection_id) {
  _shmem_name = std::move(connection_id);
}

SharedMemoryClient::~SharedMemoryClient() { Close(); }

void SharedMemoryClient::Close() { close(_shmem_fd); }

bool SharedMemoryClient::Connect() {
  _shmem_fd = shm_open(_shmem_name.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
  if (_shmem_fd == -1) {
    if (errno == ENOENT) {
      return false;
    }

    throw std::runtime_error("shm_open() failed");
  }

  struct stat sb;
  if (fstat(_shmem_fd, &sb) == -1) {
    throw std::runtime_error("fstat() failed");
  }

  if (sb.st_size == 0) {
    return false;
  }

  _shmem_size = sb.st_size;

  _shmem =
      mmap(NULL, _shmem_size, PROT_READ | PROT_WRITE, MAP_SHARED, _shmem_fd, 0);

  if (_shmem == MAP_FAILED) {
    throw std::runtime_error("mmap() failed");
  }

  _rbh = static_cast<SPSCRingBuffer *>(_shmem);

  _rbh->WaitInitialized();
  assert(_rbh->IsInitialized());

  return _rbh->IsInitialized();
}

std::string SharedMemoryClient::GetConnectionAddress() const {
  return "shmem:/" + _shmem_name;
}
