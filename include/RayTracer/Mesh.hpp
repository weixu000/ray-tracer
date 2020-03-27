#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "RayTracer/BVH.hpp"
#include "RayTracer/Geometry.hpp"

struct Triangle {
  glm::vec3 v, e1, e2, normal;
};

class Mesh : public Geometry {
public:
  Mesh() = default;

  Mesh(const glm::mat4 &transform, const std::vector<glm::vec3> &verts,
       const std::vector<glm::ivec3> &tris);

  std::optional<RayHit> Intersect(const Ray &ray) const override;

private:
  std::vector<Triangle> triangles_;

  BVH bvh;
};