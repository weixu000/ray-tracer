#include <raytracer/integrators/integrator.hpp>

Image Integrator::Render() const {
  using namespace glm;

  const auto w = camera_.width_;
  const auto h = camera_.height_;

  Image output(w, h);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      output.At(i, j) = u8vec3(min(ShadePixel({i, j}), vec3(1.f)) * 255.0f);
    }
  }
  return output;
}
