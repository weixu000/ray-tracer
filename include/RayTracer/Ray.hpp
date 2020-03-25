#pragma once
#include <optional>

#include <glm/glm.hpp>

class Material;

struct Ray {
  glm::vec3 origin, direction;

  glm::vec3 operator()(float t) const { return origin + t * direction; }
};

struct RayHit {
  float t;
  const Material *material = nullptr;
  glm::vec3 normal;
};