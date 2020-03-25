#include "RayTracer/Mesh.h"

#include <cstdint>

using namespace glm;

Mesh::Mesh(const glm::mat4 &transform, const Material &material,
           const std::vector<glm::vec3> &verts,
           const std::vector<glm::ivec3> &tris)
    : Geometry(transform, material) {
  triangles_.reserve(tris.size());
  for (const auto &f : tris) {
    const auto v0 = vec3(LocalToWorld(vec4(verts[f[0]], 1.f))),
               v1 = vec3(LocalToWorld(vec4(verts[f[1]], 1.f))),
               v2 = vec3(LocalToWorld(vec4(verts[f[2]], 1.f)));
    const auto e1 = v1 - v0, e2 = v2 - v0;
    triangles_.push_back({v0, e1, e2, normalize(cross(e1, e2))});
  }
}

/*
 * https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
 */
static std::optional<RayHit> Moller(const Triangle &triangle, const Ray &ray) {
  const auto &[v0, e1, e2, n] = triangle;
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
    return RayHit{t};
  } else {
    return std::nullopt;
  }
}

std::optional<RayHit> Mesh::Intersect(const Ray &ray) const {
  std::optional<RayHit> nearest;
  for (const auto &f : triangles_) {
    if (auto hit = Moller(f, ray)) {
      if (!nearest || hit->t + FLT_EPSILON < nearest->t) {
        nearest = hit;
      }
    }
  }
  return nearest;
}
