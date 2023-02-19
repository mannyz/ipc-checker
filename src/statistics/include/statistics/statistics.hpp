#pragma once

#include "interface/measurer/imeasurer.hpp"

#include <chrono>
#include <vector>

class Statistics {
public:
  template <typename TProducerMeasurerImpl, typename TConsumerMeasurerImpl>
  void Process(IMeasurer<TProducerMeasurerImpl> &producer,
               IMeasurer<TConsumerMeasurerImpl> &consumer);

private:
  void ProcessInternal(std::vector<std::chrono::nanoseconds> &&shippings,
                       std::chrono::nanoseconds total_time);
};
