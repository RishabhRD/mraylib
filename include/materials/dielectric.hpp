#pragma once

#include "direction.hpp"
#include "light.hpp"
#include "materials/scatter_record.hpp"
#include "normal.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "std/random_double_generator.hpp"
#include "vector.hpp"
#include <cmath>
#include <functional>
#include <optional>
#include <type_traits>

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
template <std::invocable DoubleGenerator = random_double_generator>
  requires std::same_as<std::invoke_result_t<DoubleGenerator>, double>
class basic_dielectric {
public:
  double refractive_index;

private:
  mutable DoubleGenerator gen_reflection_threshold;

public:
  // Precondition:
  //   - gen_reflection_threshold_ always generates value between [0.0, 1.0).
  constexpr basic_dielectric(double refractive_index_,
                             DoubleGenerator gen_reflection_threshold_ =
                                 random_double_generator(0.0, 1.0))
      : refractive_index(refractive_index_),
        gen_reflection_threshold(std::move(gen_reflection_threshold_)) {}

  // Precondition:
  //   - normal points to outside of object from hit_point
  constexpr std::optional<scatter_record_t> scatter(ray_t const &in_ray,
                                                    point3 const &hit_point,
                                                    direction_t normal) const {
    color_t attenuation{1.0, 1.0, 1.0};
    auto ray_from_outside = is_normal_away_from_ray(normal, in_ray.direction);
    double refraction_ratio =
        ray_from_outside ? (1.0 / refractive_index) : refractive_index;
    normal = ray_from_outside ? normal : opposite(normal);

    double cos_theta =
        std::fmin(dot(-in_ray.direction.val(), normal.val()), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
    auto out_ray_dir = __details::out_ray_dir(
        sin_theta, cos_theta, refraction_ratio,
        std::invoke(gen_reflection_threshold), in_ray.direction, normal);
    return scatter_record_t{
        ray_t{hit_point, out_ray_dir},
        attenuation,
    };
  }
};

template <std::invocable DoubleGenerator = random_double_generator>
  requires std::same_as<std::invoke_result_t<DoubleGenerator>, double>
basic_dielectric(double, DoubleGenerator) -> basic_dielectric<DoubleGenerator>;

using dielectric = basic_dielectric<>;

// Precondition:
//   - normal points to outside of object from hit_point
template <std::invocable DoubleGenerator = random_double_generator>
  requires std::same_as<std::invoke_result_t<DoubleGenerator>, double>
constexpr auto scatter(basic_dielectric<DoubleGenerator> const &material,
                       ray_t const &in_ray, point3 const &hit_point,
                       direction_t normal) {
  return material.scatter(in_ray, hit_point, normal);
}
} // namespace mrl
