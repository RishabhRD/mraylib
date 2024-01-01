#pragma once

#include "direction.hpp"
#include "materials/scatter_record.hpp"
#include "point.hpp"
#include <optional>
#include <ostream>

namespace mrl {
// TODO: there should be generic << for optional<T>
// But somehow that is not working
inline std::ostream &operator<<(std::ostream &os,
                                std::optional<scatter_record_t> const &e) {
  if (e) {
    os << *e;
  } else {
    os << "nullopt";
  }
  return os;
}

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
