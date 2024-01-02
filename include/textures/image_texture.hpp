#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "image/concepts.hpp"
#include "interval.hpp"
#include "point.hpp"
#include "textures/texture_coord.hpp"
namespace mrl {
namespace texture {
template <RandomAccessImage Image> struct image_texture {
  using image_t = Image;
  image_t image;

  image_texture(image_t img) : image{std::move(img)} {}
};

template <RandomAccessImage Image> image_texture(Image) -> image_texture<Image>;

template <RandomAccessImage Image, DoubleGenerator Generator>
constexpr color_t texture_color(image_texture<Image> const &texture,
                                texture_coord_t coord, point3 const &,
                                generator_view<Generator>) {
  coord.u = clamp(interval_t{0.0, 1.0}, coord.u);
  coord.v = 1.0 - clamp(interval_t{0.0, 1.0}, coord.v);

  auto i = static_cast<int>(coord.u * width(texture.image));
  auto j = static_cast<int>(coord.v * height(texture.image));
  return pixel_at(texture.image, i, j);
}
} // namespace texture
} // namespace mrl
