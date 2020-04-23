#include <algorithm>

#include <raytracer/primitives/primitive_group.hpp>
#include <raytracer/primitives/primitive_list.hpp>

PrimitiveGroup::PrimitiveGroup(
    std::vector<std::unique_ptr<const Shape>>::iterator begin,
    std::vector<std::unique_ptr<const Shape>>::iterator end, size_t leaf_size) {
  // Find bounding box
  for (auto it = begin; it != end; ++it) {
    aabb_.Union((*it)->GetAABB());
  }

  // Find the longest axis
  const auto v = aabb_.max - aabb_.min;
  const auto axis = v.y > v.x ? (v.z > v.y ? 2 : 1) : (v.z > v.x ? 2 : 0);

  // Split and grow the tree
  const auto ind_mid = begin + (end - begin) / 2;
  std::nth_element(begin, ind_mid, end, [&](const auto &i, const auto &j) {
    const auto &c1 = i->GetAABB().Center(), c2 = j->GetAABB().Center();
    return c1[axis] < c2[axis];
  });

  if (ind_mid - begin <= leaf_size) {
    left_ = std::make_unique<PrimitiveList>(begin, ind_mid);
  } else {
    left_ = std::unique_ptr<PrimitiveGroup>(
        new PrimitiveGroup(begin, ind_mid, leaf_size));
  }
  if (end - ind_mid <= leaf_size) {
    right_ = std::make_unique<PrimitiveList>(ind_mid, end);
  } else {
    right_ = std::unique_ptr<PrimitiveGroup>(
        new PrimitiveGroup(ind_mid, end, leaf_size));
  }
}

std::optional<RayHit> PrimitiveGroup::Hit(const Ray &ray) const {
  if (!aabb_.Hit(ray)) {
    return std::nullopt;
  }

  const auto left_hit = left_->Hit(ray), right_hit = right_->Hit(ray);
  if (left_hit && right_hit) {
    return left_hit->t < right_hit->t ? left_hit : right_hit;
  } else {
    return left_hit ? left_hit : right_hit;
  }
}
