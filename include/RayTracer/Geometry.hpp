#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "RayTracer/Material.hpp"
#include "RayTracer/Ray.hpp"

struct RayHit {
  float t;
  const Material *material = nullptr;
  glm::vec3 normal;
};

class Geometry {
public:
  Geometry() = default;

  Geometry(const glm::mat4 &transform, const Material &material);

  virtual std::optional<RayHit> Intersect(const Ray &ray) const = 0;

protected:
  Ray WorldToLocal(const Ray &ray) const;

  glm::vec4 LocalToWorld(const glm::vec4 &v) const;

  glm::vec3 NormalToWorld(const glm::vec3 &n) const;

  const Material material_{};

private:
  const glm::mat4 world_{}, local_{};
  const glm::mat3 normal_world_{};
};