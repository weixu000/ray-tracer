#include "RayTracer/Scene.hpp"

#include <cstdint>

#include "RayTracer/Material.hpp"
#include "RayTracer/Ray.hpp"

std::optional<RayHit> Scene::Trace(const Ray &ray, int depth) const {
  std::optional<RayHit> nearest;
  for (const auto &geo : geometries) {
    if (auto hit = geo->Intersect(ray)) {
      if (!nearest || hit->t + FLT_EPSILON < nearest->t) {
        nearest = hit;
      }
    }
  }
  return nearest;
}

glm::vec3 Scene::Shade(const Ray &ray, int depth) const {
  using namespace std;

  auto hit = Trace(ray, depth);
  if (hit) {
    const auto &mat = *hit->material;
    auto color = mat.ambient + mat.emission;
    for (const auto &light : lights) {
      if (auto light_cast = light->GenerateLightRay(ray(hit->t), *this)) {
        const auto V = -normalize(ray.direction), N = normalize(hit->normal),
                   L = -normalize(light_cast->direction);
        const auto H = normalize(V + L);
        color += light_cast->attenuated_intensity *
                 (mat.diffuse * max(0.f, dot(N, L)) +
                  mat.specular * pow(max(0.f, dot(N, H)), mat.shininess));
      }
    }
    return color;
  } else {
    return glm::vec3(0.f);
  }
}
