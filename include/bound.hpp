#pragma once

#include "interval.hpp"
#include "ray.hpp"
#include <algorithm>
#include <limits>
#include <optional>
#include <ostream>

namespace mrl {
struct bound_t {
  interval_t x_range;
  interval_t y_range;
  interval_t z_range;
};

inline std::ostream &operator<<(std::ostream &os, bound_t const &bound) {
  os << "{ x_range: " << bound.x_range << " , y_range: " << bound.y_range
     << " , z_range: " << bound.z_range << " }";
  return os;
}

namespace __details {
constexpr auto inf = std::numeric_limits<double>::infinity();
constexpr std::optional<interval_t> time_range(double a, double b,
                                               interval_t range) {
  if (b == 0) {
    if (a >= range.min && a <= range.max) {
      return interval_t{0, inf};
    }
    return std::nullopt;
  }
  auto t1 = (range.min - a) / b;
  auto t2 = (range.max - a) / b;
  auto min = std::min(t1, t2);
  auto max = std::max(t1, t2);
  if (max < 0)
    return std::nullopt;
  return interval_t{std::max(min, 0.0), std::max(max, 0.0)};
}
}; // namespace __details

constexpr bool hit_bounds(ray_t const &ray, bound_t const &bounds) {
  auto x_int = __details::time_range(ray.origin.x, ray.direction.val().x,
                                     bounds.x_range);
  auto y_int = __details::time_range(ray.origin.y, ray.direction.val().y,
                                     bounds.y_range);
  auto z_int = __details::time_range(ray.origin.z, ray.direction.val().z,
                                     bounds.z_range);
  if (x_int && y_int && z_int) {
    auto t1 = std::max({x_int->min, y_int->min, z_int->min});
    auto t2 = std::min({x_int->max, y_int->max, z_int->max});
    return t1 <= t2;
  }
  return false;
}

constexpr bound_t union_bounds(bound_t const &a, bound_t const &b) {
  interval_t x_range{
      std::min(a.x_range.min, b.x_range.min),
      std::max(a.x_range.max, b.x_range.max),
  };
  interval_t y_range{
      std::min(a.y_range.min, b.y_range.min),
      std::max(a.y_range.max, b.y_range.max),
  };
  interval_t z_range{
      std::min(a.z_range.min, b.z_range.min),
      std::max(a.z_range.max, b.z_range.max),
  };
  return {x_range, y_range, z_range};
}
} // namespace mrl
