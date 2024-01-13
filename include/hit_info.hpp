#pragma once

#include "direction.hpp"
#include "point.hpp"
#include "scale_2d.hpp"
namespace mrl {
struct hit_info_t {
  double t;
  point3 hit_point;
  direction_t outward_normal;
  scale_2d_t scale_2d;
};

inline std::ostream &operator<<(std::ostream &os, hit_info_t const &rec) {
  os << "{ t : " << rec.t << " , hit_point : " << rec.hit_point
     << " , normal : " << rec.outward_normal << " }";
  return os;
}
} // namespace mrl
