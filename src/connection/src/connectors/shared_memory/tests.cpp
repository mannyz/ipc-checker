#include <array>
#include <string_view>

#include <gtest/gtest.h>

#include "connection/buffer/buffer.hpp"
#include "connection/connectors/shared_memory/client.hpp"
#include "connection/connectors/shared_memory/client_impl.hpp"
#include "connection/connectors/shared_memory/server.hpp"
#include "connection/connectors/shared_memory/server_impl.hpp"
#include "connection/packers/local_packer/local_packer.hpp"
#include "connection/packers/objects/string.hpp"

namespace {

constexpr auto CONNECTION_ID = "ipc_checker_shmem_connection_id";

} // namespace

TEST(SharedMemoryTests, HelloTest) {
  auto server = SharedMemoryServer{CONNECTION_ID, 4096};

  auto client = SharedMemoryClient{CONNECTION_ID};
  client.Connect();

  auto client_buffer = IBuffer<Buffer>{};
  auto client_packer = LocalPacker{};
  client_packer.Pack(client_buffer, std::string("hello_msg"));
  client.Send(client_buffer);

  std::string obj;
  auto server_buffer = IBuffer<Buffer>{};
  auto server_packer = LocalPacker{};
  server.Receive(server_buffer);
  server_packer.Unpack(server_buffer, obj);

  EXPECT_EQ(obj, "hello_msg");
}

TEST(RingBufferTests, CorrectnessTest_EmptyMessage) {
  using namespace std::literals;

  constexpr auto MSG = ""sv;
  constexpr auto BUFFER_SIZE = 128;
  constexpr auto ITERATIONS_COUNT = 1000000;

  char ring_buffer_memory[sizeof(SPSCRingBuffer) + BUFFER_SIZE];
  auto rbh = new (ring_buffer_memory) SPSCRingBuffer(BUFFER_SIZE);

  std::thread client_thread{[rbh, MSG]() {
    for (int i = 0; i != ITERATIONS_COUNT; ++i) {
      rbh->Push(MSG.data(), MSG.size());
    }
  }};

  for (int i = 0; i != ITERATIONS_COUNT; ++i) {
    char msg_buffer[100];
    const auto msg_size = rbh->Pop(msg_buffer, 1000);
    auto msg = std::string_view{msg_buffer, msg_size};
    EXPECT_EQ(msg, MSG);
  }

  client_thread.join();

  rbh->~SPSCRingBuffer();
}

TEST(RingBufferTests, CorrectnessTest_SomeMessage) {
  using namespace std::literals;

  constexpr auto MSG = "test_msg_12345"sv;
  constexpr auto BUFFER_SIZE = 128;
  constexpr auto ITERATIONS_COUNT = 1000000;

  char ring_buffer_memory[sizeof(SPSCRingBuffer) + BUFFER_SIZE];
  auto rbh = new (ring_buffer_memory) SPSCRingBuffer(BUFFER_SIZE);

  std::thread client_thread{[rbh, MSG]() {
    for (int i = 0; i != ITERATIONS_COUNT; ++i) {
      rbh->Push(MSG.data(), MSG.size());
    }
  }};

  for (int i = 0; i != ITERATIONS_COUNT; ++i) {
    char msg_buffer[100];
    const auto msg_size = rbh->Pop(msg_buffer, 1000);
    auto msg = std::string_view{msg_buffer, msg_size};
    EXPECT_EQ(msg, MSG);
  }

  client_thread.join();

  rbh->~SPSCRingBuffer();
}

TEST(RingBufferTests, CorrectnessTest_DifferentMessages) {
  using namespace std::literals;

  constexpr std::array MSGS = {"hello_msg"sv,       "test_msg_12"sv,
                               "test_msg_123"sv,    "test_msg_12345"sv,
                               "test_msg_123456"sv, "test_msg_23456"sv,
                               "test_msg_3456"sv,   "test_msg_456"sv,
                               "test_msg_56"sv,     "test_msg_6"sv};
  constexpr auto BUFFER_SIZE = 128;
  constexpr auto ITERATIONS_COUNT = 1000000;

  char ring_buffer_memory[sizeof(SPSCRingBuffer) + BUFFER_SIZE];
  auto rbh = new (ring_buffer_memory) SPSCRingBuffer(BUFFER_SIZE);

  std::thread client_thread{[rbh, MSGS]() {
    for (int i = 0; i != ITERATIONS_COUNT; ++i) {
      auto MSG = MSGS[i % MSGS.max_size()];
      rbh->Push(MSG.data(), MSG.size());
    }
  }};

  for (int i = 0; i != ITERATIONS_COUNT; ++i) {
    char msg_buffer[100];
    const auto msg_size = rbh->Pop(msg_buffer, 1000);
    auto msg = std::string_view{msg_buffer, msg_size};
    auto MSG = MSGS[i % MSGS.max_size()];
    EXPECT_EQ(msg, MSG);
  }

  client_thread.join();

  rbh->~SPSCRingBuffer();
}

TEST(RingBufferTests, CorrectnessTest_BufferAlignedByMessage) {
  using namespace std::literals;

  constexpr auto MSG = "test_msg_123"sv;
  constexpr auto RING_BUFFER_HEADER_SIZE = 4;
  constexpr auto MSG_IN_RING_BUFFER_SIZE = RING_BUFFER_HEADER_SIZE + MSG.size();
  constexpr auto BUFFER_SIZE = 8 * MSG_IN_RING_BUFFER_SIZE;
  ASSERT_EQ(BUFFER_SIZE, 128);
  constexpr auto ITERATIONS_COUNT = 1000000;

  char ring_buffer_memory[sizeof(SPSCRingBuffer) + BUFFER_SIZE];
  auto rbh = new (ring_buffer_memory) SPSCRingBuffer(BUFFER_SIZE);
  ASSERT_EQ(rbh->GetCapacity(), BUFFER_SIZE);

  std::thread client_thread{[rbh, MSG]() {
    for (int i = 0; i != ITERATIONS_COUNT; ++i) {
      rbh->Push(MSG.data(), MSG.size());
    }
  }};

  for (int i = 0; i != ITERATIONS_COUNT; ++i) {
    char msg_buffer[100];
    const auto msg_size = rbh->Pop(msg_buffer, 1000);
    auto msg = std::string_view{msg_buffer, msg_size};
    EXPECT_EQ(msg, MSG);
  }

  client_thread.join();

  rbh->~SPSCRingBuffer();
}

TEST(FdHandleTests, CorrectnessTest) { FileDescriptor fd1{1}; }