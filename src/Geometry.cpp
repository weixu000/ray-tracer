#include "RayTracer/Geometry.h"

Geometry::Geometry(const glm::mat4 &transform, const Material &material)
    : world_(transform), local_(glm::inverse(transform)), material_(material) {}

const Material &Geometry::GetMaterial() const { return material_; }

Ray Geometry::WorldToLocal(const Ray &ray) const {
  return {local_ * glm::vec4(ray.origin, 1.f),
          local_ * glm::vec4(ray.direction, 0.f)};
}

glm::vec4 Geometry::LocalToWorld(const glm::vec4 &v) const {
  return world_ * v;
}
