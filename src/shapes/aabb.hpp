#pragma once

#include <climits>

#include "../ray.hpp"

/**
 * Axis-aligned bounding box
 */
struct AABB {
  bool Hit(const Ray &ray) const;

  void Union(const AABB &other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
  }

  glm::vec3 Center() const { return (min + max) / 2.f; }

  glm::vec3 min{FLT_MAX}, max{-FLT_MAX};
};
