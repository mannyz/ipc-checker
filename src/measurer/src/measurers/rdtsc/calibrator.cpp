#include "measurers/rdtsc/calibrator.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <thread>

#include <cpuid.h>
#include <x86intrin.h>

namespace {
struct TscSample {
  double ns_per_tick;
  std::chrono::steady_clock::time_point ref_time;
  uint64_t ref_tsc;
};

inline void cpuid_barrier() {
  unsigned int eax, ebx, ecx, edx;
  __cpuid(0, eax, ebx, ecx, edx);
}
} // namespace

Calibrator::Calibrator() {
  constexpr int samples_count = 10;
  constexpr auto sleep_time = std::chrono::milliseconds(200);

  std::array<TscSample, samples_count> samples_data;

  for (int i = 0; i < samples_count; ++i) {
    auto time_before = std::chrono::steady_clock::now();
    unsigned int aux;
    uint64_t tsc_before = __rdtscp(&aux);
    cpuid_barrier();

    std::this_thread::sleep_for(sleep_time);

    uint64_t tsc_after = __rdtscp(&aux);
    cpuid_barrier();
    auto time_after = std::chrono::steady_clock::now();

    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(time_after -
                                                                   time_before)
                  .count();
    double ns_per_tick =
        static_cast<double>(ns) / static_cast<double>(tsc_after - tsc_before);

    auto ref_time = time_before + (time_after - time_before) / 2;
    uint64_t ref_tsc = (tsc_before + tsc_after) / 2;

    samples_data[i] = TscSample{ns_per_tick, ref_time, ref_tsc};
  }

  std::sort(samples_data.begin(), samples_data.end(),
            [](const TscSample &a, const TscSample &b) {
              return a.ns_per_tick < b.ns_per_tick;
            });

  auto &median_sample = samples_data[samples_count / 2];

  _ns_per_tick = median_sample.ns_per_tick;
  _ref_time = median_sample.ref_time;
  _ref_tsc = median_sample.ref_tsc;
}

std::chrono::steady_clock::time_point
Calibrator::ToTimePoint(uint64_t tsc) const {
  auto delta = tsc - _ref_tsc;
  auto duration =
      std::chrono::nanoseconds(static_cast<uint64_t>(delta * _ns_per_tick));
  return _ref_time + duration;
}

uint64_t
Calibrator::FromTimePoint(std::chrono::steady_clock::time_point tp) const {
  auto duration =
      std::chrono::time_point_cast<std::chrono::nanoseconds>(std::move(tp)) -
      _ref_time;
  auto ticks = duration.count() / _ns_per_tick;
  return _ref_tsc + ticks;
}
