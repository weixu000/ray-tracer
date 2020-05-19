#pragma once

#include <memory>
#include <variant>
#include <vector>

#include "../soa.hpp"
#include "sphere.hpp"
#include "triangle.hpp"

/**
 * Bounding Volume Hierarchy
 * Recursively construct and traverse
 */
class BVH {
 public:
  BVH() = default;

  explicit BVH(std::vector<std::unique_ptr<const Shape>> primitives);

  std::optional<RayHit> Hit(const Ray &ray) const { return Hit(0, ray); }

 private:
  struct NonLeaf {
    AABB aabb;
    size_t left, right;
  };

  struct Leaf {
    AABB aabb;
    SoA<Sphere, Triangle>::Ref ref;
  };

  using Node = std::variant<NonLeaf, Leaf>;

  size_t Construct(std::vector<std::unique_ptr<const Shape>>::iterator begin,
                   std::vector<std::unique_ptr<const Shape>>::iterator end);

  std::optional<RayHit> Hit(size_t i, const Ray &ray) const;

  std::vector<Node> nodes_;
  SoA<Sphere, Triangle> shapes_;
};
