#include "connection/connectors/shared_memory/ring_buffer.hpp"

#include <atomic>
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace {

struct RecordHeader {
  uint32_t payload_size;
  char payload[];
};

uint32_t AlignUpByRecordHeader(uint32_t value) {
  constexpr uint32_t kStep = sizeof(RecordHeader);
  const auto aligned_value = (value + kStep - 1) & ~(kStep - 1);
  assert(value <= aligned_value);
  return aligned_value;
}

struct alignas(uint64_t) IndexAsUInt64 {
  uint32_t value;
  uint32_t version;

  friend bool operator==(const IndexAsUInt64 &lhs,
                         const IndexAsUInt64 &rhs) = default;
};

static_assert(sizeof(IndexAsUInt64) == sizeof(uint64_t),
              "Size for IndexAsUInt64 must be 8 bytes");

static_assert(alignof(IndexAsUInt64) == alignof(uint64_t),
              "Alignment for IndexAsUInt64 must be 8 bytes");

template <typename BytesCopier>
requires requires(BytesCopier bytes_copier, uint32_t fit_part,
                  uint32_t overflowed_part) {
  bytes_copier(fit_part, overflowed_part);
}
IndexAsUInt64 ProcessMessage(BytesCopier bytes_copier, IndexAsUInt64 index,
                             uint32_t payload_size, uint32_t capacity) {
  index.value += sizeof(RecordHeader);
  assert(index.value <= capacity);

  const auto buffer_free_size_fit_part = capacity - index.value;
  assert(buffer_free_size_fit_part <= capacity);

  const auto payload_size_overflowed_part =
      -(buffer_free_size_fit_part < payload_size) &
      (payload_size - buffer_free_size_fit_part);
  assert(payload_size_overflowed_part <= payload_size);
  const auto payload_size_fit_part =
      payload_size - payload_size_overflowed_part;

  bytes_copier(payload_size_fit_part, payload_size_overflowed_part);

  const auto payload_size_fit_part_aligned =
      AlignUpByRecordHeader(payload_size_fit_part);
  assert(payload_size_fit_part_aligned <= buffer_free_size_fit_part);

  index.version += (payload_size_fit_part_aligned == buffer_free_size_fit_part);
  index.value = (-(payload_size_fit_part_aligned != buffer_free_size_fit_part) &
                 (index.value + payload_size_fit_part_aligned)) +
                AlignUpByRecordHeader(payload_size_overflowed_part);
  assert(index.value < capacity);
  return index;
}

template <typename T> constexpr void *__restrict RestrictPointer(T *t) {
  return static_cast<void *__restrict>(t);
};

template <typename T>
constexpr const void *__restrict RestrictConstPointer(const T *t) {
  return static_cast<const void *__restrict>(t);
};

} // namespace

SPSCRingBuffer::SPSCRingBuffer(uint32_t capacity)
    : _capacity(capacity), _producer_index(0), _consumer_index(0) {
  if (capacity != AlignCapacity(capacity)) {
    throw std::runtime_error("wrong capacity");
  }
}

bool SPSCRingBuffer::IsInitialized() const { return _is_initialized.test(); }

void SPSCRingBuffer::WaitInitialized() const { _is_initialized.wait(false); }

void SPSCRingBuffer::SetInitialized() {
  _is_initialized.test_and_set();
  _is_initialized.notify_one();
}

void SPSCRingBuffer::Push(const char *payload, std::size_t payload_size) {
  const auto record_size = sizeof(RecordHeader) + payload_size;
  if (record_size < payload_size || _capacity < record_size) {
    throw std::runtime_error("message is not fit in ring memory buffer");
  }

  const auto producer_index = std::bit_cast<IndexAsUInt64>(
      _producer_index.load(std::memory_order_relaxed));

  for (;;) {
    const auto consumer_index = std::bit_cast<IndexAsUInt64>(
        _consumer_index.load(std::memory_order_acquire));
    assert(consumer_index.value < _capacity);
    assert((consumer_index.version == producer_index.version &&
            consumer_index.value <= producer_index.value) ||
           (consumer_index.version != producer_index.version &&
            producer_index.value <= consumer_index.value));
    const auto buffer_free_size =
        ((-(consumer_index.version == producer_index.version) &
          (_capacity - (producer_index.value - consumer_index.value))) +
         (-(producer_index.version != consumer_index.version) &
          (consumer_index.value - producer_index.value)));
    assert(buffer_free_size <= _capacity);
    if (record_size <= buffer_free_size) {
      break;
    }
  }

  const auto new_record =
      reinterpret_cast<RecordHeader *>(&_ring_buffer[producer_index.value]);
  new_record->payload_size = payload_size;

  const auto new_producer_index = ProcessMessage(
      [new_record_payload_fit_part = new_record->payload,
       new_record_payload_overflowed_part = _ring_buffer,
       payload](uint32_t payload_size_fit_part,
                uint32_t payload_size_overflowed_part) {
        memcpy(RestrictPointer(new_record_payload_fit_part),
               RestrictConstPointer(payload), payload_size_fit_part);
        memcpy(RestrictPointer(new_record_payload_overflowed_part),
               RestrictConstPointer(payload + payload_size_fit_part),
               payload_size_overflowed_part);
      },
      producer_index, payload_size, _capacity);

  _producer_index.store(std::bit_cast<uint64_t>(new_producer_index),
                        std::memory_order_release);
}

std::size_t SPSCRingBuffer::Pop(char *msg_buf, std::size_t msg_buf_capacity) {
  const auto consumer_index = std::bit_cast<IndexAsUInt64>(
      _consumer_index.load(std::memory_order_relaxed));
  while (consumer_index == std::bit_cast<IndexAsUInt64>(
                               _producer_index.load(std::memory_order_acquire)))
    ;

  const auto record = reinterpret_cast<const RecordHeader *>(
      &_ring_buffer[consumer_index.value]);
  if (record->payload_size > msg_buf_capacity) {
    throw std::runtime_error("message is not fit in outer memory buffer");
  }

  const auto payload_size = record->payload_size;
  const auto new_consumer_index = ProcessMessage(
      [msg_buf, record_payload_fit_part = record->payload,
       record_payload_overflowed_part =
           _ring_buffer](uint32_t payload_size_fit_part,
                         uint32_t payload_size_overflowed_part) {
        memcpy(RestrictPointer(msg_buf),
               RestrictConstPointer(record_payload_fit_part),
               payload_size_fit_part);
        memcpy(RestrictPointer(msg_buf + payload_size_fit_part),
               RestrictConstPointer(record_payload_overflowed_part),
               payload_size_overflowed_part);
      },
      consumer_index, payload_size, _capacity);

  _consumer_index.store(std::bit_cast<uint64_t>(new_consumer_index),
                        std::memory_order_release);

  return payload_size;
}

uint32_t SPSCRingBuffer::AlignCapacity(uint32_t capacity) {
  const auto aligned_capacity = AlignUpByRecordHeader(capacity);

  if (aligned_capacity < capacity) {
    throw std::runtime_error("capacity is overflowed after alignment");
  }

  return aligned_capacity;
}
