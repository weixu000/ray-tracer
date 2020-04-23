#pragma once

#include <raytracer/samplers/multisampler.hpp>
#include <raytracer/samplers/random.hpp>

class StratifiedMultisampler : public Multisampler {
public:
  using Multisampler::Multisampler;

  void SetCount(int count) override {
    Multisampler::SetCount(count);
    size = int(glm::ceil(glm::sqrt(count_)));
    step = 1.f / size;
  }

  const std::vector<glm::vec2>& Sample() const override {
    static thread_local std::vector<glm::vec2> samples;
    samples.resize(count_);

    auto x = 0.f, y = 0.f;
    for (auto& s : samples) {
      s = glm::vec2(x + Random() * step, y + Random() * step);
      y += step;
      if (y >= 1) {
        x += step;
        y = 0.f;
      }
    }
    return samples;
  }

private:
  int size = 1;
  float step = 1.f;
};
