#pragma once

#include <ostream>
namespace mrl {
// Class invariant:
//   - u : [0, 1]
//   - v : [0, 1]
struct texture_coord_t {
  double u;
  double v;
};

inline std::ostream &operator<<(std::ostream &os,
                                texture_coord_t const &coord) {
  os << "{ u: " << coord.u << " , v: " << coord.v << " }";
  return os;
}
} // namespace mrl
