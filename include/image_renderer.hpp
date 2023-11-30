#pragma once

#include "angle.hpp"
#include "camera/camera.hpp"
#include "camera/camera_orientation.hpp"
#include "camera/concepts.hpp"
#include "color.hpp"
#include "direction.hpp"
#include "image/concepts.hpp"
#include "interval.hpp"
#include "pixel_sampler/concepts.hpp"
#include "random/random_unit_disk.hpp"
#include "ray.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "vector.hpp"
#include <cmath>
#include <limits>

namespace mrl {

constexpr std::pair<vec3, vec3>
viewport_direction(camera_orientation_t const &o) {
  auto right = normalize(cross(o.direction.val(), o.up_dir.val()));
  return {right, -o.up_dir.val()};
}

constexpr vec3 viewport_topleft(vec3 right, vec3 down,
                                camera_orientation_t const &o, double f) {
  return o.position + o.direction.val() * f - right / 2 - down / 2;
}

template <SceneObject Object>
constexpr color_t ray_color(ray_t const &ray, Object const &world, int depth) {
  if (depth <= 0)
    return {0, 0, 0};
  constexpr static double closeness_limit = 0.001;
  auto hit_record =
      hit(world, ray,
          interval_t{closeness_limit, std::numeric_limits<double>::infinity()});
  if (hit_record) {
    auto scattering = hit_record->scattering;
    if (!scattering.has_value())
      return {0, 0, 0};
    auto scattered_ray = scattering->scattered_ray;
    auto attenuation = scattering->attenuated_color;
    return attenuation * ray_color(scattered_ray, world, depth - 1);
  }
  auto dir = ray.direction.val();
  auto a = (dir.y + 1.0) / 2;
  return (1.0 - a) * color_t{1.0, 1.0, 1.0} + a * color_t{0.5, 0.7, 1.0};
}

// Precondition:
//   - std::ranges::distance(rng) >= 1
//   - RayOriginGenerator generates center in defocus disk
template <SceneObject Object, std::ranges::input_range VectorRange,
          std::invocable RayOriginGenerator>
  requires RangeValueType<VectorRange, vec3> &&
           std::same_as<std::invoke_result_t<RayOriginGenerator>, vec3>
constexpr color_t sampled_ray_color(RayOriginGenerator &gen_center,
                                    VectorRange const &pixel_points,
                                    Object const &world, int depth) {
  auto ray_origin = std::invoke(gen_center);
  double num_ele = 0.0;
  color_t color{0, 0, 0};
  for (vec3 const &pixel_center : pixel_points) {
    ray_t r{
        .origin = ray_origin,
        .direction = pixel_center - ray_origin,
    };
    color += ray_color(r, world, depth);
    ++num_ele;
  }
  return color / num_ele;
}

template <Camera camera_t, SceneObject Object, RandomAccessImage Image,
          PixelSampler Sampler, std::invocable DiskGenerator>
  requires std::same_as<std::invoke_result_t<DiskGenerator>, vec3>
constexpr void
render_image(Object const &world, Image &img, camera_t const &camera,
             camera_orientation_t const &orientation, Sampler &sampler,
             DiskGenerator &gen_disk, int rendering_depth) {

  auto focus_dist = focus_distance(camera);
  auto h = std::tan(vertical_fov(camera).radians / 2);
  auto viewport_height = 2 * h * focus_dist;
  auto viewport_width =
      viewport_height * (static_cast<double>(width(img)) / height(img));
  auto [u_dir, v_dir] = viewport_direction(orientation);
  auto viewport_u = u_dir * viewport_width;
  auto viewport_v = v_dir * viewport_height;

  auto pixel_delta_u = viewport_u / width(img);
  auto pixel_delta_v = viewport_v / height(img);

  auto viewport_top_left =
      viewport_topleft(viewport_u, viewport_v, orientation, focus_dist);

  auto pixel00_loc = viewport_top_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  auto defocus_radius =
      focus_dist * std::tan(defocus_angle(camera).radians / 2);
  auto defocus_disk_u = u_dir * defocus_radius;
  auto defocus_disk_v = (-v_dir) * defocus_radius;

  auto ray_origin_generator = [defocus_disk_u, defocus_disk_v, &gen_disk,
                               orientation] {
    auto p = gen_disk();
    return orientation.position + defocus_disk_u * p.x + defocus_disk_v * p.y;
  };

  for (int j = 0; j < height(img); ++j) {
    for (int i = 0; i < width(img); ++i) {
      auto pixel_center = pixel00_loc + i * pixel_delta_u + j * pixel_delta_v;
      auto sampling_points = sampler.sample({
          .point = pixel_center,
          .pixel_delta_u = pixel_delta_u,
          .pixel_delta_v = pixel_delta_v,
      });
      color_t pixel_color =
          sampled_ray_color(ray_origin_generator, std::move(sampling_points),
                            world, rendering_depth);
      set_pixel_at(img, j, i, pixel_color);
    }
  }
}

template <Camera camera_t, PixelSampler Sampler,
          std::invocable UnitDiskGenerator = random_unit_disk_generator>
  requires std::same_as<std::invoke_result_t<UnitDiskGenerator>, vec3>
struct img_renderer_t {
  camera_t camera;
  camera_orientation_t camera_orientation;
  int rendering_depth;
  Sampler sampler;
  UnitDiskGenerator gen_disk;

  img_renderer_t(camera_t camera_, camera_orientation_t camera_orientation_,
                 int rendering_depth_, Sampler sampler_,
                 UnitDiskGenerator gen_disk_ = random_unit_disk_generator())
      : camera{std::move(camera_)},
        camera_orientation{std::move(camera_orientation_)},
        rendering_depth(rendering_depth_), sampler{std::move(sampler_)},
        gen_disk(std::move(gen_disk_)) {}

  template <SceneObject Object, RandomAccessImage Image>
  constexpr void render(Object const &world, Image &img) {
    render_image(world, img, camera, camera_orientation, sampler, gen_disk,
                 rendering_depth);
  }
};

template <Camera camera_t, PixelSampler Sampler,
          std::invocable UnitDiskGenerator>
  requires std::same_as<std::invoke_result_t<UnitDiskGenerator>, vec3>
img_renderer_t(camera_t, camera_orientation_t, Sampler, UnitDiskGenerator)
    -> img_renderer_t<camera_t, Sampler, UnitDiskGenerator>;
} // namespace mrl
