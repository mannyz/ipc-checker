#include <thread>

#include <gtest/gtest.h>

#include "connection/buffer/buffer.hpp"
#include "connection/connectors/unix_socket/client.hpp"
#include "connection/connectors/unix_socket/client_impl.hpp"
#include "connection/connectors/unix_socket/server.hpp"
#include "connection/connectors/unix_socket/server_impl.hpp"
#include "connection/packers/local_packer/local_packer.hpp"
#include "connection/packers/objects/string.hpp"

namespace {

constexpr auto CONNECTION_ID = "ipc_checker_uxsock_connection_id";

} // namespace

TEST(UnixSocketTests, HelloTest) {
  std::thread client_thread{[]() {
    auto client = UnixSocketClient{CONNECTION_ID};
    while (!client.Connect())
      ;

    client.SetBufferSize(4096);

    auto client_buffer = IBuffer<Buffer>{};
    auto client_packer = LocalPacker{};
    client_packer.Pack(client_buffer, std::string("hello_msg"));
    client.Send(client_buffer);
  }};

  auto server = UnixSocketServer{CONNECTION_ID};
  server.Accept();

  std::string obj;
  auto server_buffer = IBuffer<Buffer>{};
  auto server_packer = LocalPacker{};
  server.Receive(server_buffer);
  server_packer.Unpack(server_buffer, obj);

  client_thread.join();

  EXPECT_EQ(obj, "hello_msg");
}
