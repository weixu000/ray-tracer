#pragma once

#include <glm/glm.hpp>

struct LightRay {
  glm::vec3 w_i;
  glm::vec3 color;
  float distance;

  Ray GetShadowRay(const glm::vec3 &x) const {
    return Ray{x + w_i * SHADOW_EPSILON, w_i};
  }
};

/**
 * For non-physical lights that do not fit into Light structure
 */
class DeltaLight {
public:
  virtual ~DeltaLight() = default;

  virtual LightRay GetRay(const glm::vec3 &x) const = 0;
};
