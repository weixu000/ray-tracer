#include "raytracer/integrators/SimpleIntegrator.hpp"

#include <glm/glm.hpp>

glm::vec3 SimpleIntegrator::Shade(const Ray &ray, int depth) const {
  using namespace glm;

  if (depth <= 0) {
    return glm::vec3(0.f);
  }

  if (const auto hit = scene_.Trace(ray)) {
    const auto &mat = *hit->material;
    const auto incident = ray(hit->t), V = -normalize(ray.direction),
               N = normalize(hit->normal);
    auto color = mat.ambient + mat.emission;
    for (const auto &light : scene_.lights) {
      const auto light_sample = light->GenerateLightRay(incident, glm::vec2());
      const auto shadow_ray = light_sample.GetShadowRay();
      if (const auto shadow_hit = scene_.Trace(shadow_ray);
          !shadow_hit || shadow_hit->t > 1.f) {
        const auto L = normalize(shadow_ray.direction);
        const auto H = normalize(V + L);
        color += light_sample.radience *
                 (mat.diffuse * max(0.f, dot(N, L)) +
                  mat.specular * pow(max(0.f, dot(N, H)), mat.shininess));
      }
    }
    const auto R = reflect(-V, N);
    const auto reflect_ray = Ray{incident + R * SHADOW_EPSILON, R};
    color += mat.specular * Shade(reflect_ray, depth - 1);
    return color;
  } else {
    return glm::vec3(0.f);
  }
}
