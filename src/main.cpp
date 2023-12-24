#include "angle.hpp"
#include "aspect_ratio.hpp"
#include "camera/camera.hpp"
#include "camera/camera_orientation.hpp"
#include "color.hpp"
#include "direction.hpp"
#include "hit_record.hpp"
#include "image/in_memory_image.hpp"
#include "image/ppm/ppm_utils.hpp"
#include "image_renderer.hpp"
#include "materials/concept.hpp"
#include "materials/dielectric.hpp"
#include "materials/lambertian.hpp"
#include "materials/metal.hpp"
#include "pixel_sampler/delta_sampler.hpp"
#include "point.hpp"
#include "scene_objects/any_scene_object.hpp"
#include "scene_objects/concepts.hpp"
#include "scene_objects/scene_object_range.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "schedulers/concepts.hpp"
#include "schedulers/inline_scheduler.hpp"
#include "schedulers/static_thread_pool_scheduler.hpp"
#include "schedulers/type_traits.hpp"
#include "std/ranges.hpp"
#include "vector.hpp"
#include <iostream>
#include <stdexec/execution.hpp>

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

void real() {
  using any_object = mrl::any_object_t<mrl::inline_scheduler>;
  mrl::inline_scheduler sch;
  mrl::aspect_ratio_t ratio{16, 9};
  auto img_width = 600;
  auto img_height = mrl::image_height(ratio, img_width);
  mrl::camera_t camera{
      .focus_distance = 3.4,
      .vertical_fov = mrl::degrees(20),
      .defocus_angle = mrl::degrees(10.0),
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

  std::vector<any_object> world{center_sphere, big_sphere, right_sphere,
                                left_sphere,   sun,        left_sphere_1};

  mrl::in_memory_image img{img_width, img_height};

  mrl::img_renderer_t renderer(camera, camera_orientation, sch);
  stdexec::sync_wait(renderer.render(world, img));
  mrl::write_ppm_img(std::cout, img);
}

void real_img() {
  auto const num_threads = std::thread::hardware_concurrency();
  auto thread_pool = mrl::static_thread_pool{num_threads};
  using any_object = mrl::any_object_t<mrl::thread_pool_scheduler>;
  mrl::thread_pool_scheduler sch = thread_pool.get_scheduler();
  auto rand = random_generator(sch);

  mrl::aspect_ratio_t ratio{16, 9};
  auto img_width = 1000;
  auto img_height = mrl::image_height(ratio, img_width);
  mrl::in_memory_image img{img_width, img_height};
  mrl::camera_t camera{
      .focus_distance = 10.0,
      .vertical_fov = mrl::degrees(20),
      .defocus_angle = mrl::degrees(0.0),
  };

  mrl::camera_orientation_t camera_orientation{
      .position = {13, 2, 3},
      .direction = mrl::point3(0, 0, 0) - mrl::point3(13, 2, 3),
      .up_dir = mrl::direction_t{0, 1, 0},
  };

  std::vector<any_object> world;
  mrl::dielectric mat1(1.5);
  mrl::lambertian_t mat2(mrl::color_t{0.4, 0.2, 0.1});
  mrl::metal_t mat3(mrl::color_t{0.7, 0.6, 0.5});

  world.push_back(mrl::sphere_obj_t{1.0, mrl::point3{0, 1, 0}, mat1});
  world.push_back(mrl::sphere_obj_t{1.0, mrl::point3{-4, 1, 0}, mat2});
  world.push_back(mrl::sphere_obj_t{1.0, mrl::point3{4, 1, 0}, mat3});

  for (int a = -11; a < 11; ++a) {
    for (int b = -11; b < 11; ++b) {
      auto choose_mat = rand(0.0, 1.0);
      auto center =
          mrl::point3{a + 0.9 * rand(0.0, 1.0), 0.2, b * 0.9 * rand(0.0, 1.0)};
      if (choose_mat < 0.8) {
        auto color =
            mrl::color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)} *
            mrl::color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)};
        world.push_back(
            mrl::sphere_obj_t{0.2, center, mrl::lambertian_t{color}});
      } else if (choose_mat < 0.95) {
        auto color =
            mrl::color_t{rand(0.0, 1.0), rand(0.0, 1.0), rand(0.0, 1.0)};
        auto fuzz = rand(0.5, 1.0);
        world.push_back(
            mrl::sphere_obj_t{0.2, center, mrl::fuzzy_metal_t{color, fuzz}});

      } else {
        world.push_back(mrl::sphere_obj_t{0.2, center, mrl::dielectric{1.5}});
      }
    }
  }

  mrl::img_renderer_t renderer(camera, camera_orientation, sch);
  stdexec::sync_wait(renderer.render(world, img));
  mrl::write_ppm_img(std::cout, img);
}

int main() { real_img(); }
