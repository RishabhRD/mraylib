#pragma once

#include "direction.hpp"
#include "point.hpp"
#include <ostream>
namespace mrl {
struct hit_record_t {
  double t;
  point3 hit_point;
  direction_t normal;

  friend std::ostream &operator<<(std::ostream &os, hit_record_t const &rec) {
    os << "{ t : " << rec.t << " , hit_point : " << rec.hit_point
       << " , normal : " << rec.normal << " }";
    return os;
  }
};
} // namespace mrl
