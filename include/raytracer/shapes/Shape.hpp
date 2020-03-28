#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "raytracer/AABB.hpp"
#include "raytracer/Ray.hpp"
#include "raytracer/RayHit.hpp"

class Shape {
public:
  Shape() = default;

  virtual ~Shape() = default;

  virtual std::optional<RayHit> Hit(const Ray &ray) const = 0;

  virtual AABB GetWorldAABB() const = 0;
};