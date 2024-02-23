#pragma once

#include <optional>
#include <ostream>

namespace mrl {
template <typename T>
std::ostream &operator<<(std::ostream &os, std::optional<T> const &e) {
  if (e) {
    os << *e;
  } else {
    os << "nullopt";
  }
  return os;
}
} // namespace mrl
