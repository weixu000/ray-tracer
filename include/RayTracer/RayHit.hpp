#pragma once

#include <glm/glm.hpp>

#include "RayTracer/Material.hpp"

struct RayHit {
  float t;
  const Material *material = nullptr;
  glm::vec3 normal;
};