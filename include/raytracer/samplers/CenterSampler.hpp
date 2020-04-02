#pragma once
#include "raytracer/samplers/Sampler.hpp"

class CenterSampler : public Sampler {
public:
  CenterSampler() : Sampler(1) {}

  void Reset() override {}

  glm::vec2 Sample() override { return {.5f, .5f}; }
};
