#pragma once

#include "color.hpp"
#include <vector>

namespace mrl {
class in_memory_image {
private:
  int width_;
  int height_;
  std::vector<color_t> pixels;

public:
  in_memory_image(int width, int height)
      : width_(width), height_(height),
        pixels(static_cast<std::size_t>(width_ * height_)) {}

  color_t &at(int x, int y) {
    return pixels[static_cast<std::size_t>(y * width_ + x)];
  }

  color_t const &at(int x, int y) const {
    return pixels[static_cast<std::size_t>(y * width_ + x)];
  }

  constexpr int width() const { return width_; }

  constexpr int height() const { return height_; }
};

constexpr auto width(in_memory_image const &img) { return img.width(); }
constexpr auto height(in_memory_image const &img) { return img.height(); }
constexpr auto pixel_at(in_memory_image const &img, int x, int y) {
  return img.at(x, y);
}
constexpr auto set_pixel_at(in_memory_image &img, int x, int y, color_t color) {
  img.at(x, y) = color;
}
} // namespace mrl
