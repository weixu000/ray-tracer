#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "RayTracer/AABB.hpp"
#include "RayTracer/Ray.hpp"
#include "RayTracer/RayHit.hpp"

class Geometry {
public:
  Geometry() = default;

  virtual ~Geometry() = default;

  virtual std::optional<RayHit> Intersect(const Ray &ray) const = 0;

  virtual AABB GetWorldAABB() const = 0;
};