#include <gtest/gtest.h>

#include "quantiles/quantiles.hpp"

using namespace std::chrono_literals;

TEST(QuantilesTests, BasicTest) {
  std::vector<std::chrono::nanoseconds> data = {100ns, 200ns, 300ns, 400ns,
                                                500ns};

  Quantiles p(std::move(data));

  EXPECT_EQ(p.Get(0.0), 100ns);  // min
  EXPECT_EQ(p.Get(1.0), 500ns);  // max
  EXPECT_EQ(p.Get(0.5), 300ns);  // median
  EXPECT_EQ(p.Get(0.25), 200ns); // exact index
  EXPECT_EQ(p.Get(0.75), 400ns); // exact index
}

TEST(QuantilesTests, InterpolatedValuesTest) {
  std::vector<std::chrono::nanoseconds> data = {100ns, 200ns, 300ns, 400ns,
                                                500ns};

  Quantiles p(std::move(data));

  EXPECT_EQ(
      p.Get(0.1),
      140ns); // 140ns = 100ns + (200ns - 100ns) x (0.1 x 4 - 0) = 100ns + 40ns
  EXPECT_EQ(
      p.Get(0.9),
      460ns); // 460ns = 400ns + (500ns - 400ns) x (0.9 x 4 - 3) = 400ns + 60ns
}

TEST(QuantilesTests, DuplicateValuesTest) {
  std::vector<std::chrono::nanoseconds> data = {100ns, 100ns, 100ns, 100ns,
                                                200ns};

  Quantiles p(std::move(data));

  EXPECT_EQ(p.Get(0.0), 100ns);
  EXPECT_EQ(p.Get(0.1), 100ns);
  EXPECT_EQ(p.Get(0.5), 100ns);
  EXPECT_EQ(p.Get(1.0), 200ns);
}
