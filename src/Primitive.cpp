#include "RayTracer/Primitive.hpp"

Primitive::Primitive(std::unique_ptr<Geometry> geo, const Material &mat)
    : geometry(std::move(geo)), material(mat) {}

std::optional<RayHit> Primitive::Intersect(const Ray &ray) const {
  auto hit = geometry->Intersect(ray);
  if (hit) {
    hit->material = &material;
  }
  return hit;
}
