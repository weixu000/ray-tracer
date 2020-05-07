#pragma once

#include "shapes/local_info.hpp"

class BRDF {
public:
  glm::vec3 operator()(const glm::vec3 &w_i, const glm::vec3 &w_o) const;

  glm::vec3 normal;
  glm::vec3 diffuse, specular;
  float shininess;
};

struct Material {
  glm::vec3 diffuse, specular;
  float shininess;

  BRDF GetBRDF(const LocalInfo &info) const {
    return BRDF{info.normal, diffuse, specular, shininess};
  }
};