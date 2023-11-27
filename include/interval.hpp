#pragma once

#include <limits>
namespace mrl {
// represents [min, max]
struct interval_t {
  double min;
  double max;

  constexpr bool contains(double x) const { return min <= x && x <= max; }

  constexpr bool surrounds(double x) const { return min < x && x < max; }
};

constexpr static interval_t empty_interval{
    std::numeric_limits<double>::infinity(),
    -std::numeric_limits<double>::infinity()};

constexpr static interval_t universe{-std::numeric_limits<double>::infinity(),
                                     std::numeric_limits<double>::infinity()};

} // namespace mrl
