#pragma once

#include "direction.hpp"
#include "vector.hpp"
#include <ostream>
#include <tuple>
namespace mrl {
// color_t is (r, g, b) representation of color
// Each component lies between [0, 1]
struct color_t {
  // Class Invariant, Constructor Precondition:
  //   - r, g, b >= 0 && r, g, b <= 1
  double r;
  double g;
  double b;

  // All Method precondition:
  //   - Arguments should be such that it doesn't violate class invariant

  constexpr color_t &operator+=(color_t const &v) {
    r += v.r;
    g += v.g;
    b += v.b;
    return *this;
  }

  constexpr color_t &operator-=(color_t const &v) {
    r -= v.r;
    g -= v.g;
    b -= v.b;
    return *this;
  }

  constexpr color_t &operator*=(double d) {
    r *= d;
    g *= d;
    b *= d;
    return *this;
  }

  constexpr color_t &operator*=(color_t const &v) {
    r *= v.r;
    g *= v.g;
    b *= v.b;
    return *this;
  }

  constexpr color_t &operator/=(double d) {
    r /= d;
    g /= d;
    b /= d;
    return *this;
  }

  friend std::ostream &operator<<(std::ostream &out, color_t const &vec) {
    return out << "{ r : " << vec.r << " , "
               << "g : " << vec.g << " , "
               << "b : " << vec.b << " }";
  }
};
constexpr color_t operator+(color_t a, color_t const &b) { return a += b; }

constexpr color_t operator-(color_t a, color_t const &b) { return a -= b; }

constexpr color_t operator*(color_t a, color_t const &b) { return a *= b; }

constexpr color_t operator*(color_t a, double b) { return a *= b; }

constexpr color_t operator*(double a, color_t b) { return b *= a; }

constexpr color_t operator/(color_t a, double b) { return a /= b; }

// Precondition:
//   - r, g, b >= 0 && r, g, b <= 255
constexpr color_t from_rgb(int r, int g, int b) {
  vec3 tmp = normalize(vec3(r, g, b));
  return {tmp.x, tmp.y, tmp.z};
}

constexpr std::tuple<int, int, int> to_rgb(color_t color) {
  return {
      static_cast<int>(255.999 * color.r),
      static_cast<int>(255.999 * color.g),
      static_cast<int>(255.999 * color.b),
  };
}
} // namespace mrl
