#pragma once

#include "color.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_info.hpp"
#include "materials/emit_info.hpp"
#include "textures/concepts.hpp"
#include "textures/solid_color.hpp"
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

template <DoubleGenerator Generator, Texture<Generator> texture_t>
emit_info_t emit(diffuse_light<texture_t> const &light,
                 hit_info_t const &hit_info, generator_view<Generator> rand) {
  return texture_color(light.texture, hit_info.scale_2d, hit_info.hit_point,
                       rand);
}
} // namespace mrl
