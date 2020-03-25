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
  Light() = default;

  explicit Light(const glm::vec3 &color);

  virtual std::optional<LightCast>
  GenerateLightRay(const glm::vec3 &position, const Scene &scene) const = 0;

  static inline glm::vec3 attenuation{1.f, 0.f, 0.f};

protected:
  const glm::vec3 color_{};
};