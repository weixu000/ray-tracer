#pragma once

#include <glm/glm.hpp>

#include <raytracer/samplers/random.hpp>

class SquareSampler {
public:
  glm::vec2 Sample() const { return {Random(), Random()}; }
};
