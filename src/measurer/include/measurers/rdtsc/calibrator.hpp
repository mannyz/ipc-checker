#include <chrono>

class Calibrator {
public:
  Calibrator();

public:
  std::chrono::steady_clock::time_point ToTimePoint(uint64_t tsc) const;
  uint64_t FromTimePoint(std::chrono::steady_clock::time_point tp) const;

private:
  uint64_t _ref_tsc;
  std::chrono::steady_clock::time_point _ref_time;
  double _ns_per_tick;
};
