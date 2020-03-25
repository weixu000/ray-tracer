#include "RayTracer/Sampler.hpp"

#include <vector>

#include <glm/glm.hpp>

std::vector<glm::u8vec3> Sampler::Render(const Scene &scene) {
  const auto w = scene.width;
  const auto h = scene.height;

  std::vector<glm::u8vec3> output(w * h);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      auto ray = scene.camera.GenerateEyeRay(
          {(j + .5f - w / 2.f) / (h / 2.f), (-i - .5f + h / 2.f) / (h / 2.f)});
      output[i * w + j] =
          glm::u8vec3(scene.Trace(ray, scene.max_depth) * 255.0f);
    }
  }
  return output;
}
