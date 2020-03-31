#include "raytracer/integrators/AnalyticDirectIntegrator.hpp"

#include <glm/gtc/constants.hpp>

#include "raytracer/Sampler.hpp"
#include "raytracer/lights/QuadLight.hpp"

std::vector<glm::u8vec3> AnalyticDirectIntegrator::Render() const {
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

glm::vec3 AnalyticDirectIntegrator::Shade(const Ray &ray) const {
  using namespace glm;

  for (const auto &light : scene_.lights) {
    if (const auto quad = dynamic_cast<QuadLight *>(light.get())) {
      if (const auto hit = quad->Intersect(ray)) {
        return quad->color_;
      }
    }
  }

  auto hit = scene_.Trace(ray);
  if (hit) {
    const auto &mat = *hit->material;
    const auto incident = ray(hit->t), N = normalize(hit->normal);
    auto color = glm::vec3(0.f);
    for (const auto &light : scene_.lights) {
      if (const auto quad = dynamic_cast<QuadLight *>(light.get())) {
        color += mat.diffuse / pi<float>() * quad->color_ *
                 max(0.f, dot(quad->GetIrradianceVector(incident), N));
      }
    }
    return color;
  } else {
    return glm::vec3(0.f);
  }
}
