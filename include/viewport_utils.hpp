#pragma once

#include "camera_orientation.hpp"
#include <utility>

namespace mrl {

constexpr std::pair<vec3, vec3>
viewport_direction(camera_orientation_t const &o) {
  auto right =
      mrl::normalize(mrl::cross(o.direction.val(), mrl::vec3{0, 1, 0}));
  auto up = mrl::normalize(mrl::cross(right, o.direction.val()));
  return {right, -up};
}

constexpr vec3 viewport_topleft(vec3 right, vec3 down,
                                camera_orientation_t const &o, double f) {
  return o.position + o.direction.val() * f - right / 2 - down / 2;
}

} // namespace mrl
