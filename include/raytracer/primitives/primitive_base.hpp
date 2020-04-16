#pragma once

#include <optional>

#include <raytracer/primitives/aabb.hpp>
#include <raytracer/shapes/ray_hit.hpp>

/**
 * A node in BVH Tree
 */
class PrimitiveBase {
public:
  virtual ~PrimitiveBase() = default;

  virtual std::optional<RayHit> Hit(const Ray &ray) const = 0;
};
