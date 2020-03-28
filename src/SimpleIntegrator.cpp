#include "raytracer/integrators/SimpleIntegrator.hpp"

#include <vector>

#include <glm/glm.hpp>

#include "raytracer/Sampler.hpp"

SimpleIntegrator::SimpleIntegrator(const Scene &scene, const Camera &camera,
                                   int max_depth)
    : scene_(scene), camera_(camera), max_depth_(max_depth) {}

std::vector<glm::u8vec3> SimpleIntegrator::Render() const {
  using namespace glm;

  const auto w = camera_.width_;
  const auto h = camera_.height_;

  Sampler sampler(w, h);

  std::vector<u8vec3> output(w * h);
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      const auto ray = camera_.GenerateEyeRay(sampler.Sample(i, j));
      const auto color = Shade(ray, max_depth_);
      output[i * w + j] = u8vec3(min(color, vec3(1.f)) * 255.0f);
    }
  }
  return output;
}

glm::vec3 SimpleIntegrator::Shade(const Ray &ray, int depth) const {
  using namespace std;

  if (depth <= 0) {
    return glm::vec3(0.f);
  }

  auto hit = scene_.Trace(ray);
  if (hit) {
    const auto &mat = *hit->material;
    const auto incident = ray(hit->t), V = -normalize(ray.direction),
               N = normalize(hit->normal);
    auto color = mat.ambient + mat.emission;
    for (const auto &light : scene_.lights) {
      const auto light_ray = light->GenerateLightRay(incident);
      if (const auto shadow_hit = scene_.Trace(light_ray);
          !shadow_hit || shadow_hit->t > light_ray.visible_t) {
        const auto L = normalize(light_ray.direction);
        const auto H = normalize(V + L);
        color += light_ray.radience *
                 (mat.diffuse * max(0.f, dot(N, L)) +
                  mat.specular * pow(max(0.f, dot(N, H)), mat.shininess));
      }
    }
    const auto R = reflect(-V, N);
    const auto reflect_ray = Ray{incident + R * SHADOW_EPSILON, R};
    color += mat.specular * Shade(reflect_ray, depth - 1);
    return color;
  } else {
    return glm::vec3(0.f);
  }
}
