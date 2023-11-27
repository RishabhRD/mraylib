#pragma once

#include "color.hpp"
#include "light.hpp"
#include "materials/scatter_record.hpp"
#include "ray.hpp"
#include <optional>
namespace mrl {
struct metal_t {
  color_t albedo;
};

// Precondition:
//   - normal is st dot(normal, ray.dir) <= 0
constexpr std::optional<scatter_record_t> scatter(metal_t const &material,
                                                  ray_t const &in_ray,
                                                  point3 const &hit_point,
                                                  direction_t normal) {
  auto ray_dir = reflection_dir(in_ray.direction, normal);
  ray_t scattered_ray{.origin = hit_point, .direction = ray_dir};
  return scatter_record_t{
      .scattered_ray = scattered_ray,
      .attenuated_color = material.albedo,
  };
}
} // namespace mrl
