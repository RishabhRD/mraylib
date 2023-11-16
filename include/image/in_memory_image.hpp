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

  color_t &at(int row, int col) {
    return pixels[static_cast<std::size_t>(row * width_ + col)];
  }

  color_t const &at(int row, int col) const {
    return pixels[static_cast<std::size_t>(row * width_ + col)];
  }

  constexpr int width() const { return width_; }

  constexpr int height() const { return height_; }
};
} // namespace mrl
