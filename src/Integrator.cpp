#include "raytracer/integrators/Integrator.hpp"

#include "raytracer/Sampler.hpp"

Integrator::Integrator(const Scene &scene, const Camera &camera)
    : scene_(scene), camera_(camera) {}

std::vector<glm::u8vec3> Integrator::Render() const {
  using namespace glm;

  const auto w = camera_.width_;
  const auto h = camera_.height_;

  Sampler sampler(w, h);

  std::vector<u8vec3> output(w * h);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      const auto ray = camera_.GenerateEyeRay(sampler.Sample(i, j));
      const auto color = Shade(ray);
      output[i * w + j] = u8vec3(min(color, vec3(1.f)) * 255.0f);
    }
  }
  return output;
}
