#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Light.hpp"

struct PointLight : public Light {
  glm::vec3 position;
};