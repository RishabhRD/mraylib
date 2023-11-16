#pragma once

#include <ostream>
namespace mrl {
struct color_t {
  double r;
  double g;
  double b;
  constexpr color_t operator-() const { return {-r, -g, -b}; }

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
} // namespace mrl
