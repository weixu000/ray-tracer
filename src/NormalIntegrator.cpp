#include "raytracer/integrators/NormalIntegrator.hpp"

#include "raytracer/Sampler.hpp"

std::vector<glm::u8vec3> NormalIntegrator::Render() const {
  using namespace glm;

  const auto w = camera_.width_;
  const auto h = camera_.height_;

  Sampler sampler(w, h);

  std::vector<u8vec3> output(w * h);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      const auto ray = camera_.GenerateEyeRay(sampler.Sample(i, j));
      auto color = vec3(0.f);
      if (const auto hit = scene_.Trace(ray)) {
        color = (hit->normal + 1.f) / 2.f;
      }
      output[i * w + j] = u8vec3(min(color, vec3(1.f)) * 255.0f);
    }
  }
  return output;
}
