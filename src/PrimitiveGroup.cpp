#include "raytracer/PrimitiveGroup.hpp"

PrimitiveGroup::PrimitiveGroup(std::vector<Primitive> primitives)
    : primitives_(std::move(primitives)) {
  std::vector<AABB> boxes;
  boxes.reserve(primitives_.size());
  for (const auto &p : primitives_) {
    boxes.push_back(p.GetWorldAABB());
  }
  bvh_ = std::move(BVH(boxes));
}

std::optional<RayHit> PrimitiveGroup::Hit(const Ray &ray) const {
  const auto hit = bvh_.Traverse(
      ray, [&](const size_t i) { return primitives_[i].Hit(ray); });
  if (hit) {
    return hit->first;
  } else {
    return std::nullopt;
  }
}
