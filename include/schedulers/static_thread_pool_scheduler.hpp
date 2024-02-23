#pragma once

#include "generator/thread_local_random_double_generator.hpp"
#include <exec/static_thread_pool.hpp>
namespace mrl {
using static_thread_pool = exec::static_thread_pool;
using static_thread_pool_scheduler = exec::static_thread_pool::scheduler;
} // namespace mrl

namespace exec {
inline auto random_generator(mrl::static_thread_pool_scheduler,
                             unsigned long random_seed) {
  return mrl::thread_local_random_double_generator{random_seed};
}
} // namespace exec
