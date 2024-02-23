#pragma once

namespace mrl {
template <typename HitObject> struct hit_info_t {
  using hit_object_t = HitObject;

  double hit_distance;
  hit_object_t hit_object;
};

} // namespace mrl
