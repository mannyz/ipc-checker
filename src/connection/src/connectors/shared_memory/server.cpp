#include "connection/connectors/shared_memory/server.hpp"

#include <stdexcept>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "connection/connectors/shared_memory/ring_buffer.hpp"

SharedMemoryServer::SharedMemoryServer(std::string connection_id,
                                       std::optional<std::size_t> buffer_size) {
  _shmem_name = std::move(connection_id);

  _shmem_fd = shm_open(_shmem_name.c_str(), O_CREAT | O_EXCL | O_RDWR,
                       S_IRUSR | S_IWUSR);
  if (_shmem_fd == -1) {
    throw std::runtime_error("shm_open() failed");
  }

  auto aligned_buffer_size = SPSCRingBuffer::AlignCapacity(
      buffer_size.value_or(kDefaultBufferSizeInBytes));
  _shmem_size = sizeof(SPSCRingBuffer) + aligned_buffer_size;

  if (ftruncate(_shmem_fd, _shmem_size) == -1) {
    throw std::runtime_error("ftruncate() failed");
  }

  _shmem =
      mmap(NULL, _shmem_size, PROT_READ | PROT_WRITE, MAP_SHARED, _shmem_fd, 0);
  if (_shmem == MAP_FAILED) {
    throw std::runtime_error("mmap() failed");
  }

  _rbh = new (_shmem) SPSCRingBuffer(aligned_buffer_size);
  _rbh->SetInitialized();
}

SharedMemoryServer::~SharedMemoryServer() {
  _rbh->~SPSCRingBuffer();
  Close();
}

std::size_t SharedMemoryServer::GetBufferSize() const {
  return _rbh->GetCapacity();
}

void SharedMemoryServer::Close() {
  close(_shmem_fd);
  shm_unlink(_shmem_name.c_str());
}