#pragma once

#include <vector>

#include <glm/glm.hpp>

/**
 * Generate multiple samples in [0,1]x[0,1]
 */
class Multisampler {
public:
  Multisampler(int count) : count_(count) {}

  virtual ~Multisampler() = default;

  virtual const std::vector<glm::vec2>& Sample() const = 0;

  int Count() const { return count_; };

protected:
  int count_;
};
