#pragma once

#include <glm/glm.hpp>

#include "sampling.hpp"

struct HemisphereSampler {
  glm::vec3 Sample(const glm::vec3 &n) const {
    using namespace glm;

    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(xi_1), phi = TWO_PI * xi_2;
    return ConvertSpherical(theta, phi, n);
  }

  float Pdf() const { return TWO_PI; }
};

struct CosineSampler {
  glm::vec3 Sample(const glm::vec3 &n) const {
    using namespace glm;

    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(sqrt(xi_1)), phi = TWO_PI * xi_2;
    return ConvertSpherical(theta, phi, n);
  }

  float Pdf(const glm::vec3 &n, const glm::vec3 &w_i) const {
    using namespace glm;
    return dot(n, w_i) / PI;
  }
};
