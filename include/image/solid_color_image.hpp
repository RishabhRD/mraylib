#pragma once

#include "color.hpp"
namespace mrl {
struct solid_color_image {
  color_t color;
  int width;
  int height;
};

constexpr int width(solid_color_image const &img) { return img.width; }
constexpr int height(solid_color_image const &img) { return img.height; }
constexpr color_t pixel_at(solid_color_image const &img, int, int) {
  return img.color;
}
} // namespace mrl
