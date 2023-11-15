#include <iostream>

int main() {
  int width = 256;
  int height = 256;

  std::cout << "P3\n" << height << ' ' << width << '\n' << 255 << '\n';

  for (int i = 0; i < width; ++i) {
    for (int j = 0; j < height; ++j) {
      auto r = double(i) / (width - 1);
      auto g = double(j) / (height - 1);
      auto b = 10;
      int ir = static_cast<int>(255.999 * r);
      int ig = static_cast<int>(255.999 * g);
      int ib = static_cast<int>(255.999 * b);
      std::cout << ir << ' ' << ig << ' ' << ib << '\n';
    }
  }
}
