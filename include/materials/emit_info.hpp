#pragma once

#include "color.hpp"
namespace mrl {
struct emit_info_t {
  color_t color;

  friend std::ostream &operator<<(std::ostream &os, emit_info_t const &rec) {
    os << "{ color : " << rec.color << " }";
    return os;
  }
};
} // namespace mrl
