#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "raytracer/Ray.hpp"
#include "raytracer/lights/Light.hpp"
#include "raytracer/primitives/PrimitiveGroup.hpp"

/**
 * Hold primitives and lights
 */
class Scene {
public:
  PrimitiveGroup group;

  std::vector<std::unique_ptr<Light>> lights;

  std::optional<RayHit> Trace(const Ray &ray) const { return group.Hit(ray); }
};