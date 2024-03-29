#pragma once

#include <cmath>

namespace mrl {
struct angle_t {
  double radians;
};

constexpr angle_t operator-(angle_t angle) { return angle_t{-angle.radians}; }

constexpr auto radians(double rad) { return angle_t{rad}; }

constexpr auto degrees(double degree) { return angle_t{(degree * M_PI) / 180}; }

constexpr auto radians(angle_t angle) { return angle.radians; }

constexpr auto degrees(angle_t angle) { return angle.radians * (180.0 / M_PI); }
} // namespace mrl
