#include "triangle.hpp"

using namespace glm;

Triangle::Triangle(const glm::mat4& transform, const glm::vec3& v0,
                   const glm::vec3& v1, const glm::vec3& v2,
                   const MaterialRef& mat)
    : Shape(mat) {
  const auto v0_w = vec3(transform * vec4(v0, 1.f)),
             v1_w = vec3(transform * vec4(v1, 1.f)),
             v2_w = vec3(transform * vec4(v2, 1.f));
  v0_ = v0_w;
  e1_ = v1_w - v0_w;
  e2_ = v2_w - v0_w;
  n_ = normalize(cross(e1_, e2_));
}

// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
std::optional<LocalInfo> Triangle::Hit(const Ray& ray) const {
  const auto d = ray.direction, o = ray.origin;
  const auto h = cross(d, e2_);
  const auto a = dot(e1_, h);
  if (a == 0) {
    return std::nullopt;  // This ray is parallel to this triangle.
  }
  const auto f = 1 / a;
  const auto s = o - v0_;
  const auto u = f * dot(s, h);
  if (u < 0 || u > 1) {
    return std::nullopt;
  }
  const auto q = cross(s, e1_);
  const auto v = f * dot(d, q);
  if (v < 0 || u + v > 1) {
    return std::nullopt;
  }
  const auto t = f * dot(e2_, q);
  if (t > 0) {
    return LocalInfo{t, n_};
  } else {
    return std::nullopt;
  }
}
