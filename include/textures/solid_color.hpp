#pragma once

#include "color.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "point.hpp"
#include "textures/texture_coord.hpp"
namespace mrl {
namespace texture {
struct solid_color {
  color_t color;
};

template <DoubleGenerator Generator>
constexpr color_t texture_color(solid_color const &texture,
                                texture_coord_t const &, point3 const &,
                                generator_view<Generator>) {
  return texture.color;
}
} // namespace texture
} // namespace mrl
