#pragma once

#include <memory>
#include <string>
#include <vector>

#include <raytracer/lights/light.hpp>
#include <raytracer/primitives/primitive_group.hpp>
#include <raytracer/ray.hpp>

/**
 * Hold primitives and lights
 */
class Scene {
public:
  PrimitiveGroup group;

  std::vector<std::unique_ptr<Light>> lights;

  std::optional<RayHit> Trace(const Ray &ray) const { return group.Hit(ray); }
};