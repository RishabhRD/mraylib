#pragma once

#include "generator/concepts.hpp"
#include "hit_context.hpp"
#include "interval.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include <memory>
#include <optional>

namespace mrl {
template <DoubleGenerator Generator> class any_scene_object {
public:
  // TODO: Using SceneObject here is creating problem to use it with
  // scene_object_list with any algorithm that depends on SceneObject.
  //
  template <typename T>
  any_scene_object(T x) : self_(std::make_shared<model_t<T>>(std::move(x))) {}

  friend std::optional<hit_context_t> hit(any_scene_object const &obj,
                                          ray_t const &ray,
                                          interval_t const &t_rng,
                                          generator_view<Generator> rand) {
    return obj.self_->hit_mem(ray, t_rng, rand);
  }

private:
  struct concept_t {
    virtual ~concept_t() = default;
    virtual std::optional<hit_context_t>
    hit_mem(ray_t const &, interval_t const &,
            generator_view<Generator> rand) const = 0;
    virtual bound_t get_bounds_mem() const = 0;
  };

  template <SceneObject<Generator> T> struct model_t final : concept_t {
    T hittable;
    model_t(T h_arg) : hittable(std::move(h_arg)){};

    std::optional<hit_context_t>
    hit_mem(ray_t const &ray, interval_t const &t_rng,
            generator_view<Generator> rand) const override {
      return hit(hittable, ray, t_rng, rand);
    }

    bound_t get_bounds_mem() const override { return get_bounds(hittable); }
  };

public:
  std::shared_ptr<concept_t const> self_;
};

template <DoubleGenerator Generator>
std::optional<hit_context_t> hit(any_scene_object<Generator> const &,
                                 ray_t const &, interval_t const &,
                                 generator_view<Generator>);

template <DoubleGenerator Generator>
bound_t get_bounds(any_scene_object<Generator> const &obj) {
  return obj.self_->get_bounds_mem();
}
} // namespace mrl
