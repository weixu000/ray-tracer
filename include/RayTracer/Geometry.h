#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Material.h"

struct Geometry {
  glm::mat4 transform;
  Material material;
};