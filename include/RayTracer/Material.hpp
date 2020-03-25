#pragma once
#include <glm/glm.hpp>

struct Material {
  glm::vec3 ambient{.2f, .2f, .2f}; // Per each object
  glm::vec3 diffuse, specular, emission;
  float shininess;
};