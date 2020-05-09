#include "local_shape.hpp"

LocalShape::LocalShape(const glm::mat4& transform, const MaterialRef& mat)
    : Shape(mat),
      world_(transform),
      local_(inverse(transform)),
      normal_world_(glm::inverse(glm::transpose(glm::mat3(transform)))) {}

std::optional<LocalInfo> LocalShape::Hit(const Ray &ray) const {
  using namespace glm;
  const auto local_ray =
      Ray{local_ * vec4(ray.origin, 1.f), local_ * vec4(ray.direction, 0.f)};
  if (const auto local = HitLocal(local_ray)) {
    return LocalInfo{local->t, normalize(normal_world_ * local->n)};
  } else {
    return std::nullopt;
  }
}
