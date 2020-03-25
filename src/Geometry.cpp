#include "RayTracer/Geometry.h"

Geometry::Geometry(const glm::mat4 &transform, const Material &material)
    : world_to_local_(glm::inverse(transform)), material_(material) {}

std::optional<RayHit> Geometry::Intersect(const Ray &ray) const {
  auto local_ray = Ray();
  local_ray.origin = world_to_local_ * glm::vec4(ray.origin, 1.f);
  local_ray.direction = world_to_local_ * glm::vec4(ray.direction, 0.f);
  return IntersectLocal(local_ray);
}

const Material &Geometry::GetMaterial() const { return material_; }
