#pragma once

#include <glm/glm.hpp>

#include "../samplers/sampling.hpp"

/**
 * Handle diffuse with specular
 */
class Material {
 public:
  Material(const glm::vec3 &k_d, const glm::vec3 &k_s) : k_d_(k_d), k_s_(k_s) {}

  virtual ~Material() = default;

  glm::vec3 Brdf(const glm::vec3 &n, const glm::vec3 &w_i,
                 const glm::vec3 &w_o) const {
    if (dot(w_i, n) > 0)
      return k_d_ * ONE_OVER_PI + BrdfSpecular(n, w_i, w_o);
    else
      return BrdfSpecular(n, w_i, w_o);
  }

  glm::vec3 Sample(const glm::vec3 &n, const glm::vec3 &w_i) const {
    if (Random() <= ProbSpecular())
      return SampleSpecular(n, w_i);
    else {
      using namespace glm;
      const auto xi_1 = Random(), xi_2 = Random();
      const auto theta = acos(sqrt(xi_1)), phi = TWO_PI * xi_2;
      return ConvertSpherical(theta, phi, n);
    }
  }

  float Pdf(const glm::vec3 &n, const glm::vec3 &w_i,
            const glm::vec3 &w_o) const {
    using namespace glm;

    const auto t = ProbSpecular();
    return (1 - t) * max(0.f, dot(n, w_o)) * ONE_OVER_PI +
           t * PdfSpecular(n, w_i, w_o);
  }

 protected:
  glm::vec3 k_d_, k_s_;

 private:
  virtual glm::vec3 BrdfSpecular(const glm::vec3 &n, const glm::vec3 &w_i,
                                 const glm::vec3 &w_o) const = 0;

  virtual glm::vec3 SampleSpecular(const glm::vec3 &n,
                                   const glm::vec3 &w_i) const = 0;

  virtual float PdfSpecular(const glm::vec3 &n, const glm::vec3 &w_i,
                            const glm::vec3 &w_o) const = 0;

  virtual float ProbSpecular() const = 0;
};
