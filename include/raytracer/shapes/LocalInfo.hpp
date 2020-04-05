#pragma once
#include <glm/glm.hpp>

/**
 * Local geometry information for Shape::Hit
 */
struct LocalInfo {
  float t;
  glm::vec3 normal;
};