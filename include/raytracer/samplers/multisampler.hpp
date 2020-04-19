#pragma once

#include <glm/glm.hpp>

/**
 * Generate multiple samples in [0,1]x[0,1]
 */
class Multisampler {
public:
  explicit Multisampler(int count = 1) : count(count) {}

  virtual ~Multisampler() = default;

  virtual void Reset() const = 0;

  virtual glm::vec2 Sample() const = 0;

  int count;
};
