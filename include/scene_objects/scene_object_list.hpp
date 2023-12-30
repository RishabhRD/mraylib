#pragma once

#include "interval.hpp"
#include "ray.hpp"
#include <algorithm>
#include <limits>
#include <optional>
namespace mrl {
struct bound_t {
  interval_t x_range;
  interval_t y_range;
  interval_t z_range;
};

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
  if (t2 < 0)
    return std::nullopt;
  return interval_t{std::max(t1, 0.0), t2};
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
} // namespace mrl
