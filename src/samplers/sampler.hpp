#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <tuple>

#include "../material.hpp"

class Sampler {
 public:
  virtual ~Sampler() = default;

  virtual std::tuple<glm::vec3, float> Sample(const BRDF& brdf,
                                              const glm::vec3& w_o) const = 0;

 protected:
  glm::vec3 ConvertSpherical(float theta, float phi, const glm::vec3& z) const {
    using namespace glm;
    const auto s =
        vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
    const auto frame = mat3(orientation(z, {0, 0, 1}));
    return frame * s;
  }

  static constexpr auto PI = glm::pi<float>();
};
