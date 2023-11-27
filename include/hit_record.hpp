#pragma once

#include "direction.hpp"
#include "materials/scatter_record.hpp"
#include "point.hpp"
#include "std/optional.hpp"
#include <ostream>

namespace mrl {
struct hit_record_t {
  double t;
  point3 hit_point;
  direction_t normal;
  std::optional<scatter_record_t> scattering;

  friend std::ostream &operator<<(std::ostream &os, hit_record_t const &rec) {
    os << "{ t : " << rec.t << " , hit_point : " << rec.hit_point
       << " , normal : " << rec.normal << " , scattered : " << rec.scattering
       << " }";
    return os;
  }
};
} // namespace mrl
