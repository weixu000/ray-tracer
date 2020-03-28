#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "raytracer/AABB.hpp"
#include "raytracer/Ray.hpp"
#include "raytracer/RayHit.hpp"

class BVH {
public:
  using Boxes = std::vector<AABB>;
  using Index = Boxes::size_type;
  using TraversalResult = std::pair<RayHit, Index>;
  using IntersectIndex = std::function<std::optional<RayHit>(Index)>;

  BVH() = default;

  explicit BVH(const Boxes &boxes, size_t depth = 15, size_t leaf_size = 10);

  std::optional<TraversalResult> Traverse(const Ray &r,
                                          const IntersectIndex &test) const;

private:
  using Indices = std::vector<Index>;
  using Iterator = Indices::iterator;

  Indices indices_;

  struct Node {
    Iterator begin, end;
    AABB box;
    std::unique_ptr<Node> left, right;
  };
  std::unique_ptr<Node> root_;

  Node *Construct(Iterator begin, Iterator end, const Boxes &boxes,
                  size_t depth, size_t leaf_size);

#ifndef NDEBUG
  size_t num_nodes_ = 0;
#endif
};