#pragma once

#include <glm/glm.hpp>

/**
 * Direction is normalized
 */
struct Ray {
  glm::vec3 origin, direction;

  glm::vec3 operator()(float t) const { return origin + t * direction; }
};

static inline const float SHADOW_EPSILON = 1E-5f;  // Avoid self-intersection

using MaterialRef = size_t;
using GeometryId = size_t;

/**
 * local info of the primitive hit by the ray
 */
struct RayHit {
  float t;
  glm::vec3 p;
  glm::vec3 n;
  MaterialRef mat;
  GeometryId id;
};