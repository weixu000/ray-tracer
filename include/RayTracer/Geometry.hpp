#pragma once
#include <optional>

#include <glm/glm.hpp>

#include "RayTracer/Material.hpp"
#include "RayTracer/Ray.hpp"

class Geometry {
public:
  Geometry() = default;

  Geometry(const glm::mat4 &transform, const Material &material);

  virtual std::optional<RayHit> Intersect(const Ray &ray) const = 0;

  const Material &GetMaterial() const;

protected:
  Ray WorldToLocal(const Ray &ray) const;

  glm::vec4 LocalToWorld(const glm::vec4 &v) const;

private:
  glm::mat4 world_, local_;
  Material material_;
};