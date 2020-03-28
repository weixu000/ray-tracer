#include "RayTracer/AABB.hpp"

#include <utility>

using namespace std;

bool AABB::Hit(const Ray &ray) const {
  auto tmin = (min.x - ray.origin.x) / ray.direction.x;
  auto tmax = (max.x - ray.origin.x) / ray.direction.x;
  if (tmin > tmax)
    swap(tmin, tmax);

  auto tymin = (min.y - ray.origin.y) / ray.direction.y;
  auto tymax = (max.y - ray.origin.y) / ray.direction.y;
  if (tymin > tymax)
    swap(tymin, tymax);

  if ((tmin > tymax) || (tymin > tmax))
    return false;

  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;

  auto tzmin = (min.z - ray.origin.z) / ray.direction.z;
  auto tzmax = (max.z - ray.origin.z) / ray.direction.z;
  if (tzmin > tzmax)
    swap(tzmin, tzmax);

  return !((tmin > tzmax) || (tzmin > tmax));
}

void AABB::Union(const AABB &other) {
  min = glm::min(min, other.min);
  max = glm::max(max, other.max);
}

glm::vec3 AABB::Center() const { return (min + max) / 2.f; }