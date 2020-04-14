#pragma once

#include <optional>

#include "raytracer/primitives/AABB.hpp"
#include "raytracer/shapes/RayHit.hpp"

/**
 * A node in BVH Tree
 */
class PrimitiveBase {
public:
  virtual ~PrimitiveBase() = default;

  virtual std::optional<RayHit> Hit(const Ray &ray) const = 0;
};
