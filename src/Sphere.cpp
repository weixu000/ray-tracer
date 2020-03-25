#include "RayTracer/Sphere.h"

#include <glm/gtx/norm.hpp>

Sphere::Sphere(const glm::mat4 &transform, const Material &material,
               const glm::vec3 &position, float radius)
    : Geometry(transform, material), position_(position), radius_(radius) {}

std::optional<RayHit> Sphere::IntersectLocal(const Ray &ray) const {
  using namespace glm;

  const auto d = ray.direction;
  const auto o = ray.origin - position_;
  const auto d_o = dot(d, o);
  const auto discriminant =
      d_o * d_o - (length2(o) - radius_ * radius_) * length2(d);
  if (discriminant < 0) {
    return std::nullopt;
  }
  const auto t1 = (-d_o - sqrt(discriminant)) / length2(d),
             t2 = (-d_o + sqrt(discriminant)) / length2(d);

  if (t1 > 0) {
    return RayHit{t1};
  } else if (t2 > 0) {
    return RayHit{t2};
  } else {
    return std::nullopt;
  }
}
