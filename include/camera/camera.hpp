#pragma once

#include "angle.hpp"

namespace mrl {
struct camera_t {
  double focus_distance;
  angle_t vertical_fov;
  angle_t defocus_angle;
};

constexpr auto focus_distance(camera_t const &camera) {
  return camera.focus_distance;
}
constexpr auto vertical_fov(camera_t const &camera) {
  return camera.vertical_fov;
}
constexpr auto defocus_angle(camera_t const &camera) {
  return camera.defocus_angle;
}
} // namespace mrl
