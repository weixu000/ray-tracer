#include "RayTracer/Mesh.h"

Mesh::Mesh(const glm::mat4 &transform, const Material &material,
           const std::vector<glm::vec3> &verts,
           const std::vector<glm::ivec3> &tris)
    : Geometry(transform, material) {
  triangles_.reserve(tris.size());
  for (const auto &f : tris) {
    const auto e1 = verts[f[1]] - verts[f[0]], e2 = verts[f[2]] - verts[f[0]];
    triangles_.push_back(
        {verts[f[0]], e1, e2, glm::normalize(glm::cross(e1, e2))});
  }
}

static std::optional<RayHit> Moller(const Triangle &f, const Ray &ray) {
  using namespace glm;

  const auto &[v0, e1, e2, n] = f;
  const auto d = ray.direction, o = ray.origin;
  const auto sol = inverse(mat3(e1, e2, -d)) * (o - v0);
  const auto u = sol[0], v = sol[1], t = sol[2];
  if (t >= 0 && u >= 0 && v >= 0 && u + v <= 1) {
    return RayHit{t};
  } else {
    return std::nullopt;
  }
}

std::optional<RayHit> Mesh::IntersectLocal(const Ray &ray) const {
  std::optional<RayHit> nearest;
  for (const auto &f : triangles_) {
    if (auto hit = Moller(f, ray)) {
      if (!nearest || hit->t <= nearest->t) {
        nearest = hit;
      }
    }
  }
  return nearest;
}
