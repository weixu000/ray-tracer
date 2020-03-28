#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "RayTracer/BVH.hpp"
#include "RayTracer/shapes/Shape.hpp"

#include "Triangle.hpp"

class Mesh : public Shape {
public:
  Mesh() = default;

  Mesh(const glm::mat4 &transform, const std::vector<glm::vec3> &verts,
       const std::vector<glm::ivec3> &tris);

  std::optional<RayHit> Hit(const Ray &ray) const override;

  AABB GetWorldAABB() const override { return aabb_; }

private:
  std::vector<Triangle> triangles_;

  AABB aabb_;

  BVH bvh_;
};