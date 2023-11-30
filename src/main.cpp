#include "angle.hpp"
#include "aspect_ratio.hpp"
#include "camera/camera.hpp"
#include "camera/camera_orientation.hpp"
#include "direction.hpp"
#include "hit_record.hpp"
#include "image/in_memory_image.hpp"
#include "image/ppm/ppm_utils.hpp"
#include "image_renderer.hpp"
#include "materials/concept.hpp"
#include "materials/dielectric.hpp"
#include "materials/lambertian.hpp"
#include "materials/metal.hpp"
#include "pixel_sampler/randomized_delta_sampler.hpp"
#include "scene_objects/any_scene_object.hpp"
#include "scene_objects/debug/debug_object.hpp"
#include "scene_objects/debug/hooks/noop_hook.hpp"
#include "scene_objects/debug/hooks/ref_hook.hpp"
#include "scene_objects/debug/hooks/tagged_stream_hook.hpp"
#include "scene_objects/scene_object_range.hpp"
#include "scene_objects/shapes/bubble.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "std/random_double_generator.hpp"
#include "std/ranges.hpp"
#include "vector.hpp"
#include <iostream>

struct count_hook {
  int zero = 0;
  int neg = 0;
  int pos = 0;
  int nil = 0;

  void invoke_hit(mrl::ray_t const &ray,
                  std::optional<mrl::hit_record_t> const &h) {
    if (h) {
      auto const n = (*h).normal.val();
      auto const d = ray.direction.val();
      auto const res = mrl::dot(d, n);
      if (res == 0)
        ++zero;
      else if (res < 0)
        ++neg;
      else
        ++pos;
    } else {
      ++nil;
    }
  }
};

void debug() {
  mrl::aspect_ratio_t ratio{16, 9};
  auto img_width = 600;
  auto img_height = mrl::image_height(ratio, img_width);
  mrl::camera_t camera{
      .focal_length = 1.0,
      .vertical_fov = mrl::degrees(90),
  };

  mrl::camera_orientation_t camera_orientation{
      .position = {0, 0, 0},
      .direction = mrl::direction_t{0, 0, -1},
      .up_dir = mrl::direction_t{0, 1, 0},
  };

  auto material = mrl::lambertian_t{mrl::color_t{0.5, 0.5, 0.5}};

  auto small_sphere = mrl::sphere_obj_t{0.5, {0, 0, -1}, material};
  auto big_sphere = mrl::sphere_obj_t{100, {0, -100.5, -1}, material};

  count_hook hook{};

  mrl::any_scene_object small =
      mrl::debug_obj_t{small_sphere, mrl::noop_hook{}};
  mrl::any_scene_object big = mrl::debug_obj_t{big_sphere, mrl::ref_hook{hook}};

  std::vector<mrl::any_scene_object> world{small, big};

  mrl::in_memory_image img{img_width, img_height};
  mrl::img_renderer_t renderer(camera, camera_orientation, 50,
                               mrl::randomized_delta_sampler{5});
  renderer.render(world, img);

  std::cerr << "nil : " << hook.nil << '\n'
            << "neg : " << hook.neg << '\n'
            << "zero : " << hook.zero << '\n'
            << "pos : " << hook.pos << '\n';
  mrl::write_ppm_img(std::cout, img);
}

void real() {
  mrl::aspect_ratio_t ratio{16, 9};
  auto img_width = 600;
  auto img_height = mrl::image_height(ratio, img_width);
  mrl::camera_t camera{
      .focal_length = 1.0,
      .vertical_fov = mrl::degrees(20),
  };

  mrl::camera_orientation_t camera_orientation{
      .position = {-2, 2, 1},
      .direction = mrl::point3(0, 0, -1) - mrl::point3(-2, 2, 1),
      .up_dir = mrl::calc_up_dir({0, 1, 0},
                                 mrl::point3(0, 0, -1) - mrl::point3(-2, 2, 1)),
  };

  auto sun_material = mrl::lambertian_t{mrl::color_t{1, 1, 0.0}};
  auto air = mrl::dielectric{1};
  auto ground_material = mrl::lambertian_t{mrl::color_t{0.3, 0.3, 0.3}};
  auto left_material = mrl::dielectric{2};
  auto right_material = mrl::fuzzy_metal_t{mrl::color_t{0.3, 0.3, 0.3}, 0.2};
  auto center_material = mrl::lambertian_t{mrl::color_t{0.6, 0.6, 0.3}};
  auto sun = mrl::sphere_obj_t{0.1, {1, 1, -1.3}, sun_material};
  auto center_sphere = mrl::sphere_obj_t{0.5, {0, 0, -1}, center_material};
  auto left_sphere = mrl::sphere_obj_t{0.5, {-1, 0, -1}, left_material};
  auto left_sphere_1 = mrl::sphere_obj_t{0.3, {-1, 0, -1}, air};
  auto right_sphere = mrl::sphere_obj_t{0.5, {1, 0, -1}, right_material};
  auto big_sphere = mrl::sphere_obj_t{100, {0, -100.5, -1}, ground_material};

  std::vector<mrl::any_scene_object> world{
      center_sphere, big_sphere, right_sphere, left_sphere, sun, left_sphere_1};

  mrl::in_memory_image img{img_width, img_height};
  mrl::img_renderer_t renderer(camera, camera_orientation, 50,
                               mrl::randomized_delta_sampler{100});
  renderer.render(world, img);
  mrl::write_ppm_img(std::cout, img);
}

int main() { real(); }
