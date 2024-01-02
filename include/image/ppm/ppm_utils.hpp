#pragma once

#include "color.hpp"
#include "image/concepts.hpp"
#include <sstream>
namespace mrl {
inline auto write_ppm_color_str(std::ostream &os, color_t color) {
  auto [r, g, b] = to_rgb_gamma(color);
  os << r << ' ' << g << ' ' << b << '\n';
}

inline auto convert_to_ppm_str(color_t color) {
  std::stringstream ss;
  write_ppm_color_str(ss, color);
  return ss.str();
}

template <RandomAccessImage Image>
void write_ppm_img(std::ostream &os, Image const &img) {
  os << "P3\n";
  os << width(img) << ' ' << height(img) << '\n';
  os << "\n255\n";
  for (int j = 0; j < height(img); ++j) {
    for (int i = 0; i < width(img); ++i) {
      write_ppm_color_str(os, pixel_at(img, j, i));
    }
  }
}

template <RandomAccessImage Image>
auto make_ppm_img_str(Image const &img, std::ostream &os) {
  std::stringstream ss;
  write_ppm_img(img, os);
  return ss.str();
}
} // namespace mrl
