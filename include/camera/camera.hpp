#pragma once

#include "angle.hpp"

namespace mrl {
struct camera_t {
  double focal_length;
  angle_t vertical_fov;
};

constexpr auto focal_length(camera_t const &camera) {
  return camera.focal_length;
}
constexpr auto vertical_fov(camera_t const &camera) {
  return camera.vertical_fov;
}
} // namespace mrl
