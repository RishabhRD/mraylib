#pragma once

#include <limits>
namespace mrl {
constexpr auto
is_equal(double a, double b,
         double tolerance = std::numeric_limits<double>::epsilon()) {
  return (a - b) <= tolerance && (a - b) >= -tolerance;
}
} // namespace mrl
