#include "raytracer/integrators/Integrator.hpp"

#include "raytracer/samplers/CenterSampler.hpp"

Integrator::Integrator(const Scene &scene, const Camera &camera)
    : scene_(scene), camera_(camera) {}

std::vector<glm::u8vec3> Integrator::Render() const {
  using namespace glm;

  const auto w = camera_.width_;
  const auto h = camera_.height_;

  CenterSampler sampler;

  std::vector<u8vec3> output(w * h);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      sampler.Reset();
      const auto pixel = glm::vec2(i, j) + sampler.Sample();
      const auto ray = camera_.GenerateEyeRay(pixel);
      const auto color = Shade(ray);
      output[i * w + j] = u8vec3(min(color, vec3(1.f)) * 255.0f);
    }
  }
  return output;
}
