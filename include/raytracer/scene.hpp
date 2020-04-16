#pragma once

#include <memory>
#include <string>
#include <vector>

#include <raytracer/lights/delta_light.hpp>
#include <raytracer/lights/light.hpp>
#include <raytracer/primitives/primitive_group.hpp>

/**
 * Hold primitives and lights
 */
class Scene {
public:
  PrimitiveGroup group;
  std::vector<std::unique_ptr<Light>> lights;
  std::vector<std::unique_ptr<DeltaLight>> delta_lights;

  std::optional<RayHit> Trace(const Ray &ray) const { return group.Hit(ray); }
};