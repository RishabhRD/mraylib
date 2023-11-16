#pragma once

#include "camera_orientation.hpp"
#include "vector.hpp"
#include <utility>

namespace mrl {

constexpr std::pair<vec3, vec3>
viewport_direction(camera_orientation_t const &o) {
  auto normal = o.direction.val();
  auto upward_dir = (normal == vec3{0, 1, 0} || normal == vec3{0, -1, 0})
                        ? vec3{0, 0, 1}
                        : vec3{0, 1, 0};
  auto right = normalize(cross(o.direction.val(), upward_dir));
  auto up = normalize(cross(right, o.direction.val()));
  return {right, -up};
}

constexpr vec3 viewport_topleft(vec3 right, vec3 down,
                                camera_orientation_t const &o, double f) {
  return o.position + o.direction.val() * f - right / 2 - down / 2;
}

} // namespace mrl
