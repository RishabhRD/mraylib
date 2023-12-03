#pragma once

#include "scene_objects/any_scene_object.hpp"
#include "schedulers/concepts.hpp"

namespace mrl {
template <Scheduler scheduler_t>
using scheduler_random_generator_t =
    std::remove_cvref_t<decltype(random_generator(
        std::declval<scheduler_t>()))>;

template <Scheduler scheduler_t>
using any_object_t =
    any_scene_object<scheduler_random_generator_t<scheduler_t>>;
} // namespace mrl
