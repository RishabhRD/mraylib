#pragma once

#include "angle.hpp"
#include "direction.hpp"
#include "point.hpp"
#include <concepts>
#include <ostream>

namespace mrl {

struct ray_t {
  point3 origin;
  direction_t direction;

  constexpr point3 at(double t) const { return origin + t * direction.val(); }

  friend std::ostream &operator<<(std::ostream &os, ray_t const &dir) {
    os << "{ origin : " << dir.origin << " , direction : " << dir.direction
       << " }";
    return os;
  }
};

constexpr ray_t rotate(ray_t const &r, ray_t const &axis,
                       angle_t const &angle) {
  vec3 translated_origin = r.origin - axis.origin;

  auto rdir = r.direction.val();
  auto adir = axis.direction.val();

  double angle_rad = radians(angle);

  double cos_theta = std::cos(angle_rad);
  double sin_theta = std::sin(angle_rad);
  vec3 rotated_direction = rdir * cos_theta + cross(adir, rdir) * sin_theta +
                           adir * dot(adir, rdir) * (1 - cos_theta);

  vec3 rotated_origin = axis.origin + (cos_theta * translated_origin) +
                        cross(adir, translated_origin) * sin_theta +
                        adir * dot(adir, translated_origin) * (1 - cos_theta);

  return {rotated_origin, rotated_direction};
}

} // namespace mrl
