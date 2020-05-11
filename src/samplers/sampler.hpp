#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <tuple>

#include "random.hpp"

inline glm::vec2 SampleSquare() { return {Random(), Random()}; }

inline glm::vec3 ConvertSpherical(float theta, float phi, const glm::vec3 &z) {
  using namespace glm;
  const auto s = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
  const auto frame = mat3(orientation(z, {0, 0, 1}));
  return frame * s;
}

struct HemisphereSampler {
  glm::vec3 Sample(const glm::vec3 &n) const {
    using namespace glm;

    const auto PI = pi<float>();

    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(xi_1), phi = 2 * PI * xi_2;
    return ConvertSpherical(theta, phi, n);
  }

  float Pdf() const { return glm::one_over_two_pi<float>(); }
};

struct CosineSampler {
  glm::vec3 Sample(const glm::vec3 &n) const {
    using namespace glm;

    const auto PI = pi<float>();

    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(sqrt(xi_1)), phi = 2 * PI * xi_2;
    return ConvertSpherical(theta, phi, n);
  }

  float Pdf(const glm::vec3 &n, const glm::vec3 &w_i) const {
    using namespace glm;
    return dot(n, w_i) / pi<float>();
  }
};
