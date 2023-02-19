#include "quantiles/quantiles.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

Quantiles::Quantiles(std::vector<std::chrono::nanoseconds> &&shippings)
    : _N(shippings.size()), _shippings(std::move(shippings)) {
  assert(_N > 0);
  std::sort(_shippings.begin(), _shippings.end());
}

std::chrono::nanoseconds Quantiles::Get(double alpha) const {
  assert(alpha >= 0.0);
  assert(alpha <= 1.0);

  const double pos = alpha * (_N - 1);
  const std::size_t idx = static_cast<std::size_t>(std::floor(pos));
  assert(idx <= pos);
  const double frac = pos - idx;

  const auto lo = _shippings[idx];
  const auto hi = _shippings[std::min(idx + 1, _N - 1)];

  const auto delta = hi - lo;
  const auto interpolated =
      lo + std::chrono::duration_cast<std::chrono::nanoseconds>(delta * frac);

  return interpolated;
}
