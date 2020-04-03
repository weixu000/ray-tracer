#pragma once
#include <glm/glm.hpp>

class Sampler {
public:
  Sampler(int count = 1) : count(count) {}

  virtual ~Sampler() = default;

  virtual void Reset() = 0;

  virtual glm::vec2 Sample() = 0;

  int count;
};
