#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <tuple>

#include "random.hpp"

constexpr auto PI = glm::pi<float>();

inline glm::vec3 ConvertSpherical(float theta, float phi, const glm::vec3& z) {
  using namespace glm;
  const auto s = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
  const auto frame = mat3(orientation(z, {0, 0, 1}));
  return frame * s;
}

inline std::tuple<glm::vec3, float> SampleHemisphere(const glm::vec3& n) {
  using namespace glm;

  const auto xi_1 = Random(), xi_2 = Random();
  const auto theta = acos(xi_1), phi = 2 * PI * xi_2;
  return std::make_tuple(ConvertSpherical(theta, phi, n), 1 / (2 * PI));
}

inline std::tuple<glm::vec3, float> SampleConsine(const glm::vec3& n) {
  using namespace glm;

  const auto xi_1 = Random(), xi_2 = Random();
  const auto theta = acos(sqrt(xi_1)), phi = 2 * PI * xi_2;
  return std::make_tuple(ConvertSpherical(theta, phi, n), sqrt(xi_1) / PI);
}