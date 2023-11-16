#include "aspect_ratio.hpp"
#include "camera.hpp"
#include "camera_orientation.hpp"
#include "color.hpp"
#include "dimension.hpp"
#include "direction.hpp"
#include "image/in_memory_image.hpp"
#include "image/ppm/ppm_utils.hpp"
#include "point.hpp"
#include "ray.hpp"
#include "scene.hpp"
#include "scene_viewer.hpp"
#include "tracer.hpp"
#include "vector.hpp"
#include "viewport.hpp"
#include "viewport_utils.hpp"
#include <iostream>

constexpr auto make_scene_viewer(mrl::dimension_t<int> scene_dimensions,
                                 double focal_length, double viewport_height,
                                 mrl::camera_orientation_t orientation) {
  double viewport_width = mrl::viewport_width(
      {scene_dimensions.width, scene_dimensions.height}, viewport_height);
  mrl::camera_t camera{
      .viewport =
          {
              .width = viewport_width,
              .height = viewport_height,
          },
      .focal_length = focal_length,
  };
  return mrl::scene_viewer_t{
      .camera = camera,
      .camera_orientation = orientation,
  };
}

int main() {
  mrl::aspect_ratio_t ratio{16, 9};
  int scene_width = 400;
  int scene_height = mrl::scene_height(ratio, scene_width);
  auto scene_dimensions = mrl::dimension_t<int>{scene_width, scene_height};
  auto viewport_height = 2.0;
  auto viewport_width = mrl::viewport_width(scene_dimensions, viewport_height);
  mrl::camera_t camera{
      .viewport = {.width = viewport_width, .height = viewport_height},
      .focal_length = 1.0,
  };

  mrl::camera_orientation_t camera_orientation{
      .position = {0, 0, 0},
      .direction = mrl::vec3{0, 0, 1},
  };
  mrl::in_memory_image img{scene_width, scene_height};
  mrl::trace(camera, camera_orientation, scene_dimensions, img);
  mrl::write_ppm_img(std::cout, img);
}
