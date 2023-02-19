#pragma once

#include "interface/connection/ibuffer.hpp"
#include "interface/connection/iclient.hpp"
#include "interface/connection/ipacker.hpp"
#include "interface/generator/igenerator.hpp"
#include "interface/measurer/imeasurer.hpp"

class IProducer {
public:
  IProducer() = default;
  virtual ~IProducer() = default;

public:
  virtual void Run() = 0;
};

template <typename TBufferImpl, typename TGeneratorImpl, typename TMeasurerImpl,
          typename TMeasurerConnectionPackerImpl,
          typename TMeasurerConnectionClientImpl,
          typename TConsumerConnectionPackerImpl,
          typename TConsumerConnectionClientImpl>
class Producer final : public IProducer {
public:
  Producer(TGeneratorImpl generator, TMeasurerImpl measurer,
           TMeasurerConnectionClientImpl measurer_connection,
           TConsumerConnectionClientImpl consumer_connection)
      : generator(std::move(generator)), measurer(std::move(measurer)),
        measurer_connection(std::move(measurer_connection)),
        consumer_connection(std::move(consumer_connection)) {}

public:
  virtual void Run() override {
    generator.ToFirst();
    while (generator.HasNext()) {
      measurer.Tick();
      buffer.Reset();
      consumer_connection_packer.template Pack<TBufferImpl, std::string>(
          buffer, generator.Next());
      consumer_connection.template Send<TBufferImpl>(buffer);
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
  IGenerator<TGeneratorImpl> generator;
  IMeasurer<TMeasurerImpl> measurer;
  IPacker<TConsumerConnectionPackerImpl> consumer_connection_packer;
  IClient<TConsumerConnectionClientImpl> consumer_connection;
  IPacker<TMeasurerConnectionPackerImpl> measurer_connection_packer;
  IClient<TMeasurerConnectionClientImpl> measurer_connection;
};