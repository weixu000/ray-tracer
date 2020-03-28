#pragma once

#include <climits>

#include <glm/glm.hpp>

#include "RayTracer/Ray.hpp"

struct AABB {
  bool Hit(const Ray &ray) const;

  void Union(const AABB &other);

  glm::vec3 Center() const;

  glm::vec3 min{FLT_MAX}, max{-FLT_MAX};
};
