#include "statistics/statistics.hpp"

#include <iomanip>
#include <iostream>
#include <locale>
#include <sys/types.h>

#include "quantiles/quantiles.hpp"

namespace {

struct PrettyNumpunct : std::numpunct<char> {
  std::string do_grouping() const override { return "\03"; }
  char do_thousands_sep() const override { return '\''; }
};

} // namespace

void Statistics::ProcessInternal(
    std::vector<std::chrono::nanoseconds> &&shippings,
    std::chrono::nanoseconds total_time) {
  std::cout << std::left << std::fixed << std::setprecision(9);
  std::cout.imbue(std::locale(std::cout.getloc(), new PrettyNumpunct));

  std::cout << "Total time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(total_time)
                   .count()
            << "ms" << std::endl
            << std::flush;

  Quantiles quantiles{std::move(shippings)};

  std::cout << "0.9999 = " << quantiles.Get(0.9999).count() << "ns" << std::endl
            << std::flush;
  std::cout << "0.999 = " << quantiles.Get(0.999).count() << "ns" << std::endl
            << std::flush;
  std::cout << "0.99 = " << quantiles.Get(0.99).count() << "ns" << std::endl
            << std::flush;
  std::cout << "0.95 = " << quantiles.Get(0.95).count() << "ns" << std::endl
            << std::flush;
  std::cout << "0.5 = " << quantiles.Get(0.5).count() << "ns" << std::endl
            << std::flush;
}
