#pragma once
#include <vector>

#include "raytracer/BVH.hpp"
#include "raytracer/Primitive.hpp"

/*
 * Use acceleration structure to hold all primitives
 */
class PrimitiveGroup {
public:
  PrimitiveGroup() = default;

  explicit PrimitiveGroup(std::vector<Primitive> primitives);

  std::optional<RayHit> Hit(const Ray &ray) const;

private:
  std::vector<Primitive> primitives_;

  BVH bvh_;
};
