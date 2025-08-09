#pragma once

#include <vector>

#include "interface/measurer/time_tick.hpp"

#include "calibrator.hpp"

class RdtscMeasurer {
public:
  RdtscMeasurer(std::size_t chunks_total_count);

public:
  void Tick();

public:
  TimeTick FirstTick() const;
  TimeTick LastTick() const;

public:
  void ToFirst();
  bool HasNext() const;
  TimeTick Next();
  std::size_t Count() const;

public:
  bool IsFull() const;
  void PushBack(const TimeTick &tick);

private:
  Calibrator _calibrator;
  std::vector<uint64_t> _measurements;
  std::vector<uint64_t>::const_iterator _it_measurements;
};
