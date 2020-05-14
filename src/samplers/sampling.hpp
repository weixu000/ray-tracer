#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <random>

constexpr auto PI = glm::pi<float>();
constexpr auto TWO_PI = glm::two_pi<float>();
constexpr auto ONE_OVER_PI = glm::one_over_pi<float>();
constexpr auto TWO_OVER_PI = glm::one_over_two_pi<float>();

inline float Random() {
  static thread_local std::default_random_engine gen{std::random_device()()};
  static thread_local std::uniform_real_distribution<float> dist;
  return dist(gen);
}

inline glm::vec3 ConvertSpherical(float theta, float phi, const glm::vec3 &z) {
  using namespace glm;
  const auto s = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
  const auto frame = mat3(orientation(z, {0, 0, 1}));
  return frame * s;
}