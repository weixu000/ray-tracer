#pragma once
#include <memory>

#include <glm/glm.hpp>

#include "RayTracer/Material.hpp"
#include "RayTracer/shapes/Shape.hpp"

struct Primitive {
  explicit Primitive(std::unique_ptr<Shape> geo, const Material &mat);

  std::unique_ptr<Shape> shape;
  Material material;

  std::optional<RayHit> Intersect(const Ray &ray) const;
};
