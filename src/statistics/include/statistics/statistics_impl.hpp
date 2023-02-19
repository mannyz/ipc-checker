#include "statistics.hpp"

template <typename TProducerMeasurerImpl, typename TConsumerMeasurerImpl>
void Statistics::Process(IMeasurer<TProducerMeasurerImpl> &producer,
                         IMeasurer<TConsumerMeasurerImpl> &consumer) {
  assert(producer.Count() == consumer.Count());

  std::vector<std::chrono::nanoseconds> shippings;
  shippings.reserve(std::min(producer.Count(), consumer.Count()));

  producer.ToFirst();
  consumer.ToFirst();
  while (producer.HasNext() && consumer.HasNext()) {
    shippings.emplace_back(std::chrono::duration_cast<std::chrono::nanoseconds>(
        consumer.Next() - producer.Next()));
  }

  ProcessInternal(std::move(shippings),
                  consumer.LastTick() - producer.FirstTick());
}
