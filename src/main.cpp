#include "aspect_ratio.hpp"
#include "camera.hpp"
#include "camera_orientation.hpp"
#include "image/in_memory_image.hpp"
#include "image/ppm/ppm_utils.hpp"
#include "scene.hpp"
#include "scene_objects/scene_object_list.hpp"
#include "scene_objects/shapes/sphere.hpp"
#include "tracer.hpp"
#include "vector.hpp"
#include "viewport.hpp"
#include <iostream>

int main() {
  mrl::aspect_ratio_t ratio{16, 9};
  auto scene = mrl::make_scene(ratio, 600);
  auto viewport = mrl::make_viewport(scene, 2.0);
  mrl::camera_t camera{
      .viewport = viewport,
      .focal_length = 1.0,
  };

  mrl::camera_orientation_t camera_orientation{
      .position = {0, 0, 0},
      .direction = mrl::vec3{0, 0, -1},
  };

  mrl::scene_object_list<mrl::sphere_obj_t> world{
      mrl::sphere_obj_t{.sphere{.radius = 0.5}, .center{0, 0, -1}},
      mrl::sphere_obj_t{.sphere{.radius = 100}, .center{0, -100.5, -1}},
  };

  mrl::in_memory_image img{scene.width, scene.height};
  mrl::trace(world, img, camera, camera_orientation);
  mrl::write_ppm_img(std::cout, img);
}
