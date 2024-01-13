#pragma once

#include "hit_info.hpp"
#include "materials/emit_info.hpp"
#include "materials/scatter_info.hpp"
#include "std/optional.hpp"
#include <optional>
#include <ostream>

namespace mrl {
struct hit_context_t {
  hit_info_t hit_info;
  std::optional<scatter_info_t> scatter_info;
  std::optional<emit_info_t> emit_info;
};

inline std::ostream &operator<<(std::ostream &os, hit_context_t const &rec) {
  os << "{ hit_info : " << rec.hit_info
     << " , scatter_info : " << rec.scatter_info
     << " , emit_info : " << rec.emit_info << " }";
  return os;
}
} // namespace mrl
