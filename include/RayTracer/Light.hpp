#pragma once
#include <optional>

#include <glm/glm.hpp>

class Scene;

struct LightCast {
  glm::vec3 attenuated_intensity;
  glm::vec3 direction;
};

class Light {
public:
  glm::vec3 color;
  static inline glm::vec3 attenuation{1.f, 0.f, 0.f};

  virtual std::optional<LightCast>
  GenerateLightRay(const glm::vec3 &position, const Scene &scene) const = 0;

protected:
  static inline const float SHADOW_EPSILON = 1E-4f;
};