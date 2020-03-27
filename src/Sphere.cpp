#include "RayTracer/Sphere.hpp"

#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;

Sphere::Sphere(const glm::mat4 &transform, const glm::vec3 &position,
               float radius)
    : LocalGeometry(scale(translate(transform, position), vec3(radius))) {}

std::optional<RayHit> Sphere::Intersect(const Ray &ray) const {
  const auto local_ray = WorldToLocal(ray);
  const auto d = local_ray.direction;
  const auto o = local_ray.origin;
  const auto d_o = dot(d, o);
  const auto d2 = length2(d);
  const auto discriminant = d_o * d_o - (length2(o) - 1) * d2;
  if (discriminant < FLT_EPSILON) {
    return std::nullopt;
  }
  const auto sqrt_disc = sqrt(discriminant);
  if (const auto t1 = (-d_o - sqrt_disc) / d2; t1 > FLT_EPSILON) {
    const auto normal = NormalToWorld(local_ray(t1));
    return RayHit(t1, normal);
  }
  if (const auto t2 = (-d_o + sqrt_disc) / d2; t2 > FLT_EPSILON) {
    const auto normal = NormalToWorld(local_ray(t2));
    return RayHit(t2, normal);
  } else {
    return std::nullopt;
  }
}

AABB Sphere::GetWorldAABB() const {
  // https://tavianator.com/exact-bounding-boxes-for-spheres-ellipsoids/
  const auto M3x3 = mat3(world_);
  const auto M3x3_2 = matrixCompMult(M3x3, M3x3);
  const auto d = sqrt(M3x3_2[0] + M3x3_2[1] + M3x3_2[2]);
  const auto v = vec3(world_[3]);
  return AABB{v - d, v + d};
}
