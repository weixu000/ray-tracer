#include "raytracer/integrators/AnalyticDirectIntegrator.hpp"

#include <glm/gtc/constants.hpp>

#include "raytracer/lights/QuadLight.hpp"

glm::vec3 AnalyticDirectIntegrator::Shade(const Ray &ray) const {
  using namespace glm;

  for (const auto &light : scene_.lights) {
    if (const auto quad = dynamic_cast<QuadLight *>(light.get())) {
      if (const auto hit = quad->Hit(ray)) {
        return quad->intensity;
      }
    }
  }

  if (const auto hit = scene_.Trace(ray)) {
    const auto &mat = *hit->material;
    const auto incident = ray(hit->t), N = normalize(hit->normal);
    auto color = glm::vec3(0.f);
    for (const auto &light : scene_.lights) {
      if (const auto quad = dynamic_cast<QuadLight *>(light.get())) {
        color += mat.diffuse / pi<float>() * quad->intensity *
                 max(0.f, dot(quad->GetIrradianceVector(incident), N));
      }
    }
    return color;
  } else {
    return glm::vec3(0.f);
  }
}
