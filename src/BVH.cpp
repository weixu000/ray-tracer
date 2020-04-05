#include "raytracer/BVH.hpp"

#include <algorithm>
#include <climits>
#include <iostream>
#include <numeric>
#include <stack>

BVH::BVH(const Boxes &boxes, size_t depth, size_t leaf_size) {
  std::cout << "Constructing BVH" << std::endl;

  indices_.resize(boxes.size());
  std::iota(indices_.begin(), indices_.end(), 0);

  root_ = std::unique_ptr<Node>(
      Construct(indices_.begin(), indices_.end(), boxes, depth, leaf_size));
#ifndef NDEBUG
  std::cout << "BVH: " << boxes.size() << " boxes, " << num_nodes_ << " nodes"
            << std::endl;
#endif
}

std::optional<BVH::TraversalResult>
BVH::Traverse(const Ray &r, const HitFunction &test) const {
  std::stack<const Node *> stack;
  stack.push(root_.get());

  std::optional<TraversalResult> ret;

  while (!stack.empty()) {
    const auto node = stack.top();
    stack.pop();
    if (!node || !node->box.Hit(r)) {
      continue;
    }

    stack.push(node->right.get());
    stack.push(node->left.get());

    if (!node->left && !node->right) {
      for (auto iter = node->begin; iter != node->end; ++iter) {
        if (const auto hit = test(*iter);
            hit && (!ret || hit->t < ret->first.t)) {
          ret = std::make_pair(*hit, *iter);
        }
      }
    }
  }
  return ret;
}

BVH::Node *BVH::Construct(Iterator begin, Iterator end, const Boxes &boxes,
                          size_t depth, size_t leaf_size) {
#ifndef NDEBUG
  ++num_nodes_;
#endif

  // Find bounding box
  AABB box;
  for (auto it = begin; it != end; ++it) {
    box.Union(boxes[*it]);
  }

  if (end - begin < leaf_size || depth == 0) {
    return new Node{begin, end, box, nullptr, nullptr};
  }

  // Find the longest axis
  const auto v = box.max - box.min;
  const auto axis = v.y > v.x ? (v.z > v.y ? 2 : 1) : (v.z > v.x ? 2 : 0);

  // Split and grow the tree
  const auto ind_mid = begin + (end - begin) / 2;
  std::nth_element(begin, ind_mid, end, [&](const auto &i, const auto &j) {
    const auto c1 = boxes[i].Center(), c2 = boxes[j].Center();
    return c1[axis] < c2[axis];
  });
  const auto left = Construct(begin, ind_mid, boxes, depth - 1, leaf_size);
  const auto right = Construct(ind_mid, end, boxes, depth - 1, leaf_size);
  return new Node{begin, end, box, std::unique_ptr<Node>(left),
                  std::unique_ptr<Node>(right)};
}
