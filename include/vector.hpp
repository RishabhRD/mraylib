#pragma once

#include "angle.hpp"
#include "utils/double_utils.hpp"
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
    return out << "{ x : " << vec.x << " , "
               << "y : " << vec.y << " , "
               << "z : " << vec.z << " }";
  }

  constexpr friend vec3 operator+(vec3 a, vec3 const &b) { return a += b; }

  constexpr friend vec3 operator-(vec3 a, vec3 const &b) { return a -= b; }

  constexpr friend vec3 operator*(vec3 a, vec3 const &b) { return a *= b; }

  constexpr friend vec3 operator*(vec3 a, double b) { return a *= b; }

  constexpr friend vec3 operator*(double a, vec3 b) { return b *= a; }

  constexpr friend vec3 operator/(vec3 a, double b) { return a /= b; }

  constexpr friend bool operator==(vec3 const &a, vec3 const &b) {
    return is_equal(a.x, b.x) && is_equal(a.y, b.y) && is_equal(a.z, b.z);
  }
};

constexpr double dot(vec3 const &a, vec3 const &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

constexpr vec3 cross(vec3 const &a, vec3 const &b) {
  return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

constexpr vec3 unit_vector(vec3 const &v) { return v / v.length(); }

constexpr vec3 normalize(vec3 const &v) { return unit_vector(v); }

constexpr bool near_zero(vec3 const &v) {
  constexpr static auto s = 1e-8;
  return std::fabs(v.x) < s && std::fabs(v.y) < s && std::fabs(v.z) < s;
}

constexpr vec3 rotate_x(vec3 const &vector, angle_t angle) {
  double angle_rad = radians(angle);
  double cos_a = std::cos(angle_rad);
  double sin_a = std::sin(angle_rad);

  return {
      vector.x,
      cos_a * vector.y - sin_a * vector.z,
      sin_a * vector.y + cos_a * vector.z,
  };
}

constexpr vec3 rotate_y(vec3 const &vector, angle_t angle) {
  double angle_rad = radians(angle);
  double cos_a = cos(angle_rad);
  double sin_a = sin(angle_rad);

  return {
      cos_a * vector.x + sin_a * vector.z,
      vector.y,
      sin_a * vector.x - cos_a * vector.z,
  };
}

constexpr vec3 rotate_z(vec3 const &vector, angle_t angle) {
  double angle_rad = radians(angle);
  double cos_a = cos(angle_rad);
  double sin_a = sin(angle_rad);

  return {
      cos_a * vector.x - sin_a * vector.y,
      sin_a * vector.x + cos_a * vector.y,
      vector.z,
  };
}
} // namespace mrl
