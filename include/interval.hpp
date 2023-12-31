#pragma once

#include <limits>
#include <ostream>
namespace mrl {
// represents [min, max]
struct interval_t {
  double min;
  double max;

  constexpr interval_t(double min_, double max_) : min{min_}, max{max_} {}

  constexpr interval_t()
      : interval_t{std::numeric_limits<double>::infinity(),
                   -std::numeric_limits<double>::infinity()} {}

  constexpr bool contains(double x) const { return min <= x && x <= max; }

  constexpr bool surrounds(double x) const { return min < x && x < max; }
};

inline std::ostream &operator<<(std::ostream &os, interval_t const &rng) {
  os << "{ min: " << rng.min << " , max: " << rng.max << " }";
  return os;
}

constexpr static interval_t empty_interval{};

constexpr static interval_t universe{-std::numeric_limits<double>::infinity(),
                                     std::numeric_limits<double>::infinity()};

} // namespace mrl
