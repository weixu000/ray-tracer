#include "RayTracer/geometries/LocalGeometry.hpp"

LocalGeometry::LocalGeometry(const glm::mat4 &transform)
    : world_(transform), local_(inverse(transform)),
      normal_world_(glm::inverse(glm::transpose(glm::mat3(transform)))) {}

Ray LocalGeometry::WorldToLocal(const Ray &ray) const {
  return {local_ * glm::vec4(ray.origin, 1.f),
          local_ * glm::vec4(ray.direction, 0.f)};
}

glm::vec3 LocalGeometry::NormalToWorld(const glm::vec3 &n) const {
  return normal_world_ * n;
}