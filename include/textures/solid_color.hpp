#pragma once

#include "color.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "point.hpp"
#include "scale_2d.hpp"
namespace mrl {
struct solid_color_texture {
  color_t color;
};

template <DoubleGenerator Generator>
constexpr color_t texture_color(solid_color_texture const &texture,
                                scale_2d_t const &, point3 const &,
                                generator_view<Generator>) {
  return texture.color;
}
} // namespace mrl
