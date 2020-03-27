#include "RayTracer/RayHit.hpp"

RayHit::RayHit(float t, const glm::vec3 &normal) : t(t), normal(normal) {}
