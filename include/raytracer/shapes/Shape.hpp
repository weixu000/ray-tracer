#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "raytracer/AABB.hpp"
#include "raytracer/Ray.hpp"
#include "raytracer/RayHit.hpp"
#include "raytracer/shapes/LocalInfo.hpp"

class Shape {
public:
  virtual ~Shape() = default;

  virtual std::optional<LocalInfo> Hit(const Ray &ray) const = 0;

  virtual AABB GetAABB() const = 0;
};