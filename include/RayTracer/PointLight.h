#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Light.h"

struct PointLight : public Light {
  glm::vec3 position;
};