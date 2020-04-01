#include "raytracer/integrators/SimpleIntegrator.hpp"

#include <glm/glm.hpp>

glm::vec3 SimpleIntegrator::Shade(const Ray &ray, int depth) const {
  using namespace glm;

  if (depth <= 0) {
    return glm::vec3(0.f);
  }

  auto hit = scene_.Trace(ray);
  if (hit) {
    const auto &mat = *hit->material;
    const auto incident = ray(hit->t), V = -normalize(ray.direction),
               N = normalize(hit->normal);
    auto color = mat.ambient + mat.emission;
    for (const auto &light : scene_.lights) {
      const auto light_ray = light->GenerateLightRay(incident);
      if (const auto shadow_hit = scene_.Trace(light_ray);
          !shadow_hit || shadow_hit->t > light_ray.visible_t) {
        const auto L = normalize(light_ray.direction);
        const auto H = normalize(V + L);
        color += light_ray.radience *
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
