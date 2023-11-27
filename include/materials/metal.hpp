#pragma once

#include "color.hpp"
#include "direction.hpp"
#include "light.hpp"
#include "materials/scatter_record.hpp"
#include "random/random_vector.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <functional>
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

template <std::invocable DirectionGenerator = random_direction_generator>
  requires std::same_as<std::invoke_result_t<DirectionGenerator>, direction_t>
class basic_fuzzy_metal_t {
public:
  color_t albedo;
  double fuzz_factor;
  mutable DirectionGenerator gen_dir;

public:
  basic_fuzzy_metal_t(
      color_t albedo_, double fuzz_factor_,
      DirectionGenerator gen_dir_ = random_direction_generator())
      : albedo(std::move(albedo_)), fuzz_factor(fuzz_factor_),
        gen_dir(std::move(gen_dir_)) {}
};

using fuzzy_metal_t = basic_fuzzy_metal_t<>;

// Precondition:
//   - normal is st dot(normal, ray.dir) <= 0
template <std::invocable DirectionGenerator>
  requires std::same_as<std::invoke_result_t<DirectionGenerator>, direction_t>
constexpr std::optional<scatter_record_t>
scatter(basic_fuzzy_metal_t<DirectionGenerator> const &material,
        ray_t const &in_ray, point3 const &hit_point, direction_t normal) {
  auto ray_dir = reflection_dir(in_ray.direction, normal);
  ray_t scattered_ray{.origin = hit_point,
                      .direction = ray_dir.val() +
                                   material.fuzz_factor *
                                       std::invoke(material.gen_dir).val()};
  if (dot(scattered_ray.direction.val(), normal.val()) <= 0)
    return std::nullopt;
  return scatter_record_t{
      .scattered_ray = scattered_ray,
      .attenuated_color = material.albedo,
  };
}

template <std::invocable DirectionGenerator>
  requires std::same_as<std::invoke_result_t<DirectionGenerator>, direction_t>
basic_fuzzy_metal_t(color_t, double, DirectionGenerator)
    -> basic_fuzzy_metal_t<DirectionGenerator>;
} // namespace mrl
