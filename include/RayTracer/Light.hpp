#pragma once
#include <glm/glm.hpp>

struct Light {
  glm::vec3 color;
  static inline glm::vec3 attenuation{1.f, 0.f, 0.f};
};