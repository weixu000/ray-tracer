#pragma once

#include <iterator>
#include <memory>
#include <vector>

#include <raytracer/primitives/primitive_base.hpp>
#include <raytracer/shapes/shape.hpp>

/**
 * Simply loop over the shapes
 * Servers as leaf node of BVH tree
 */
class PrimitiveList : public PrimitiveBase {
public:
  PrimitiveList() = default;

  explicit PrimitiveList(std::vector<std::unique_ptr<const Shape>> primitives)
      : PrimitiveList(std::begin(primitives), std::end(primitives)) {}

  PrimitiveList(std::vector<std::unique_ptr<const Shape>>::iterator begin,
                std::vector<std::unique_ptr<const Shape>>::iterator end)
      : primitives_(std::make_move_iterator(begin),
                    std::make_move_iterator(end)) {
    for (const auto &p : primitives_) {
      aabb_.Union(p->GetAABB());
    }
  }

  std::optional<RayHit> Hit(const Ray &ray) const {
    if (!aabb_.Hit(ray)) {
      return std::nullopt;
    }
    std::optional<RayHit> ret;
    for (const auto &p : primitives_) {
      if (!p->GetAABB().Hit(ray)) {
        continue;
      }
      if (const auto hit = p->HitMaterial(ray);
          hit && (!ret || hit->t < ret->t)) {
        ret = hit;
      }
    }
    return ret;
  }

private:
  std::vector<std::unique_ptr<const Shape>> primitives_;
  AABB aabb_;
};
