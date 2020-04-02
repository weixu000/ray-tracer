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

  virtual LightSample GenerateSample(const glm::vec3 &incident,
                                     const glm::vec2 &uv) const = 0;

  virtual std::optional<float> Hit(const Ray &ray) const = 0;

  const glm::vec3 intensity;
};