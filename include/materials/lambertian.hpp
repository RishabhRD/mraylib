#pragma once

#include "color.hpp"
#include "direction.hpp"
#include "generator/concepts.hpp"
#include "generator/direction_generator.hpp"
#include "generator/generator_view.hpp"
#include "materials/scatter_record.hpp"
#include "normal.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "textures/concepts.hpp"
#include "textures/solid_color.hpp"
#include "textures/texture_coord.hpp"
#include "vector.hpp"
#include <optional>

namespace mrl {
template <typename Texture> struct lambertian_t {
  using texture_t = Texture;
  texture_t albedo;

  lambertian_t(texture_t texture) : albedo(std::move(texture)) {}

  lambertian_t(color_t color) : albedo(std::move(solid_color_texture{color})) {}
};

template <typename Texture> lambertian_t(Texture) -> lambertian_t<Texture>;
lambertian_t(color_t texture) -> lambertian_t<solid_color_texture>;

constexpr std::optional<scatter_record_t>
lambertian_scatter(color_t const &material_color, ray_t const &in_ray,
                   point3 hit_point, direction_t normal,
                   direction_t const &random_dir) {
  normal = normal_dir(normal, in_ray.direction);
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

template <DoubleGenerator Generator, Texture<Generator> texture_t>
constexpr auto
scatter(lambertian_t<texture_t> const &material, ray_t const &in_ray,
        point3 const &hit_point, texture_coord_t const &texture_coord,
        direction_t const &normal, generator_view<Generator> rand) {
  auto color = texture_color(material.albedo, texture_coord, hit_point, rand);
  return lambertian_scatter(color, in_ray, hit_point, normal,
                            direction_generator{}(rand));
}

} // namespace mrl
