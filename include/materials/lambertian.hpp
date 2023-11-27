#pragma once

#include "color.hpp"
#include "direction.hpp"
#include "materials/scatter_record.hpp"
#include "point.hpp"
#include "random/random_vector.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <functional>
#include <optional>

namespace mrl {
template <std::invocable DirectionGenerator = random_direction_generator>
  requires std::same_as<std::invoke_result_t<DirectionGenerator>, direction_t>
struct basic_lambertian_t {
  color_t albedo;
  mutable DirectionGenerator gen_dir;

  constexpr basic_lambertian_t(
      color_t albedo_,
      DirectionGenerator gen_dir_ = random_direction_generator())
      : albedo(std::move(albedo_)), gen_dir(std::move(gen_dir_)) {}
};

using lambertian_t = basic_lambertian_t<>;

// Precondition:
//   - normal is st dot(normal, ray.dir) <= 0
constexpr std::optional<scatter_record_t>
lambertian_scatter(color_t const &material_color, point3 hit_point,
                   direction_t const &normal, direction_t const &random_dir) {
  auto scatter_dir = normal.val() + random_dir.val();
  if (near_zero(scatter_dir))
    scatter_dir = normal.val();
  auto scattered_ray = ray_t{
      .origin = hit_point,
      .direction = scatter_dir,
  };
  auto attenuation = material_color;
  return scatter_record_t{
      .scattered_ray = scattered_ray,
      .attenuated_color = attenuation,
  };
}

// Precondition:
//   - normal is st dot(normal, ray.dir) <= 0
template <std::invocable DirectionGenerator>
  requires std::same_as<std::invoke_result_t<DirectionGenerator>, direction_t>
constexpr auto scatter(basic_lambertian_t<DirectionGenerator> const &material,
                       ray_t const &, point3 const &hit_point,
                       direction_t const &normal) {
  return lambertian_scatter(material.albedo, hit_point, normal,
                            std::invoke(material.gen_dir));
}

} // namespace mrl
