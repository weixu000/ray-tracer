#include "RayTracer/Mesh.hpp"

#include <cstdint>

using namespace glm;

Mesh::Mesh(const glm::mat4 &transform, const std::vector<glm::vec3> &verts,
           const std::vector<glm::ivec3> &tris)
    : Geometry(transform) {
  std::vector<AABB> boxes;
  boxes.reserve(tris.size());
  triangles_.reserve(tris.size());
  for (const auto &f : tris) {
    const auto v0 = vec3(LocalToWorld(vec4(verts[f[0]], 1.f))),
               v1 = vec3(LocalToWorld(vec4(verts[f[1]], 1.f))),
               v2 = vec3(LocalToWorld(vec4(verts[f[2]], 1.f)));

    boxes.push_back(AABB{min(v0, min(v1, v2)), max(v0, max(v1, v2))});

    const auto e1 = v1 - v0, e2 = v2 - v0;
    triangles_.push_back({v0, e1, e2, normalize(cross(e1, e2))});
  }

  bvh = std::move(BVH(boxes));
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
    return RayHit(t, n);
  } else {
    return std::nullopt;
  }
}

std::optional<RayHit> Mesh::Intersect(const Ray &ray) const {
  const auto hit = bvh.Traverse(
      ray, [&](const size_t f) { return Moller(triangles_[f], ray); });
  if (hit) {
    return hit->first;
  } else {
    return std::nullopt;
  }
}
