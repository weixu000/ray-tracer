#pragma once
#include <glm/glm.hpp>

#include <raytracer/shapes/LocalInfo.hpp>

class BRDF {
public:
  glm::vec3 operator()(const glm::vec3 &w_i, const glm::vec3 &w_o) const;

  glm::vec3 normal;
  glm::vec3 diffuse, specular;
  float shininess;
};

struct Material {
  glm::vec3 ambient{.2f, .2f, .2f}; // Per each object
  glm::vec3 diffuse, specular, emission;
  float shininess;

  BRDF GetBRDF(const LocalInfo &info) const {
    return BRDF{info.normal, diffuse, specular, shininess};
  }
};