#pragma once

#include <glm/glm.hpp>

#include "raytracer/Material.hpp"

struct RayHit {
  float t;
  glm::vec3 normal;
  const Material *material = nullptr;
};