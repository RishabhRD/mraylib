#pragma once

#include <cmath>

namespace mrl {
constexpr static double pi = M_PI;
struct angle_t {
  double radians;
};

constexpr auto radians(double rad) { return angle_t{rad}; }

constexpr auto degrees(double degree) { return angle_t{(degree * pi) / 180}; }

constexpr auto radians(angle_t angle) { return angle.radians; }

constexpr auto degrees(angle_t angle) { return angle.radians * (180.0 / pi); }
} // namespace mrl
