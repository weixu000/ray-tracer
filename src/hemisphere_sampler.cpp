#include "raytracer/samplers/hemisphere_sampler.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>

glm::vec3 HemisphereSampler::Sample(const glm::vec3& n) const {
  using namespace glm;
  const auto xi_1 = dist_(gen_), xi_2 = dist_(gen_);
  const auto theta = acos(xi_1), phi = 2 * pi<float>() * xi_2;
  const auto s = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
  const auto frame = mat3(orientation(normalize(n), {0, 0, 1}));
  return frame * s;
}
