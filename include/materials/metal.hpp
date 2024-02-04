#pragma once

#include "color.hpp"
#include "direction.hpp"
#include "generator/concepts.hpp"
#include "generator/direction_generator.hpp"
#include "generator/generator_view.hpp"
#include "hit_info.hpp"
#include "light.hpp"
#include "materials/scatter_info.hpp"
#include "normal.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <optional>

namespace mrl {
struct metal_t {
  color_t albedo;
};

template <DoubleGenerator Generator>
constexpr std::optional<scatter_info_t>
scatter(metal_t const &material, ray_t const &in_ray,
        hit_info_t const &hit_info, generator_view<Generator>) {
  auto normal = normal_dir(hit_info.outward_normal, in_ray.direction);
  auto ray_dir = reflection_dir(in_ray.direction, normal);
  ray_t scattered_ray{.origin = hit_info.hit_point, .direction = ray_dir};
  return scatter_info_t{
      .scattered_ray = scattered_ray,
      .attenuated_color = material.albedo,
  };
}

struct fuzzy_metal_t {
  color_t albedo;
  double fuzz_factor;
};

template <DoubleGenerator Generator>
constexpr std::optional<scatter_info_t>
scatter(fuzzy_metal_t const &material, ray_t const &in_ray,
        hit_info_t const &hit_info, generator_view<Generator> rand) {
  auto normal = normal_dir(hit_info.outward_normal, in_ray.direction);
  auto ray_dir = reflection_dir(in_ray.direction, normal);
  ray_t scattered_ray{.origin = hit_info.hit_point,
                      .direction = ray_dir.val() +
                                   material.fuzz_factor *
                                       direction_generator{}(rand).val()};
  if (dot(scattered_ray.direction.val(), normal.val()) <= 0)
    return std::nullopt;
  return scatter_info_t{
      .scattered_ray = scattered_ray,
      .attenuated_color = material.albedo,
  };
}
} // namespace mrl
