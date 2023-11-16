#pragma once

#include "color.hpp"
#include <sstream>
namespace mrl {
inline auto convert_to_ppm_str(color_t color) {
  std::stringstream ss;
  ss << static_cast<int>(255.999 * color.r) << ' '
     << static_cast<int>(255.999 * color.g) << ' '
     << static_cast<int>(255.999 * color.b) << '\n';
  return ss.str();
}
} // namespace mrl
