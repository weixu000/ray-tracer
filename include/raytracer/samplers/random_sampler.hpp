#pragma once

#include <random>

#include <raytracer/samplers/sampler.hpp>

class RandomSampler : public Sampler {
public:
  using Sampler::Sampler;

  void Reset() override {}

  glm::vec2 Sample() override { return {dist_(gen_), dist_(gen_)}; }

private:
  std::default_random_engine gen_{std::random_device()()};
  std::uniform_real_distribution<float> dist_;
};
