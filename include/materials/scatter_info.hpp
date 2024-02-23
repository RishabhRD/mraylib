#pragma once

#include "color.hpp"
#include "ray.hpp"

namespace mrl {
struct scatter_info_t {
  ray_t scattered_ray;
  color_t attenuated_color;

  friend std::ostream &operator<<(std::ostream &os, scatter_info_t const &rec) {
    os << "{ scattered_ray : " << rec.scattered_ray
       << " , attenuated_color : " << rec.attenuated_color << " }";
    return os;
  }
};
} // namespace mrl
