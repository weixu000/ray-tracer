#include <glm/gtx/norm.hpp>

#include <raytracer/integrators/direct_integrator.hpp>

glm::vec3 DirectIntegrator::ShadePixel(const glm::vec2 &pixel) const {
  using namespace glm;

  const auto ray = camera_.GenerateEyeRay(pixel + .5f);

  auto color = glm::vec3(0.f);
  auto t = FLT_MAX;
  for (const auto &light : scene_.lights) {
    if (const auto hit = light->Hit(ray); hit && hit->distance < t) {
      color = hit->L_e;
      t = hit->distance;
    }
  }

  if (const auto hit = scene_.Trace(ray); hit && hit->t < t) {
    color = glm::vec3(0.f);
    const auto &brdf = hit->brdf;
    const auto x = ray(hit->t), w_o = -normalize(ray.direction),
               n = normalize(hit->normal);
    for (const auto &light : scene_.lights) {
      sampler->Reset();
      auto radiance = glm::vec3(0.f);
      for (int i_sample = 0; i_sample < sampler->count; ++i_sample) {
        const auto light_sample = light->GetSample(x, sampler->Sample());
        const auto w_i = normalize(light_sample.light - x);
        const auto n_l = normalize(light_sample.normal);
        const auto w_i_n = dot(w_i, n), w_i_n_l = dot(w_i, n_l);
        if (w_i_n < 0 || w_i_n_l < 0) {
          continue;
        }
        if (const auto shadow_hit = scene_.Trace(light_sample.GetShadowRay(x));
            !shadow_hit || shadow_hit->t > .99f) {
          const auto G = w_i_n * w_i_n_l / length2(light_sample.light - x);
          radiance += light_sample.radience * light_sample.jacobian *
                      brdf(w_i, w_o) * G;
        }
      }
      color += radiance / float(sampler->count);
    }
  }
  return color;
}
