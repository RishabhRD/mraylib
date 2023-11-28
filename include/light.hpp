#pragma once

#include "direction.hpp"
#include "vector.hpp"
#include <cmath>
namespace mrl {
// Precondition: normal is away from ray_dir
constexpr direction_t reflection_dir(direction_t const &ray_dir,
                                     direction_t const &normal) {
  return dir_from_unit(ray_dir.val() -
                       2 * dot(ray_dir.val(), normal.val()) * normal.val());
}

// Precondition:
//   - normal is away from ray_dir
constexpr direction_t refraction_dir(direction_t in_ray_dir, direction_t normal,
                                     double incident_to_refracted_index_ratio) {
  auto cos = std::fmin(dot(-in_ray_dir.val(), normal.val()), 1.0);
  auto out_ray_perp = incident_to_refracted_index_ratio *
                      (in_ray_dir.val() + cos * normal.val());
  auto out_ray_parallel =
      -std::sqrt(std::fabs(1.0 - out_ray_perp.length_square())) * normal.val();
  return out_ray_perp + out_ray_parallel;
}
} // namespace mrl
