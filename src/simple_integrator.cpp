#include <raytracer/integrators/simple_integrator.hpp>

glm::vec3 SimpleIntegrator::Shade(const Ray &ray, int depth) const {
  using namespace glm;

  if (depth <= 0) {
    return glm::vec3(0.f);
  }

  if (const auto hit = scene_.Trace(ray)) {
    const auto &mat = *hit->material;
    const auto x = ray(hit->t), w_o = -normalize(ray.direction),
               n = normalize(hit->normal);
    auto radiance = mat.ambient + mat.emission;
    for (const auto &light : scene_.delta_lights) {
      const auto light_ray = light->GetRay(x);
      if (const auto shadow_hit = scene_.Trace(light_ray.GetShadowRay(x));
          !shadow_hit || shadow_hit->t > light_ray.distance) {
        const auto w_i = normalize(light_ray.w_i);
        const auto halfway = normalize(w_i + w_o);
        radiance +=
            light_ray.color *
            (mat.diffuse * max(0.f, dot(n, w_i)) +
             mat.specular * pow(max(0.f, dot(n, halfway)), mat.shininess));
      }
    }
    const auto R = reflect(-w_o, n);
    const auto reflect_ray = Ray{x + R * SHADOW_EPSILON, R};
    radiance += mat.specular * Shade(reflect_ray, depth - 1);
    return radiance;
  } else {
    return glm::vec3(0.f);
  }
}
