#pragma once

#include "vector.hpp"
namespace mrl {

template <std::regular T> using point3_basic = vec3_basic<T>;
using point3 = point3_basic<double>;

} // namespace mrl
