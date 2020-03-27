#include "RayTracer/Triangle.hpp"

Triangle::Triangle(const glm::vec3 &v0, const glm::vec3 &v1,
                   const glm::vec3 &v2)
    : v0(v0), e1(v1 - v0), e2(v2 - v0),
      normal(glm::normalize(glm::cross(e1, e2))), aabb{min(v0, min(v1, v2)),
                                                       max(v0, max(v1, v2))} {}

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
std::optional<RayHit> Triangle::Intersect(const Ray &ray) const {
  const auto d = ray.direction, o = ray.origin;
  const auto h = cross(d, e2);
  const auto a = dot(e1, h);
  if (a > -FLT_EPSILON && a < FLT_EPSILON) {
    return std::nullopt; // This ray is parallel to this triangle.
  }
  const auto f = 1 / a;
  const auto s = o - v0;
  const auto u = f * dot(s, h);
  if (u < 0 || u > 1) {
    return std::nullopt;
  }
  const auto q = cross(s, e1);
  const auto v = f * dot(d, q);
  if (v < 0 || u + v > 1) {
    return std::nullopt;
  }
  const auto t = f * dot(e2, q);
  if (t > FLT_EPSILON) {
    return RayHit(t, normal);
  } else {
    return std::nullopt;
  }
}
