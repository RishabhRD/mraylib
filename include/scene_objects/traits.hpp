#pragma once

#include "hit_info.hpp"
namespace mrl {
namespace __object_trait_details {
template <typename Object>
concept has_hit_object = requires(Object) { typename Object::hit_object_type; };
}

template <typename Object> struct hit_object;

template <__object_trait_details::has_hit_object Object>
struct hit_object<Object> {
  using type = typename Object::hit_object_type;
};

template <typename Object>
using hit_object_t = typename hit_object<Object>::type;

template <typename Object> using hit_info_of = hit_info_t<hit_object_t<Object>>;
} // namespace mrl
