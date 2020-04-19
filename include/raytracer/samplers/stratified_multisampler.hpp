#pragma once

#include <random>

#include <raytracer/samplers/multisampler.hpp>

class StratifiedMultisampler : public Multisampler {
public:
  using Multisampler::Multisampler;

  void Reset() const override {
    size = glm::sqrt(count);
    step = 1.f / size;
    x = y = 0.f;
  }

  glm::vec2 Sample() const override {
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
  mutable std::default_random_engine gen_{std::random_device()()};
  mutable std::uniform_real_distribution<float> dist_;

  mutable int size;
  mutable float x, y, step;
};
