#pragma once

#include <random>

#include <raytracer/samplers/sampler.hpp>

class StratifiedSampler : public Sampler {
public:
  using Sampler::Sampler;

  void Reset() override {
    size = glm::sqrt(count);
    step = 1.f / size;
    x = y = 0.f;
  }

  glm::vec2 Sample() override {
    const auto sample =
        glm::vec2(x + dist_(gen_) * step, y + dist_(gen_) * step);
    y += step;
    if (y >= 1) {
      x += step;
      y = 0.f;
    }
    return sample;
  }

private:
  std::default_random_engine gen_{std::random_device()()};
  std::uniform_real_distribution<float> dist_;

  int size;
  float x, y, step;
};
