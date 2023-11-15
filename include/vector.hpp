#pragma once

#include <cmath>
#include <concepts>
#include <ostream>
namespace mrl {
template <std::regular T> struct vec3_basic {
  using component_type = T;

  T x;
  T y;
  T z;

  constexpr vec3_basic operator-() const { return {-x, -y, -z}; }

  constexpr vec3_basic &operator+=(vec3_basic const &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  constexpr vec3_basic &operator-=(vec3_basic const &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  constexpr vec3_basic &operator*=(T d) {
    x *= d;
    y *= d;
    z *= d;
    return *this;
  }

  constexpr vec3_basic &operator*=(vec3_basic const &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  constexpr vec3_basic &operator/=(T d) {
    x /= d;
    y /= d;
    z /= d;
    return *this;
  }

  constexpr T length() const { return std::sqrt(length_square()); }

  constexpr T length_square() const { return x * x + y * y + z * z; }

  friend std::ostream &operator<<(std::ostream &out, vec3_basic const &vec) {
    return out << '(' << vec.x << ' ' << vec.y << ' ' << vec.z << ')';
  }

  constexpr friend vec3_basic operator+(vec3_basic a, vec3_basic const &b) {
    return a += b;
  }

  constexpr friend vec3_basic operator-(vec3_basic a, vec3_basic const &b) {
    return a -= b;
  }

  constexpr friend vec3_basic operator*(vec3_basic a, vec3_basic const &b) {
    return a *= b;
  }

  constexpr friend vec3_basic operator*(vec3_basic a, T b) { return a *= b; }

  constexpr friend vec3_basic operator*(T a, vec3_basic b) { return b *= a; }

  constexpr friend vec3_basic operator/(vec3_basic a, T b) { return a /= b; }
};

template <typename T>
constexpr T dot(vec3_basic<T> const &a, vec3_basic<T> const &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

template <typename T>
constexpr vec3_basic<T> cross(vec3_basic<T> const &a, vec3_basic<T> const &b) {
  return {a.y * b.z - a.z - b.y, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y};
}

template <typename T>
constexpr vec3_basic<T> unit_vector(vec3_basic<T> const &v) {
  return v / v.length();
}

using vec3 = vec3_basic<double>;
} // namespace mrl
