#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "raytracer/Ray.hpp"

class Scene;

struct LightRay : public Ray {
  glm::vec3 radience;
  float visible_t;
  glm::vec3 position, normal;
  float area;
};

class Light {
public:
  explicit Light(const glm::vec3 &color);

  virtual ~Light() = default;

  virtual LightRay GenerateLightRay(const glm::vec3 &incident, float u,
                                    float v) const = 0;

  static inline glm::vec3 attenuation{1.f, 0.f, 0.f};

  const glm::vec3 color_;
};