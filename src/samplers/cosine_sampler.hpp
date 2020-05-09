#pragma once

#include "random.hpp"
#include "sampler.hpp"

class CosineSampler {
 public:
  std::tuple<glm::vec3, float> Sample(const glm::vec3 &n) const {
    using namespace glm;

    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(sqrt(xi_1)), phi = 2 * PI * xi_2;
    return std::make_tuple(ConvertSpherical(theta, phi, n), sqrt(xi_1) / PI);
  }
};
