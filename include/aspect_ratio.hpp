#pragma once

#include <numeric>
namespace mrl {
class aspect_ratio_t {
private:
  // Invariant: width_ and height_ are always normalized
  int width_;
  int height_;

public:
  // Precondition: width > 0 && height > 0
  constexpr aspect_ratio_t(int width, int height)
      : width_(width), height_(height) {
    auto gcd = std::gcd(width_, height_);
    width_ /= gcd;
    height_ /= gcd;
  }

  constexpr int width() const { return width_; }
  constexpr int height() const { return height_; }

  constexpr double val() const { return double(width_) / height_; }
};

constexpr auto image_height(aspect_ratio_t ratio, int img_width) {
  return static_cast<int>(img_width / ratio.val());
}
} // namespace mrl
