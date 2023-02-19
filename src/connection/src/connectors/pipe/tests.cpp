#include <gtest/gtest.h>

#include "connection/buffer/buffer.hpp"
#include "connection/connectors/pipe/client.hpp"
#include "connection/connectors/pipe/client_impl.hpp"
#include "connection/connectors/pipe/server.hpp"
#include "connection/connectors/pipe/server_impl.hpp"
#include "connection/packers/local_packer/local_packer.hpp"
#include "connection/packers/objects/string.hpp"

namespace {

constexpr auto CONNECTION_ID = "ipc_checker_pipe_connection_id";

} // namespace

TEST(PipeTests, HelloTest) {
  auto server = PipeServer{CONNECTION_ID};

  auto client = PipeClient{CONNECTION_ID};
  while (!client.Connect())
    ;

  client.SetBufferSize(4096);

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