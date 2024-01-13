#pragma once

#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "image/concepts.hpp"
#include "interval.hpp"
#include "point.hpp"
#include "scale_2d.hpp"
namespace mrl {
template <RandomAccessImage Image> struct image_texture {
  using image_t = Image;
  image_t image;

  image_texture(image_t img) : image{std::move(img)} {}
};

template <RandomAccessImage Image> image_texture(Image) -> image_texture<Image>;

template <RandomAccessImage Image, DoubleGenerator Generator>
constexpr color_t texture_color(image_texture<Image> const &texture,
                                scale_2d_t coord, point3 const &,
                                generator_view<Generator>) {
  auto x_scale = clamp(interval_t{0.0, 1.0}, coord.x_scale());
  auto y_scale = 1.0 - clamp(interval_t{0.0, 1.0}, coord.y_scale());

  auto i = static_cast<int>(x_scale * width(texture.image));
  auto j = static_cast<int>(y_scale * height(texture.image));
  return pixel_at(texture.image, i, j);
}
} // namespace mrl
