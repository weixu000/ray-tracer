#include "raytracer/Primitive.hpp"

Primitive::Primitive(std::unique_ptr<Shape> geo, const Material &mat)
    : shape(std::move(geo)), material(mat) {}

std::optional<RayHit> Primitive::Hit(const Ray &ray) const {
  auto hit = shape->Hit(ray);
  if (const auto hit = shape->Hit(ray)) {
    return RayHit{hit->t, hit->normal, &material, material.GetBRDF(*hit)};
  } else {
    return std::nullopt;
  }
}
