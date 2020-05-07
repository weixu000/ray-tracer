#pragma once

#include <glm/glm.hpp>

/**
 * Direction is normalized
 */
struct Ray {
  glm::vec3 origin, direction;

  glm::vec3 operator()(float t) const { return origin + t * direction; }
};

static inline const float SHADOW_EPSILON = 1E-3f;  // Avoid self-intersection
