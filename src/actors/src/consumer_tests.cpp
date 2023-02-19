#include "actors/consumer.hpp"

#include <chrono>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "interface/declare_mock.hpp"

DECLARE_MOCK(BufferImpl, ::testing::StrictMock<Mock>);
DECLARE_MOCK(MeasurerImpl, ::testing::StrictMock<Mock>);
DECLARE_MOCK(MeasurerConnectionPackerImpl, ::testing::StrictMock<Mock>);
DECLARE_MOCK(MeasurerConnectionClientImpl, ::testing::StrictMock<Mock>);
DECLARE_MOCK(ConsumerConnectionPackerImpl, ::testing::StrictMock<Mock>);
DECLARE_MOCK(ConsumerConnectionServerImpl, ::testing::StrictMock<Mock>);

TEST(ConsumerTests, HelloTest) {
  MeasurerImpl measurer_impl;
  MeasurerConnectionClientImpl measurer_client_impl;
  ConsumerConnectionServerImpl connection_server_impl;

  Consumer<BufferImpl, MeasurerImpl, MeasurerConnectionPackerImpl,
           MeasurerConnectionClientImpl, ConsumerConnectionPackerImpl,
           ConsumerConnectionServerImpl>
      consumer(std::move(measurer_impl), std::move(measurer_client_impl),
               std::move(connection_server_impl));

  testing::InSequence s;
  //
  EXPECT_CALL(consumer.measurer, IsFull()).WillOnce(::testing::Return(false));
  EXPECT_CALL(consumer.buffer, Reset());
  EXPECT_CALL(consumer.producer_connection,
              Receive<BufferImpl>(::testing::Ref(consumer.buffer)));
  EXPECT_CALL_TEMPLATE(
      consumer.producer_connection_packer, (BufferImpl, std::string), Unpack,
      (::testing::Ref(consumer.buffer), ::testing::An<std::string &>()));
  EXPECT_CALL(consumer.measurer, Tick());
  //
  EXPECT_CALL(consumer.measurer, IsFull()).WillOnce(::testing::Return(true));
  //
  EXPECT_CALL(consumer.measurer, ToFirst());
  EXPECT_CALL(consumer.measurer, HasNext()).WillOnce(::testing::Return(true));
  EXPECT_CALL(consumer.buffer, Reset());
  TimeTick tp1;
  EXPECT_CALL(consumer.measurer, Next()).WillOnce(::testing::Return(tp1));
  EXPECT_CALL_TEMPLATE(
      consumer.measurer_connection_packer, (BufferImpl, TimeTick), Pack,
      (::testing::Ref(consumer.buffer), ::testing::Eq<const TimeTick &>(tp1)));
  EXPECT_CALL(consumer.measurer_connection,
              Send<BufferImpl>(::testing::Ref(consumer.buffer)));
  //
  EXPECT_CALL(consumer.measurer, HasNext()).WillOnce(::testing::Return(false));

  consumer.Run();
}