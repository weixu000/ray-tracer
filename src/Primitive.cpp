#include "RayTracer/Primitive.hpp"

Primitive::Primitive(std::unique_ptr<Shape> geo, const Material &mat)
    : shape(std::move(geo)), material(mat) {}

std::optional<RayHit> Primitive::Intersect(const Ray &ray) const {
  auto hit = shape->Intersect(ray);
  if (hit) {
    hit->material = &material;
  }
  return hit;
}
