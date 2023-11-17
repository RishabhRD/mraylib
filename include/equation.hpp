#pragma once

namespace mrl {
constexpr auto calc_discriminant(double a, double b, double c) {
  return b * b - 4 * a * c;
}
} // namespace mrl
