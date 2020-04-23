#pragma once

#include <memory>
#include <vector>

#include <raytracer/primitives/primitive_base.hpp>
#include <raytracer/shapes/shape.hpp>

/**
 * Bounding Volume Hierarchy
 * Recursively construct and traverse
 */
class PrimitiveGroup : public PrimitiveBase {
public:
  PrimitiveGroup() = default;

  explicit PrimitiveGroup(std::vector<std::unique_ptr<const Shape>> primitives,
                          size_t leaf_size = 10)
      : PrimitiveGroup(std::begin(primitives), std::end(primitives),
                       leaf_size) {}

  std::optional<RayHit> Hit(const Ray &ray) const override;

private:
  PrimitiveGroup(std::vector<std::unique_ptr<const Shape>>::iterator begin,
                 std::vector<std::unique_ptr<const Shape>>::iterator end,
                 size_t leaf_size);

  AABB aabb_;
  std::unique_ptr<PrimitiveBase> left_, right_;
};
