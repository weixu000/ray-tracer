#include "raytracer/shapes/Sphere.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;

Sphere::Sphere(const glm::mat4 &transform, const glm::vec3 &position,
               float radius)
    : LocalShape(scale(translate(transform, position), vec3(radius))) {}

std::optional<LocalInfo> Sphere::HitLocal(const Ray &ray) const {
  const auto d = ray.direction, o = ray.origin;
  const auto d_o = dot(d, o);
  const auto d2 = length2(d);
  const auto discriminant = d_o * d_o - (length2(o) - 1) * d2;
  if (discriminant < 0) {
    return std::nullopt;
  }
  const auto sqrt_disc = sqrt(discriminant);
  if (const auto t1 = (-d_o - sqrt_disc) / d2; t1 > 0) {
    return LocalInfo{t1, ray(t1)};
  } else if (const auto t2 = (-d_o + sqrt_disc) / d2; t2 > 0) {
    return LocalInfo{t2, ray(t2)};
  } else {
    return std::nullopt;
  }
}

AABB Sphere::GetAABB() const {
  // https://tavianator.com/exact-bounding-boxes-for-spheres-ellipsoids/
  const auto M3x3 = mat3(world_);
  const auto M3x3_2 = matrixCompMult(M3x3, M3x3);
  const auto d = sqrt(M3x3_2[0] + M3x3_2[1] + M3x3_2[2]);
  const auto v = vec3(world_[3]);
  return AABB{v - d, v + d};
}
