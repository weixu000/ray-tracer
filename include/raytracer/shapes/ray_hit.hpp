#pragma once

#include <raytracer/material.hpp>
#include <raytracer/shapes/local_info.hpp>

/**
 * local info of the primitive hit by the ray
 */
struct RayHit : public LocalInfo {
  const Material *material = nullptr;
};