#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

#include "cache_traits.hpp"

class SPSCRingBuffer {
public:
  static uint32_t AlignCapacity(uint32_t capacity);

public:
  SPSCRingBuffer(uint32_t capacity);
  ~SPSCRingBuffer() = default;

public:
  bool IsInitialized() const;
  void WaitInitialized() const;
  void SetInitialized();

public:
  std::size_t GetCapacity() const { return _capacity; }
  void Push(const char *msg, std::size_t msg_size);
  std::size_t Pop(char *msg_buf, std::size_t msg_buf_capacity);

private:
  const uint32_t _capacity;
  std::atomic_flag _is_initialized;

private:
  alignas(hardware_destructive_interference_size)
      std::atomic<uint64_t> _producer_index;
  alignas(hardware_destructive_interference_size)
      std::atomic<uint64_t> _consumer_index;

private:
  alignas(hardware_destructive_interference_size) char _ring_buffer[];
};
