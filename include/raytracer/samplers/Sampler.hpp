#pragma once
#include <glm/glm.hpp>

/**
 * Generate samples in [0,1]x[0,1]
 */
class Sampler {
public:
  explicit Sampler(int count = 1) : count(count) {}

  virtual ~Sampler() = default;

  virtual void Reset() = 0;

  virtual glm::vec2 Sample() = 0;

  int count;
};
