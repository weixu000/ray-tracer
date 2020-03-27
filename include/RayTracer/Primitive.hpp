#pragma once
#include <memory>

#include <glm/glm.hpp>

#include "RayTracer/Material.hpp"
#include "RayTracer/geometries/Geometry.hpp"

struct Primitive {
  explicit Primitive(std::unique_ptr<Geometry> geo, const Material &mat);

  std::unique_ptr<Geometry> geometry;
  Material material;

  std::optional<RayHit> Intersect(const Ray &ray) const;
};
