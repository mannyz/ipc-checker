#include "worker.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "interface/declare_mock.hpp"
#include "interface/return_by_move_from_ref.hpp"

#include "tags.hpp"

DECLARE_MOCK(ConnectionImpl, ::testing::StrictMock<Mock>);
DECLARE_MOCK(ConnectionChannelImpl, ::testing::StrictMock<Mock>);

IMPL_TRAITS_DEFINITION(ConnectionImpl)
USE_IMPL_FOR_TAG(tags::Connection::Channel, ConnectionChannelImpl);
END_IMPL_TRAITS_DEFINITION();

TEST(WorkerTests, HelloTest) {
  ConnectionImpl connection_impl;
  Worker<ConnectionImpl> worker(std::move(connection_impl));

  ::testing::InSequence s;

  IChannel<ConnectionChannelImpl> connection_channel;

  EXPECT_CALL(worker.connection, GetChannel())
      .WillOnce(::testing::ReturnByMoveFromRef(connection_channel));

  EXPECT_CALL(connection_channel, Read(::testing::An<std::string &>()));
  EXPECT_CALL(connection_channel, Write(::testing::An<const TimeTick &>()));

  worker.Run();
}