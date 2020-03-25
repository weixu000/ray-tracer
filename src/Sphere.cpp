#include "RayTracer/Sphere.h"

#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;

Sphere::Sphere(const glm::mat4 &transform, const Material &material,
               const glm::vec3 &position, float radius)
    : Geometry(scale(translate(transform, position), vec3(radius)), material) {}

std::optional<RayHit> Sphere::IntersectLocal(const Ray &ray) const {

  const auto d = ray.direction;
  const auto o = ray.origin;
  const auto d_o = dot(d, o);
  const auto d2 = length2(d);
  const auto discriminant = d_o * d_o - (length2(o) - 1) * d2;
  if (discriminant < FLT_EPSILON) {
    return std::nullopt;
  }
  const auto sqrt_disc = sqrt(discriminant);
  const auto t1 = (-d_o - sqrt_disc) / d2;
  if (t1 > FLT_EPSILON) {
    return RayHit{t1};
  }
  const auto t2 = (-d_o + sqrt_disc) / d2;
  if (t2 > FLT_EPSILON) {
    return RayHit{t2};
  } else {
    return std::nullopt;
  }
}
