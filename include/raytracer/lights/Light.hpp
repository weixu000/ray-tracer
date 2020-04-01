#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "raytracer/Ray.hpp"
#include "raytracer/lights/LightSample.hpp"

class Scene;

class Light {
public:
  explicit Light(const glm::vec3 &color);

  virtual ~Light() = default;

  virtual LightSample GenerateLightRay(const glm::vec3 &incident, float u,
                                       float v) const = 0;

  static inline glm::vec3 attenuation{1.f, 0.f, 0.f};

  const glm::vec3 intensity;
};