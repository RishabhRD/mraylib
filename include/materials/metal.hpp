#pragma once

#include "color.hpp"
#include "direction.hpp"
#include "generator/concepts.hpp"
#include "generator/direction_generator.hpp"
#include "generator/generator_view.hpp"
#include "light.hpp"
#include "materials/material_context.hpp"
#include "materials/scatter_info.hpp"
#include "normal.hpp"
#include "ray.hpp"
#include "textures/concepts.hpp"
#include "textures/solid_color.hpp"
#include "vector.hpp"
#include <optional>

namespace mrl {
template <typename Texture> struct metal_t {
  using texture_type = Texture;
  texture_type albedo;

  metal_t(color_t color) : albedo(solid_color_texture{color}) {}
  metal_t(texture_type albedo_arg) : albedo(std::move(albedo_arg)) {}
};

metal_t(color_t) -> metal_t<solid_color_texture>;
template <typename Texture> metal_t(Texture) -> metal_t<Texture>;

template <DoubleGenerator Generator, Texture<Generator> texture_t>
constexpr std::optional<scatter_info_t>
scatter(metal_t<texture_t> const &material, scattering_context const &ctx,
        generator_view<Generator> rand) {
  auto color =
      texture_color(material.albedo, ctx.scaling_2d, ctx.hit_point, rand);
  auto const &ray = ctx.ray;
  auto normal = normal_dir(ctx.normal, ray.direction);
  auto ray_dir = reflection_dir(ray.direction, normal);
  ray_t scattered_ray{.origin = ctx.hit_point, .direction = ray_dir};
  return scatter_info_t{
      .scattered_ray = scattered_ray,
      .attenuated_color = color,
  };
}

template <typename Texture> struct fuzzy_metal_t {
  using texture_type = Texture;

  texture_type albedo;
  double fuzz_factor;

  fuzzy_metal_t(color_t color, double fuzz)
      : albedo(solid_color_texture{color}), fuzz_factor(fuzz) {}

  fuzzy_metal_t(texture_type albedo_arg, double fuzz)
      : albedo(std::move(albedo_arg)), fuzz_factor(fuzz) {}
};

template <typename Texture>
fuzzy_metal_t(Texture, double) -> fuzzy_metal_t<Texture>;
fuzzy_metal_t(color_t, double) -> fuzzy_metal_t<solid_color_texture>;

template <DoubleGenerator Generator, Texture<Generator> texture_t>
constexpr std::optional<scatter_info_t>
scatter(fuzzy_metal_t<texture_t> const &material, scattering_context const &ctx,
        generator_view<Generator> rand) {
  auto color =
      texture_color(material.albedo, ctx.scaling_2d, ctx.hit_point, rand);
  auto const &ray = ctx.ray;
  auto normal = normal_dir(ctx.normal, ray.direction);
  auto ray_dir = reflection_dir(ray.direction, normal);
  ray_t scattered_ray{.origin = ctx.hit_point,
                      .direction = ray_dir.val() +
                                   material.fuzz_factor *
                                       direction_generator{}(rand).val()};
  if (dot(scattered_ray.direction.val(), normal.val()) <= 0)
    return std::nullopt;
  return scatter_info_t{
      .scattered_ray = scattered_ray,
      .attenuated_color = color,
  };
}
} // namespace mrl
