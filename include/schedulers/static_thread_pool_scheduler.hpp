#pragma once

#include "generator/thread_local_random_double_generator.hpp"
#include <exec/static_thread_pool.hpp>
namespace mrl {
using static_thread_pool = exec::static_thread_pool;
using thread_pool_scheduler = exec::static_thread_pool::scheduler;
} // namespace mrl

namespace exec {
inline auto random_generator(mrl::thread_pool_scheduler) {
  return mrl::thread_local_random_double_generator{};
}
} // namespace exec
