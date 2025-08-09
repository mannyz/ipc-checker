#pragma once

#include <vector>

#include "interface/measurer/time_tick.hpp"

class Measurer {
public:
  Measurer(std::size_t chunks_total_count);

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
  std::vector<TimeTick> _measurements;
  std::vector<TimeTick>::const_iterator _it_measurements;
};
