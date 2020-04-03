#include "raytracer/shapes/LocalShape.hpp"

std::optional<RayHit> LocalShape::Hit(const Ray &ray) const {
  const auto local_ray = Ray{local_ * glm::vec4(ray.origin, 1.f),
                             local_ * glm::vec4(ray.direction, 0.f)};
  if (const auto local = HitLocal(local_ray)) {
    return RayHit(local->t, normal_world_ * local->normal);
  } else {
    return std::nullopt;
  }
}

LocalShape::LocalShape(const glm::mat4 &transform)
    : world_(transform), local_(inverse(transform)),
      normal_world_(glm::inverse(glm::transpose(glm::mat3(transform)))) {}
