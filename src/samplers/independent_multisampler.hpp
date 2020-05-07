#pragma once

#include "multisampler.hpp"
#include "square_sampler.hpp"

template <typename Sampler>
class IndependentMultisampler : public Multisampler {
 public:
  explicit IndependentMultisampler(int count) : Multisampler(count) {}

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

using SquareMultiampler = IndependentMultisampler<SquareSampler>;
