#pragma once

#include "color.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_info.hpp"
#include "materials/emit_info.hpp"
#include "textures/concepts.hpp"
#include "textures/solid_color.hpp"
#include <optional>
#include <utility>
namespace mrl {
template <typename Texture> struct diffuse_light {
  using texture_type = Texture;

  texture_type texture;
  double scale;

  constexpr diffuse_light(texture_type texture_, double scale_ = 1)
      : texture(std::move(texture_)), scale(scale_) {}

  constexpr diffuse_light(color_t color, double scale_ = 1)
      : texture(color), scale(scale_) {}
};

diffuse_light(color_t, double) -> diffuse_light<solid_color_texture>;

template <typename Texture>
diffuse_light(Texture, double) -> diffuse_light<Texture>;

template <DoubleGenerator Generator, Texture<Generator> texture_t>
std::optional<emit_info_t> emit(diffuse_light<texture_t> const &light,
                                hit_info_t const &hit_info,
                                generator_view<Generator> rand) {
  return emit_info_t{texture_color(light.texture, hit_info.scale_2d,
                                   hit_info.hit_point, rand) *
                     light.scale};
}
} // namespace mrl
