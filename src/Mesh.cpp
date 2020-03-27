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
    triangles_.emplace_back(v0, v1, v2);
    boxes.push_back(triangles_.back().GetWorldAABB());
    aabb.Union(boxes.back());
  }

  bvh = std::move(BVH(boxes));
}

std::optional<RayHit> Mesh::Intersect(const Ray &ray) const {
  const auto hit = bvh.Traverse(
      ray, [&](const size_t f) { return triangles_[f].Intersect(ray); });
  if (hit) {
    return hit->first;
  } else {
    return std::nullopt;
  }
}
