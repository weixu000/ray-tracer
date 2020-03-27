#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "RayTracer/AABB.hpp"
#include "RayTracer/Ray.hpp"
#include "RayTracer/RayHit.hpp"

class Geometry {
public:
  Geometry() = default;

  explicit Geometry(const glm::mat4 &transform);

  virtual ~Geometry() = default;

  virtual std::optional<RayHit> Intersect(const Ray &ray) const = 0;

  virtual AABB GetWorldAABB() const = 0;

protected:
  Ray WorldToLocal(const Ray &ray) const;

  glm::vec4 LocalToWorld(const glm::vec4 &v) const;

  glm::vec3 NormalToWorld(const glm::vec3 &n) const;

  const glm::mat4 world_{}, local_{};
  const glm::mat3 normal_world_{};
};