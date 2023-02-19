#pragma once

#include "interface/connection/ipacker.hpp"
#include "interface/connection/iserver.hpp"
#include "interface/measurer/imeasurer.hpp"
#include "interface/statistics/istatistics.hpp"

class IAnalyzer {
public:
  IAnalyzer() = default;
  virtual ~IAnalyzer() = default;

public:
  virtual void Run() = 0;
};

template <typename TStatisticsImpl, typename TMeasurerConsumerImpl,
          typename TMeasurerProducerImpl,
          typename TProducerConnectionPackerImpl,
          typename TConsumerConnectionPackerImpl, typename TBufferImpl,
          typename TConsumerConnectionServerImpl,
          typename TProducerConnectionServerImpl>
class Analyzer final : public IAnalyzer {
public:
  Analyzer(TStatisticsImpl statistics, TMeasurerConsumerImpl consumer_measurer,
           TMeasurerProducerImpl producer_measurer,
           TConsumerConnectionServerImpl consumer_measurer_connection,
           TProducerConnectionServerImpl producer_measurer_connection)
      : statistics(std::move(statistics)),
        consumer_measurer(std::move(consumer_measurer)),
        producer_measurer(std::move(producer_measurer)),
        consumer_measurer_connection(std::move(consumer_measurer_connection)),
        producer_measurer_connection(std::move(producer_measurer_connection)) {}

public:
  virtual void Run() override {
    auto consumer_worker = std::thread([this]() {
      while (!consumer_measurer.IsFull()) {
        consumer_buffer.Reset();
        consumer_measurer_connection.template Receive<TBufferImpl>(
            consumer_buffer);
        TimeTick tp;
        consumer_measurer_connection_packer
            .template Unpack<TBufferImpl, TimeTick>(consumer_buffer, tp);
        consumer_measurer.PushBack(tp);
      }
    });

    auto producer_worker = std::thread([this]() {
      while (!producer_measurer.IsFull()) {
        producer_buffer.Reset();
        producer_measurer_connection.template Receive<TBufferImpl>(
            producer_buffer);
        TimeTick tp;
        producer_measurer_connection_packer
            .template Unpack<TBufferImpl, TimeTick>(producer_buffer, tp);
        producer_measurer.PushBack(tp);
      }
    });

    consumer_worker.join();
    producer_worker.join();

    statistics.template Process<TMeasurerProducerImpl, TMeasurerConsumerImpl>(
        producer_measurer, consumer_measurer);
  }

public:
  IStatistics<TStatisticsImpl> statistics;
  IMeasurer<TMeasurerConsumerImpl> consumer_measurer;
  IMeasurer<TMeasurerProducerImpl> producer_measurer;
  IPacker<TConsumerConnectionPackerImpl> consumer_measurer_connection_packer;
  IPacker<TProducerConnectionPackerImpl> producer_measurer_connection_packer;
  IBuffer<TBufferImpl> consumer_buffer;
  IBuffer<TBufferImpl> producer_buffer;
  IServer<TConsumerConnectionServerImpl> consumer_measurer_connection;
  IServer<TProducerConnectionServerImpl> producer_measurer_connection;
};
