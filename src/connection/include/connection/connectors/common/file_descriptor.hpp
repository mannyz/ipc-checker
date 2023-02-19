#pragma once

class FileDescriptor {
public:
  explicit FileDescriptor(int fd = -1) noexcept : _fd(fd) {}

  FileDescriptor &operator=(int fd) noexcept {
    _fd = fd;
    return *this;
  }

  FileDescriptor(FileDescriptor &&other) noexcept : _fd(other._fd) {
    other._fd = -1;
  }

  FileDescriptor &operator=(FileDescriptor &&other) noexcept {
    _fd = other._fd;
    other._fd = -1;
    return *this;
  }

  FileDescriptor(const FileDescriptor &other) = delete;
  FileDescriptor &operator=(const FileDescriptor &other) = delete;

  operator int() const noexcept { return _fd; }

private:
  int _fd;
};
