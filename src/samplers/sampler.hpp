#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <tuple>

constexpr auto PI = glm::pi<float>();

inline glm::vec3 ConvertSpherical(float theta, float phi, const glm::vec3& z) {
  using namespace glm;
  const auto s = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
  const auto frame = mat3(orientation(z, {0, 0, 1}));
  return frame * s;
}