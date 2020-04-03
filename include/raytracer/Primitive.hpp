#pragma once
#include <memory>

#include <glm/glm.hpp>

#include "raytracer/Material.hpp"
#include "raytracer/shapes/Shape.hpp"

class Primitive {
public:
  explicit Primitive(std::unique_ptr<Shape> geo, const Material &mat);

  std::optional<RayHit> Hit(const Ray &ray) const;

  AABB GetAABB() const { return shape->GetAABB(); }

private:
  std::unique_ptr<Shape> shape;
  Material material;
};
