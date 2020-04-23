#pragma once

#include <vector>

#include <glm/glm.hpp>

/**
 * Generate multiple samples in [0,1]x[0,1]
 */
class Multisampler {
public:
  virtual ~Multisampler() = default;

  virtual const std::vector<glm::vec2>& Sample() const = 0;

  virtual void SetCount(int count) { count_ = count; }

  int Count() const { return count_; };

protected:
  int count_ = 1;
};
