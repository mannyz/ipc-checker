#pragma once

#include "interface/connection/ibuffer.hpp"
#include "interface/connection/iclient.hpp"
#include "interface/connection/ipacker.hpp"
#include "interface/connection/iserver.hpp"
#include "interface/measurer/imeasurer.hpp"

class IConsumer {
public:
  IConsumer() = default;
  virtual ~IConsumer() = default;

public:
  virtual void Run() = 0;
};

template <typename TBufferImpl, typename TMeasurerImpl,
          typename TMeasurerConnectionPackerImpl,
          typename TMeasurerConnectionClientImpl,
          typename TConsumerConnectionPackerImpl,
          typename TConsumerConnectionServerImpl>
class Consumer final : public IConsumer {
public:
  Consumer(TMeasurerImpl measurer,
           TMeasurerConnectionClientImpl measurer_connection,
           TConsumerConnectionServerImpl producer_connection)
      : measurer(std::move(measurer)),
        measurer_connection(std::move(measurer_connection)),
        producer_connection(std::move(producer_connection)) {}

public:
  virtual void Run() override {
    while (!measurer.IsFull()) {
      buffer.Reset();
      producer_connection.template Receive<TBufferImpl>(buffer);
      std::string msg;
      producer_connection_packer.template Unpack<TBufferImpl, std::string>(
          buffer, msg);
      measurer.Tick();
    }

    measurer.ToFirst();
    while (measurer.HasNext()) {
      buffer.Reset();
      measurer_connection_packer.template Pack<TBufferImpl, TimeTick>(
          buffer, measurer.Next());
      measurer_connection.template Send<TBufferImpl>(buffer);
    }
  }

public:
  IBuffer<TBufferImpl> buffer;
  IMeasurer<TMeasurerImpl> measurer;
  IPacker<TMeasurerConnectionPackerImpl> measurer_connection_packer;
  IClient<TMeasurerConnectionClientImpl> measurer_connection;
  IPacker<TConsumerConnectionPackerImpl> producer_connection_packer;
  IServer<TConsumerConnectionServerImpl> producer_connection;
};