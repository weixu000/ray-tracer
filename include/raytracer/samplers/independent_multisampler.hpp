#pragma once

#include <random>

#include <raytracer/samplers/multisampler.hpp>

template <typename Sampler>
class IndependentMultisampler : public Multisampler {
public:
  using Multisampler::Multisampler;

  void Reset() const override {}

  glm::vec2 Sample() const override { return sampler_.Sample(); }

private:
  Sampler sampler_;
};
