#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Geometry.h"

struct Sphere : public Geometry {
  glm::vec3 position;
  float radius;
};