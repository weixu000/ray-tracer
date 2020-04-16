#pragma once

#include <optional>

#include <glm/glm.hpp>

#include <raytracer/ray.hpp>

/**
 * Local light information returned by Light::GetSample
 */
struct LightSample {
  glm::vec3 light;
  glm::vec3 radience, normal;
  float jacobian;  // jacobian determinant

  Ray GetShadowRay(const glm::vec3 &x) const {
    const auto d = light - x;
    return Ray{x + d * SHADOW_EPSILON, d};
  }
};

/**
 * Emitted light returned by Light::Hit
 */
struct LightEmission {
  float distance;
  glm::vec3 L_e;
};

class Light {
public:
  virtual ~Light() = default;

  virtual LightSample GetSample(const glm::vec3 &x,
                                const glm::vec2 &uv) const = 0;

  /**
   * Test if the ray hit the light and should draw L_e
   */
  virtual std::optional<LightEmission> Hit(const Ray &ray) const = 0;
};