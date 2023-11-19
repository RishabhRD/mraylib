#pragma once

#include "concepts.hpp"
#include <optional>
#include <ostream>

template <mrl::Streamable T>
std::ostream &operator<<(std::ostream &os, std::optional<T> const &opt) {
  if (opt) {
    os << "nullopt";
  } else {
    os << *opt;
  }
  return os;
}
