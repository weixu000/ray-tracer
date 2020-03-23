#pragma once
#include <glm/glm.hpp>

#include "RayTracer/Geometry.h"

struct Mesh : public Geometry {
  std::vector<glm::ivec3> triangles;
};