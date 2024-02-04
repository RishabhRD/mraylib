#pragma once

#include "direction.hpp"
#include "generator/concepts.hpp"
#include "generator/generator_view.hpp"
#include "hit_info.hpp"
#include "interval.hpp"
#include "materials/emit_info.hpp"
#include "materials/scatter_info.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "scale_2d.hpp"
#include "scene_objects/concepts.hpp"
#include <memory>
#include <optional>

namespace mrl {
template <DoubleGenerator Generator> struct any_hit_object {
  template <typename Object>
  any_hit_object(Object o)
      : self_(std::make_shared<model_t<Object>>(std::move(o))) {}

  struct concept_t {
    virtual ~concept_t() = default;
    virtual direction_t normal_at_mem(point3 const &) const = 0;
    virtual scale_2d_t scaling_2d_at_mem(point3 const &) const = 0;
    virtual std::optional<scatter_info_t>
    scattering_for_mem(ray_t const &, double,
                       generator_view<Generator>) const = 0;
    virtual std::optional<emit_info_t>
    emission_at_mem(point3 const &, generator_view<Generator>) const = 0;
  };

  template <HitObject<Generator> T> struct model_t final : concept_t {
    model_t(T h_arg) : obj(std::move(h_arg)){};

    direction_t normal_at_mem(point3 const &p) const override {
      return normal_at(obj, p);
    };
    scale_2d_t scaling_2d_at_mem(point3 const &p) const override {
      return scaling_2d_at(obj, p);
    };
    std::optional<scatter_info_t>
    scattering_for_mem(ray_t const &r, double hit_distance,
                       generator_view<Generator> rand) const override {
      return scattering_for(obj, r, hit_distance, rand);
    };
    std::optional<emit_info_t>
    emission_at_mem(point3 const &p,
                    generator_view<Generator> rand) const override {
      return emission_at(obj, p, rand);
    };

    T obj;
  };

  std::shared_ptr<concept_t const> self_;
};

template <typename Object>
auto normal_at(any_hit_object<Object> const &o, point3 const &p) {
  return o.self_->normal_at_mem(p);
}
template <typename Object>
auto scaling_2d_at(any_hit_object<Object> const &o, point3 const &p) {
  return o.self_->scaling_2d_at_mem(p);
}
template <DoubleGenerator Generator>
auto scattering_for(any_hit_object<Generator> const &o, ray_t const &r,
                    double hit_distance, generator_view<Generator> rand) {
  return o.self_->scattering_for_mem(r, hit_distance, rand);
}
template <DoubleGenerator Generator>
auto emission_at(any_hit_object<Generator> const &o, point3 const &p,
                 generator_view<Generator> rand) {
  return o.self_->emission_at_mem(p, rand);
}

template <DoubleGenerator Generator> struct any_scene_object {
  using hit_object_type = any_hit_object<Generator>;

  template <typename T>
  any_scene_object(T x) : self_(std::make_shared<model_t<T>>(std::move(x))) {}

  struct concept_t {
    virtual ~concept_t() = default;
    virtual std::optional<hit_info_t<hit_object_type>>
    hit_mem(ray_t const &, interval_t const &) const = 0;
    virtual bound_t get_bounds_mem() const = 0;
  };

  template <SceneObject T> struct model_t final : concept_t {
    T hittable;
    model_t(T h_arg) : hittable(std::move(h_arg)){};

    std::optional<hit_info_t<hit_object_type>>
    hit_mem(ray_t const &ray, interval_t const &t_rng) const override {
      auto res = hit(hittable, ray, t_rng);
      if (!res)
        return std::nullopt;
      return hit_info_t<hit_object_type>{
          .hit_distance = res->hit_distance,
          .hit_object = hit_object_type{std::move(res->hit_object)},
      };
    }

    bound_t get_bounds_mem() const override { return get_bounds(hittable); }
  };

public:
  std::shared_ptr<concept_t const> self_;
};

template <DoubleGenerator Generator>
auto hit(any_scene_object<Generator> const &o, ray_t const &r,
         interval_t const &i) {
  return o.self_->hit_mem(r, i);
}

template <DoubleGenerator Generator>
bound_t get_bounds(any_scene_object<Generator> const &obj) {
  return obj.self_->get_bounds_mem();
}
} // namespace mrl
