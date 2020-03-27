#include "RayTracer/Geometry.hpp"

using namespace glm;

Geometry::Geometry(const glm::mat4 &transform)
    : world_(transform), local_(inverse(transform)),
      normal_world_(inverse(transpose(mat3(transform)))) {}

Ray Geometry::WorldToLocal(const Ray &ray) const {
  return {local_ * vec4(ray.origin, 1.f), local_ * vec4(ray.direction, 0.f)};
}

glm::vec4 Geometry::LocalToWorld(const glm::vec4 &v) const {
  return world_ * v;
}

glm::vec3 Geometry::NormalToWorld(const glm::vec3 &n) const {
  return normal_world_ * n;
}
