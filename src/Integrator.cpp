#include "raytracer/integrators/Integrator.hpp"

#include "raytracer/samplers/CenterSampler.hpp"

Image Integrator::Render() const {
  using namespace glm;

  const auto w = camera_.width_;
  const auto h = camera_.height_;

  CenterSampler sampler;

  Image output(w, h);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      sampler.Reset();
      const auto pixel = glm::vec2(i, j) + sampler.Sample();
      const auto ray = camera_.GenerateEyeRay(pixel);
      const auto color = Shade(ray);
      assert(glm::all(glm::greaterThanEqual(color, glm::vec3(0.f))));
      output.At(i, j) = u8vec3(min(color, vec3(1.f)) * 255.0f);
    }
  }
  return output;
}
