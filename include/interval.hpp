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

constexpr double clamp(interval_t const &interval, double x) {
  if (x < interval.min)
    return interval.min;
  if (x > interval.max)
    return interval.max;
  return x;
}

constexpr interval_t expand(interval_t i, double delta) {
  auto padding = delta / 2;
  i.min -= padding;
  i.max += padding;
  return i;
}

constexpr double size(interval_t const &i) { return i.max - i.min; }
} // namespace mrl
