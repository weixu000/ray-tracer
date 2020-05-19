#include "bvh.hpp"

#include <algorithm>
#include <iostream>

BVH::BVH(std::vector<std::unique_ptr<const Shape>> primitives) {
  Construct(std::begin(primitives), std::end(primitives));
  std::cout << "Nodes: " << nodes_.size() << ", " << sizeof(Node) << " bytes"
            << std::endl;
}

size_t BVH::Construct(std::vector<std::unique_ptr<const Shape>>::iterator begin,
                      std::vector<std::unique_ptr<const Shape>>::iterator end) {
  assert(end > begin);
  if (end - begin == 1) {
    nodes_.emplace_back(
        Leaf{(*begin)->GetAABB(), shapes_.Add(std::move(*begin))});
    return nodes_.size() - 1;
  }

  NonLeaf non_leaf;

  // Find bounding box
  for (auto it = begin; it != end; ++it) {
    non_leaf.aabb.Union((*it)->GetAABB());
  }

  // Find the longest axis
  const auto v = non_leaf.aabb.max - non_leaf.aabb.min;
  const auto axis = v.y > v.x ? (v.z > v.y ? 2 : 1) : (v.z > v.x ? 2 : 0);

  // Split and grow the tree
  const auto mid = begin + (end - begin) / 2;
  std::nth_element(begin, mid, end, [&](const auto &i, const auto &j) {
    const auto &c1 = i->GetAABB().Center(), c2 = j->GetAABB().Center();
    return c1[axis] < c2[axis];
  });

  const auto idx = nodes_.size();
  nodes_.emplace_back();

  non_leaf.left = Construct(begin, mid);
  non_leaf.right = Construct(mid, end);
  nodes_[idx] = std::move(non_leaf);

  return idx;
}

std::optional<RayHit> BVH::Hit(size_t i, const Ray &ray) const {
  return std::visit(
      [&](const auto &node) -> std::optional<RayHit> {
        using T = std::decay_t<decltype(node)>;
        if (!node.aabb.Hit(ray)) {
          return std::nullopt;
        }
        if constexpr (std::is_same_v<T, NonLeaf>) {
          const auto left_hit = Hit(node.left, ray),
                     right_hit = Hit(node.right, ray);
          if (left_hit && right_hit) {
            return left_hit->t < right_hit->t ? left_hit : right_hit;
          } else {
            return left_hit ? left_hit : right_hit;
          }
        } else {
          return shapes_.Call(
              node.ref, [&ray](const auto &s) { return s.HitMaterial(ray); });
        }
      },
      nodes_[i]);
}
