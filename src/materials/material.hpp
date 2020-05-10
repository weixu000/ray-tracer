#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class Material {
 public:
  Material(const glm::vec3 &k_d, const glm::vec3 &k_s) : k_d_(k_d), k_s_(k_s) {}

  virtual ~Material() = default;

  virtual glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                         const glm::vec3 &w_o) const = 0;

  virtual std::tuple<glm::vec3, float> SampleBrdf(
      const glm::vec3 &n, const glm::vec3 &w_o) const = 0;

 protected:
  glm::vec3 DiffuseBrdf() const { return k_d_ * one_over_pi; }

  glm::vec3 SampleDiffuse(const glm::vec3 &n) const {
    using namespace glm;
    const auto xi_1 = Random(), xi_2 = Random();
    const auto theta = acos(sqrt(xi_1)), phi = two_pi * xi_2;
    return ConvertSpherical(theta, phi, n);
  }

  glm::vec3 k_d_, k_s_;

  static constexpr auto pi = glm::pi<float>();
  static constexpr auto two_pi = glm::two_pi<float>();
  static constexpr auto one_over_pi = glm::one_over_pi<float>();
  static constexpr auto one_over_two_pi = glm::one_over_two_pi<float>();
};
