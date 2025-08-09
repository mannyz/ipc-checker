#include "measurers/chrono/measurer.hpp"

Measurer::Measurer(std::size_t chunks_total_count) {
  _measurements.reserve(chunks_total_count);
}

void Measurer::Tick() {
  _measurements.emplace_back(std::chrono::steady_clock::now());
}

TimeTick Measurer::FirstTick() const { return _measurements.front(); }

TimeTick Measurer::LastTick() const { return _measurements.back(); }

void Measurer::ToFirst() { _it_measurements = std::begin(_measurements); }

bool Measurer::HasNext() const {
  return _it_measurements != std::end(_measurements);
}

TimeTick Measurer::Next() { return *_it_measurements++; }

std::size_t Measurer::Count() const { return _measurements.size(); }

bool Measurer::IsFull() const {
  return _measurements.size() == _measurements.capacity();
}

void Measurer::PushBack(const TimeTick &tick) {
  _measurements.emplace_back(tick);
}
