#pragma once

#include <random>

#include <glm/glm.hpp>

class SquareSampler {
public:
  glm::vec2 Sample() const { return {dist_(gen_), dist_(gen_)}; }

private:
  mutable std::default_random_engine gen_{std::random_device()()};
  mutable std::uniform_real_distribution<float> dist_;
};
