#pragma once
#include <memory>

#include <glm/glm.hpp>

#include "raytracer/Material.hpp"
#include "raytracer/shapes/Shape.hpp"

struct Primitive {
  explicit Primitive(std::unique_ptr<Shape> geo, const Material &mat);

  std::unique_ptr<Shape> shape;
  Material material;

  std::optional<RayHit> Hit(const Ray &ray) const;
};
