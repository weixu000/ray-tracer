#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Light.hpp"

struct DirectionalLight : public Light {
  glm::vec3 direction;
};