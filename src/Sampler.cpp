#include "RayTracer/Sampler.hpp"

#include <vector>

#include <glm/glm.hpp>

std::vector<glm::u8vec3> Sampler::Render(const Scene &scene) {
  using namespace glm;

  const auto w = scene.width;
  const auto h = scene.height;

  std::vector<u8vec3> output(w * h);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      const auto ray = scene.camera.GenerateEyeRay(
          {(j + .5f - w / 2.f) / (h / 2.f), (-i - .5f + h / 2.f) / (h / 2.f)});
      const auto color = scene.Shade(ray, scene.max_depth);
      output[i * w + j] = u8vec3(min(color, vec3(1.f)) * 255.0f);
    }
  }
  return output;
}
