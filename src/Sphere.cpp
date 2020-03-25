#include "RayTracer/Sphere.hpp"

#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

using namespace glm;

Sphere::Sphere(const glm::mat4 &transform, const Material &material,
               const glm::vec3 &position, float radius)
    : Geometry(scale(translate(transform, position), vec3(radius)), material) {}

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
    const auto normal = vec3(LocalToWorld(vec4(local_ray(t1), 0.0f)));
    return RayHit{t1, &material_, normal};
  }
  if (const auto t2 = (-d_o + sqrt_disc) / d2; t2 > FLT_EPSILON) {
    const auto normal = vec3(LocalToWorld(vec4(local_ray(t2), 0.0f)));
    return RayHit{t2, &material_, normal};
  } else {
    return std::nullopt;
  }
}
