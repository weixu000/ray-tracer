#pragma once
#include <glm/glm.hpp>

struct Camera {
  glm::vec3 look_from, look_at, up;
  float fov;
};