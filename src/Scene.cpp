#include "RayTracer/Scene.hpp"

#include <cstdint>

#include "RayTracer/Material.hpp"
#include "RayTracer/Ray.hpp"

std::optional<RayHit> Scene::Trace(const Ray &ray) const {
  std::optional<RayHit> nearest;
  for (const auto &geo : primitives) {
    if (auto hit = geo.Intersect(ray)) {
      if (!nearest || hit->t + FLT_EPSILON < nearest->t) {
        nearest = hit;
      }
    }
  }
  return nearest;
}

glm::vec3 Scene::Shade(const Ray &ray, int depth) const {
  using namespace std;

  if (depth <= 0) {
    return glm::vec3(0.f);
  }

  auto hit = Trace(ray);
  if (hit) {
    const auto &mat = *hit->material;
    const auto incident = ray(hit->t), V = -normalize(ray.direction),
               N = normalize(hit->normal);
    auto color = mat.ambient + mat.emission;
    for (const auto &light : lights) {
      if (auto light_cast = light->GenerateLightRay(incident, *this)) {
        const auto L = -normalize(light_cast->direction);
        const auto H = normalize(V + L);
        color += light_cast->attenuated_intensity *
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
