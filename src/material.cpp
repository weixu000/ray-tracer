#include <glm/gtc/constants.hpp>

#include <raytracer/material.hpp>

glm::vec3 BRDF::operator()(const glm::vec3 &w_i, const glm::vec3 &w_o) const {
  using namespace glm;

  const auto r = reflect(-w_o, normalize(normal));
  const auto f = (diffuse + specular * (shininess + 2) / 2.f *
                                pow(max(0.f, dot(r, w_i)), shininess)) *
                 one_over_pi<float>();
  return f;
}
