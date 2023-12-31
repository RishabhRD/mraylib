#pragma once

#include "generator/random_double_generator.hpp"
#include <exec/inline_scheduler.hpp>
namespace mrl {
using inline_scheduler = exec::inline_scheduler;

} // namespace mrl

namespace stdexec {
namespace __inln {
inline auto random_generator(mrl::inline_scheduler, unsigned long random_seed) {
  return mrl::random_double_generator{random_seed};
}
} // namespace __inln
} // namespace stdexec
