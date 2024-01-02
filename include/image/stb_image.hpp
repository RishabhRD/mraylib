#pragma once

#include "color.hpp"
#include <stdexcept>
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include <stb_image.h>
#include <string_view>

namespace mrl {
class stb_image {
public:
  // Postcondition:
  //   - loads the filename image into data
  stb_image(std::string_view filename) {
    auto n = bytes_per_pixel;
    data = stbi_load(filename.data(), &width_, &height_, &n, bytes_per_pixel);
    bytes_per_scanline = width_ * bytes_per_pixel;
    if (data == nullptr)
      throw std::runtime_error(std::string{filename} + " failed to load.");
  }

  constexpr int width() const { return width_; }
  constexpr int height() const { return height_; }
  constexpr color_t pixel_at(int x, int y) const {
    x = clamp(x, 0, width_);
    y = clamp(y, 0, height_);
    auto pixel = data + y * bytes_per_scanline + x * bytes_per_pixel;

    return from_rgb(static_cast<int>(pixel[0]), static_cast<int>(pixel[1]),
                    static_cast<int>(pixel[2]));
  }

  ~stb_image() { STBI_FREE(data); }

  stb_image(stb_image &&other)
      : data(other.data), width_(other.width_), height_(other.height_),
        bytes_per_scanline(other.bytes_per_scanline),
        bytes_per_pixel(other.bytes_per_pixel) {
    other.data = nullptr;
  }

private:
  int clamp(int x, int low, int high) const {
    if (x < low)
      return low;
    if (x < high)
      return x;
    return high - 1;
  }
  unsigned char *data;
  int width_;
  int height_;
  int bytes_per_scanline;
  int bytes_per_pixel{3};
};

constexpr auto width(stb_image const &img) { return img.width(); }
constexpr auto height(stb_image const &img) { return img.height(); }
constexpr auto pixel_at(stb_image const &img, int x, int y) {
  return img.pixel_at(x, y);
}
} // namespace mrl

#ifdef _MSC_VER
#pragma warning(pop)
#endif
