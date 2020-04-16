#include <glm/gtc/constants.hpp>

#include <raytracer/integrators/analytic_direct_integrator.hpp>
#include <raytracer/lights/quad_light.hpp>

glm::vec3 AnalyticDirectIntegrator::Shade(const Ray &ray) const {
  using namespace glm;

  for (const auto &light : scene_.lights) {
    if (const auto quad = dynamic_cast<QuadLight *>(light.get())) {
      if (const auto hit = quad->Hit(ray)) {
        return hit->L_e;
      }
    }
  }

  if (const auto hit = scene_.Trace(ray)) {
    const auto &mat = *hit->material;
    const auto incident = ray(hit->t), N = normalize(hit->normal);
    auto color = glm::vec3(0.f);
    for (const auto &light : scene_.lights) {
      if (const auto quad = dynamic_cast<QuadLight *>(light.get())) {
        color += mat.diffuse / pi<float>() * quad->GetRadiance() *
                 max(0.f, dot(quad->GetIrradianceVector(incident), N));
      }
    }
    return color;
  } else {
    return glm::vec3(0.f);
  }
}
