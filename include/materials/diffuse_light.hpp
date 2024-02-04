#pragma once

#include "color.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "materials/emit_info.hpp"
#include "materials/material_context.hpp"
#include "textures/concepts.hpp"
#include "textures/solid_color.hpp"
#include <optional>
#include <utility>
namespace mrl {
template <typename Texture> struct diffuse_light {
  using texture_type = Texture;

  texture_type texture;

  constexpr diffuse_light(texture_type texture_)
      : texture(std::move(texture_)) {}

  constexpr diffuse_light(color_t color) : texture(color) {}
};

diffuse_light(color_t) -> diffuse_light<solid_color_texture>;

template <typename Texture> diffuse_light(Texture) -> diffuse_light<Texture>;

template <DoubleGenerator Generator, Texture<Generator> texture_t>
std::optional<emit_info_t> emit(diffuse_light<texture_t> const &light,
                                emission_context const &ctx,
                                generator_view<Generator> rand) {
  return emit_info_t{
      texture_color(light.texture, ctx.scaling_2d, ctx.hit_point, rand)};
}
} // namespace mrl
