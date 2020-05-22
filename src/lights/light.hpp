#pragma once

#include <glm/glm.hpp>
#include <optional>

#include "../ray.hpp"

class Light;

/**
 * Emitted light returned by Light::Hit
 * Light normal is normalized
 */
struct Emission {
  float t;
  glm::vec3 L_e, n;
  float jacobian;  // jacobian determinant
  const Light *light;
};

class Light {
 public:
  virtual ~Light() = default;

  virtual glm::vec3 Sample() const = 0;

  /**
   * Test if the ray hit the light and should draw L_e
   */
  virtual std::optional<Emission> Hit(const Ray &ray) const = 0;
};