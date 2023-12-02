#pragma once

#include "direction.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "light.hpp"
#include "materials/scatter_record.hpp"
#include "normal.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <cmath>
#include <optional>

namespace mrl {
namespace __details {
constexpr auto should_tir(double sin_theta, double refractive_index_ratio) {
  return refractive_index_ratio * sin_theta > 1.0;
}

constexpr double reflectance(double cosine, double refractive_index_ratio) {
  // Use schlick's approximation
  auto r0 = (1 - refractive_index_ratio) / (1 + refractive_index_ratio);
  r0 = r0 * r0;
  return r0 + (1 - r0) * ((1 - cosine) * (1 - cosine) * (1 - cosine) *
                          (1 - cosine) * (1 - cosine));
}

constexpr bool choose_reflection(double cosine, double refractive_index_ratio,
                                 double reflection_threshold) {
  return reflectance(cosine, refractive_index_ratio) > reflection_threshold;
}

constexpr auto out_ray_dir(double sin_theta, double cos_theta,
                           double refractive_index_ratio,
                           double reflection_threshold,
                           direction_t const &in_ray_dir,
                           direction_t const &normal) {
  if (should_tir(sin_theta, refractive_index_ratio) ||
      choose_reflection(cos_theta, refractive_index_ratio,
                        reflection_threshold))
    return reflection_dir(in_ray_dir, normal);
  return refraction_dir(in_ray_dir, normal, refractive_index_ratio);
}

} // namespace __details
  //
struct dielectric {
  double refractive_index;
};

// Precondition:
//   - normal points to outside of object from hit_point
template <DoubleGenerator Generator>
constexpr auto scatter(dielectric const &material, ray_t const &in_ray,
                       point3 const &hit_point, direction_t normal,
                       generator_view<Generator> rand) {
  auto refractive_index = material.refractive_index;
  color_t attenuation{1.0, 1.0, 1.0};
  auto ray_from_outside = is_normal_away_from_ray(normal, in_ray.direction);
  double refraction_ratio =
      ray_from_outside ? (1.0 / refractive_index) : refractive_index;
  normal = ray_from_outside ? normal : opposite(normal);

  double cos_theta = std::fmin(dot(-in_ray.direction.val(), normal.val()), 1.0);
  double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
  auto out_ray_dir =
      __details::out_ray_dir(sin_theta, cos_theta, refraction_ratio,
                             rand(0.0, 1.0), in_ray.direction, normal);
  return std::optional{scatter_record_t{
      ray_t{hit_point, out_ray_dir},
      attenuation,
  }};
}
} // namespace mrl
