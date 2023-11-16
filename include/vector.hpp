#pragma once

#include <cmath>
#include <concepts>
#include <ostream>
namespace mrl {
struct vec3 {
  double x;
  double y;
  double z;

  constexpr vec3 operator-() const { return {-x, -y, -z}; }

  constexpr vec3 &operator+=(vec3 const &v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  constexpr vec3 &operator-=(vec3 const &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  constexpr vec3 &operator*=(double d) {
    x *= d;
    y *= d;
    z *= d;
    return *this;
  }

  constexpr vec3 &operator*=(vec3 const &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  constexpr vec3 &operator/=(double d) {
    x /= d;
    y /= d;
    z /= d;
    return *this;
  }

  constexpr double length() const { return std::sqrt(length_square()); }

  constexpr double length_square() const { return x * x + y * y + z * z; }

  friend std::ostream &operator<<(std::ostream &out, vec3 const &vec) {
    return out << '(' << vec.x << ' ' << vec.y << ' ' << vec.z << ')';
  }

  constexpr friend vec3 operator+(vec3 a, vec3 const &b) { return a += b; }

  constexpr friend vec3 operator-(vec3 a, vec3 const &b) { return a -= b; }

  constexpr friend vec3 operator*(vec3 a, vec3 const &b) { return a *= b; }

  constexpr friend vec3 operator*(vec3 a, double b) { return a *= b; }

  constexpr friend vec3 operator*(double a, vec3 b) { return b *= a; }

  constexpr friend vec3 operator/(vec3 a, double b) { return a /= b; }
};

constexpr double dot(vec3 const &a, vec3 const &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

constexpr vec3 cross(vec3 const &a, vec3 const &b) {
  return {a.y * b.z - a.z - b.y, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y};
}

constexpr vec3 unit_vector(vec3 const &v) { return v / v.length(); }

constexpr vec3 normalize(vec3 const &v) { return unit_vector(v); }
} // namespace mrl
