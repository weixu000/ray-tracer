#pragma once

#include <glm/glm.hpp>

#include <raytracer/material.hpp>

/**
 * local info of the primitive hit by the ray
 */
struct RayHit {
  float t;
  glm::vec3 normal;
  const Material *material = nullptr;
  BRDF brdf;
};