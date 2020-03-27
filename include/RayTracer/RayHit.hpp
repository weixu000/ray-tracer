#pragma once

#include <glm/glm.hpp>

#include "RayTracer/Material.hpp"

struct RayHit {
  RayHit(float t, const glm::vec3 &normal);

  float t;
  glm::vec3 normal;
  const Material *material = nullptr;
};