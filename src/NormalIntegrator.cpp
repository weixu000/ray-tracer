#include "raytracer/integrators/NormalIntegrator.hpp"

glm::vec3 NormalIntegrator::Shade(const Ray &ray) const {
  using namespace glm;

  auto color = vec3(0.f);
  if (const auto hit = scene_.Trace(ray)) {
    color = (hit->normal + 1.f) / 2.f;
  }
  return color;
}
