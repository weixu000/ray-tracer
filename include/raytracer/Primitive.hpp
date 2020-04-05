#pragma once
#include <memory>

#include <glm/glm.hpp>

#include "raytracer/Material.hpp"
#include "raytracer/shapes/Shape.hpp"

/**
 * Bundle shape and material to be stored in PrimitiveGroup
 *
 * TODO: really need this class?
 */
class Primitive {
public:
  Primitive(std::unique_ptr<Shape> geo, const Material &mat)
      : shape(std::move(geo)), material(mat) {}

  std::optional<RayHit> Hit(const Ray &ray) const {
    auto hit = shape->Hit(ray);
    if (const auto hit = shape->Hit(ray)) {
      return RayHit{hit->t, hit->normal, &material, material.GetBRDF(*hit)};
    } else {
      return std::nullopt;
    }
  }

  AABB GetAABB() const { return shape->GetAABB(); }

private:
  std::unique_ptr<Shape> shape;
  Material material;
};
