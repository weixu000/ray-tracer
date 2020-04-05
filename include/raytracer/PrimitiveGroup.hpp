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

  explicit PrimitiveGroup(std::vector<Primitive> primitives)
      : primitives_(std::move(primitives)) {
    std::vector<AABB> boxes;
    boxes.reserve(primitives_.size());
    for (const auto &p : primitives_) {
      boxes.push_back(p.GetAABB());
    }
    bvh_ = std::move(BVH(boxes));
  }

  std::optional<RayHit> Hit(const Ray &ray) const {
    const auto hit = bvh_.Traverse(
        ray, [&](const size_t i) { return primitives_[i].Hit(ray); });
    if (hit) {
      return hit->first;
    } else {
      return std::nullopt;
    }
  }

private:
  std::vector<Primitive> primitives_;
  BVH bvh_;
};
