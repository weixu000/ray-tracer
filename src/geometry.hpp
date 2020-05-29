#pragma once
#include <glm/glm.hpp>
#include <variant>
#include <vector>

#include "ray.hpp"

struct Mesh {
  std::vector<glm::vec3> vertices;
  std::vector<glm::uvec3> indices;
  MaterialRef material;
};

struct Sphere {
  glm::mat4 transform;
  MaterialRef material;
};

using Geometry = std::variant<Mesh, Sphere>;