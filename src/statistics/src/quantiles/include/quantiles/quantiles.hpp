#pragma once

#include <chrono>
#include <vector>

class Quantiles {
public:
  explicit Quantiles(std::vector<std::chrono::nanoseconds> &&shippings);
  std::chrono::nanoseconds Get(double alpha) const;

private:
  const std::size_t _N;
  std::vector<std::chrono::nanoseconds> _shippings;
};
