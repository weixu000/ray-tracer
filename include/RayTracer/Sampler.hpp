#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "RayTracer/Scene.hpp"

class Sampler {
public:
  std::vector<glm::u8vec3> Render(const Scene &scene);
};
