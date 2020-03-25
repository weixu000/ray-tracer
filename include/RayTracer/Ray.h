#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "RayTracer/Material.h"

struct Ray {
  glm::vec3 origin, direction;
};

struct RayHit {
  float t;
};