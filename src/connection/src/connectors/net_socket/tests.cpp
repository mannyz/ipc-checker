#include <thread>

#include <gtest/gtest.h>

#include "connection/buffer/buffer.hpp"
#include "connection/connectors/net_socket/client.hpp"
#include "connection/connectors/net_socket/client_impl.hpp"
#include "connection/connectors/net_socket/server.hpp"
#include "connection/connectors/net_socket/server_impl.hpp"
#include "connection/packers/network_packer/network_packer.hpp"
#include "connection/packers/objects/string.hpp"

namespace {

constexpr auto CONNECTION_ID_HOST = "localhost";
constexpr auto CONNECTION_ID_PORT = 2000;

} // namespace

TEST(NetSocketTests, HelloTest) {
  std::thread client_thread{[]() {
    auto client = NetSocketClient{CONNECTION_ID_HOST, CONNECTION_ID_PORT};
    while (!client.Connect())
      ;

    client.SetBufferSize(4096);
    client.SetNoDelay();

    auto client_buffer = IBuffer<Buffer>{};
    auto client_packer = NetworkPacker<Endianness::kBig>{};
    client_packer.Pack(client_buffer, std::string("hello_msg"));
    client.Send(client_buffer);
  }};

  auto server = NetSocketServer{CONNECTION_ID_PORT};
  server.Accept();
  server.SetBufferSize(4096);

  std::string obj;
  auto server_buffer = IBuffer<Buffer>{};
  auto server_packer = NetworkPacker<Endianness::kBig>{};
  server.Receive(server_buffer);
  server_packer.Unpack(server_buffer, obj);

  client_thread.join();

  EXPECT_EQ(obj, "hello_msg");
}
