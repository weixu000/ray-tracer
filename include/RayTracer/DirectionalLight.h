#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Light.h"

struct DirectionalLight : public Light {
  glm::vec3 direction;
};