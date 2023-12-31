#pragma once

#include "color.hpp"
namespace mrl {
template <typename Image>
concept RandomAccessImage =
    requires(Image const &img, int x, int y, color_t const &color) {
      { width(img) } -> std::same_as<int>;
      { height(img) } -> std::same_as<int>;
      { pixel_at(img, x, y) } -> std::convertible_to<color_t>;
    };

template <typename Image>
concept OutputRandomAccessImage =
    RandomAccessImage<Image> &&
    requires(Image &img_mut, int x, int y, color_t const &color) {
      { set_pixel_at(img_mut, x, y, color) };
    };
} // namespace mrl
