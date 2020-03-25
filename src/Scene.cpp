#include "RayTracer/Scene.h"

#include "RayTracer/Ray.h"

glm::vec3 Scene::Trace(const Ray &ray, int depth) const {
  std::optional<RayHit> nearest;
  auto color = glm::vec3(0.f);
  for (const auto &geo : geometries) {
    if (auto hit = geo->Intersect(ray)) {
      if (!nearest || hit->t <= nearest->t) {
        nearest = hit;
        color = geo->GetMaterial().ambient; // TODO: shading
      }
    }
  }
  return color;
}
