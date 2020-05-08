#pragma once

#include "random.hpp"
#include "sampler.hpp"

class HemisphereSampler : public Sampler {
 public:
  std::tuple<glm::vec3, float> Sample(const BRDF& brdf,
                                      const glm::vec3&) const override {
    using namespace glm;

    const auto n = brdf.normal;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(xi_1), phi = 2 * PI * xi_2;
    return std::make_tuple(ConvertSpherical(theta, phi, n), 1 / (2 * PI));
  }
};
