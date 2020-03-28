#include "raytracer/Scene.hpp"

#include <cstdint>

#include "raytracer/Material.hpp"
#include "raytracer/Ray.hpp"

std::optional<RayHit> Scene::Trace(const Ray &ray) const {
  std::optional<RayHit> nearest;
  for (const auto &geo : primitives) {
    if (auto hit = geo.Hit(ray)) {
      if (!nearest || hit->t + FLT_EPSILON < nearest->t) {
        nearest = hit;
      }
    }
  }
  return nearest;
}
