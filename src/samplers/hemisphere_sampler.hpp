#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <tuple>

#include "random.hpp"

class HemisphereSampler {
 public:
  std::tuple<glm::vec3, float> Sample(const glm::vec3& n) const {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(xi_1), phi = 2 * pi<float>() * xi_2;
    const auto s =
        vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
    const auto frame = mat3(orientation(n, {0, 0, 1}));
    return std::make_tuple(frame * s, 1 / (2 * glm::pi<float>()));
  }
};
