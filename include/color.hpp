#pragma once

namespace mrl {
struct color_t {
  double r;
  double g;
  double b;

  friend bool operator==(color_t const &, color_t const &) = default;
};
} // namespace mrl
