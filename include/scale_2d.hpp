#pragma once

#include <ostream>
namespace mrl {
class scale_2d_t {
  // Class Invariant:
  //   - x_scale is in range [0, 1]
  //   - y_scale is in range [0, 1]
private:
  double x_scale_;
  double y_scale_;

public:
  // Precondition:
  //   - x_scale, y_scale is in range [0, 1]
  constexpr scale_2d_t(double x_scale, double y_scale)
      : x_scale_(x_scale), y_scale_(y_scale) {}

  constexpr double x_scale() const { return x_scale_; }
  constexpr double y_scale() const { return y_scale_; }
};

inline std::ostream &operator<<(std::ostream &os, scale_2d_t const &coord) {
  os << "{ x_scale: " << coord.x_scale() << " , y_scale: " << coord.y_scale()
     << " }";
  return os;
}
} // namespace mrl
