#include "RayTracer/shapes/Mesh.hpp"

#include <cstdint>

using namespace glm;

Mesh::Mesh(const glm::mat4 &transform, const std::vector<glm::vec3> &verts,
           const std::vector<glm::ivec3> &tris) {
  std::vector<AABB> boxes;
  boxes.reserve(tris.size());
  triangles_.reserve(tris.size());
  for (const auto &f : tris) {
    triangles_.emplace_back(transform, verts[f[0]], verts[f[1]], verts[f[2]]);
    boxes.push_back(triangles_.back().GetWorldAABB());
    aabb_.Union(boxes.back());
  }

  bvh_ = std::move(BVH(boxes));
}

std::optional<RayHit> Mesh::Hit(const Ray &ray) const {
  const auto hit = bvh_.Traverse(
      ray, [&](const size_t f) { return triangles_[f].Hit(ray); });
  if (hit) {
    return hit->first;
  } else {
    return std::nullopt;
  }
}
