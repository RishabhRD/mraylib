#pragma once

#include "direction.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "scale_2d.hpp"
#include "scene_objects/shapes/quad.hpp"

namespace mrl {
struct scattering_context {
  ray_t ray;
  direction_t normal;
  point3 hit_point;
  scale_2d_t scaling_2d;
};

struct emission_context {
  direction_t normal;
  point3 hit_point;
  scale_2d_t scaling_2d;
};

template <typename HitObject>
constexpr auto make_scattering_context(HitObject const &o, ray_t const &r,
                                       double hit_distance) {
  auto const hit_point = r.at(hit_distance);
  return scattering_context{
      .ray = r,
      .normal = normal_at(o, hit_point),
      .hit_point = hit_point,
      .scaling_2d = scaling_2d_at(o, hit_point),
  };
}

template <typename HitObject>
constexpr auto make_emission_context(HitObject const &o,
                                     point3 const &hit_point) {
  return emission_context{
      .normal = normal_at(o, hit_point),
      .hit_point = hit_point,
      .scaling_2d = scaling_2d_at(o, hit_point),
  };
}
} // namespace mrl
