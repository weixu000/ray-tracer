#pragma once

#include <raytracer/samplers/multisampler.hpp>

template <typename Sampler>
class IndependentMultisampler : public Multisampler {
public:
  using Multisampler::Multisampler;

  const std::vector<glm::vec2>& Sample() const override {
    static thread_local std::vector<glm::vec2> samples;
    samples.resize(count_);

    for (auto& s : samples) {
      s = sampler_.Sample();
    }
    return samples;
  }

private:
  Sampler sampler_;
};
