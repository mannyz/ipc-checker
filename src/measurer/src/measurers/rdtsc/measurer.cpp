#include "measurers/rdtsc/measurer.hpp"

#include <stdexcept>

#include <cpuid.h>
#include <x86intrin.h>

namespace {
inline void cpuid_barrier() {
  unsigned int eax, ebx, ecx, edx;
  __cpuid(0, eax, ebx, ecx, edx);
}

inline uint64_t ReadTSC() {
  unsigned int aux;
  auto tsc = __rdtscp(&aux);
  cpuid_barrier();
  return tsc;
}
} // namespace

RdtscMeasurer::RdtscMeasurer(std::size_t chunks_total_count) {
  _measurements.reserve(chunks_total_count);
}

void RdtscMeasurer::Tick() { _measurements.emplace_back(ReadTSC()); }

TimeTick RdtscMeasurer::FirstTick() const {
  return _calibrator.ToTimePoint(_measurements.front());
}

TimeTick RdtscMeasurer::LastTick() const {
  return _calibrator.ToTimePoint(_measurements.back());
}

void RdtscMeasurer::ToFirst() { _it_measurements = std::begin(_measurements); }

bool RdtscMeasurer::HasNext() const {
  return _it_measurements != std::end(_measurements);
}

TimeTick RdtscMeasurer::Next() {
  return _calibrator.ToTimePoint(*_it_measurements++);
}

std::size_t RdtscMeasurer::Count() const { return _measurements.size(); }

bool RdtscMeasurer::IsFull() const {
  return _measurements.size() == _measurements.capacity();
}

void RdtscMeasurer::PushBack(const TimeTick &) {
  throw std::runtime_error("Not implemented!");
}
