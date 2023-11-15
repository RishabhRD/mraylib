#pragma once

namespace mrl {
struct color {
  double r;
  double g;
  double b;

  friend bool operator==(color const &, color const &) = default;
};
} // namespace mrl
