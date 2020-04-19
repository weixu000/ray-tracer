#pragma once

#include <random>

#include <glm/glm.hpp>

class HemisphereSampler {
public:
  glm::vec3 Sample(const glm::vec3& n) const;

private:
  mutable std::default_random_engine gen_{std::random_device()()};
  mutable std::uniform_real_distribution<float> dist_;
};
