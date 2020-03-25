#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "RayTracer/Geometry.h"

struct Triangle {
  glm::vec3 v, e1, e2, normal;
};

class Mesh : public Geometry {
public:
  Mesh() = default;

  Mesh(const glm::mat4 &transform, const Material &material,
       const std::vector<glm::vec3> &verts,
       const std::vector<glm::ivec3> &tris);

private:
  std::optional<RayHit> IntersectLocal(const Ray &ray) const override;

  std::vector<Triangle> triangles_;
};