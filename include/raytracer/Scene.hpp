#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "raytracer/PrimitiveGroup.hpp"
#include "raytracer/Ray.hpp"
#include "raytracer/lights/Light.hpp"

/**
 * Hold primitives and lights
 */
class Scene {
public:
  PrimitiveGroup group;

  std::vector<std::unique_ptr<Light>> lights;

  std::optional<RayHit> Trace(const Ray &ray) const { return group.Hit(ray); }
};