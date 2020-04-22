#include <glm/gtx/norm.hpp>

#include <raytracer/integrators/direct_integrator.hpp>

glm::vec3 DirectIntegrator::ShadePixel(const glm::vec2& pixel) const {
  using namespace glm;

  const auto ray = camera_.GenerateEyeRay(pixel + .5f);

  return scene_.Trace(
      ray, [&](const LightEmission& emission) { return emission.L_e; },
      [&](const RayHit& hit) {
        const auto brdf = hit.material->GetBRDF(hit);
        const auto x = ray(hit.t), n = hit.normal, w_o = -ray.direction;
        return LightDirect(x, n, w_o, brdf);
      });
}

glm::vec3 DirectIntegrator::LightDirect(const glm::vec3& x, const glm::vec3& n,
                                        const glm::vec3& w_o,
                                        const BRDF& brdf) const {
  using namespace glm;

  auto radiance = vec3(0.f);
  for (const auto& light : scene_.lights) {
    sampler->Reset();
    auto radiance_light = vec3(0.f);
    for (int i_sample = 0; i_sample < sampler->count; ++i_sample) {
      const auto light_sample = light->GetSample(x, sampler->Sample());
      const auto w_i = normalize(light_sample.light - x);
      const auto n_l = normalize(light_sample.normal);
      const auto w_i_n = dot(w_i, n), w_i_n_l = dot(w_i, n_l);
      if (w_i_n < 0 || w_i_n_l < 0) {
        continue;
      }
      if (const auto shadow_hit =
              scene_.TraceShapes(light_sample.GetShadowRay(x));
          !shadow_hit || shadow_hit->t > light_sample.distance) {
        const auto G = w_i_n * w_i_n_l / length2(light_sample.light - x);
        radiance_light +=
            light_sample.radience * light_sample.jacobian * brdf(w_i, w_o) * G;
      }
    }
    radiance += radiance_light / float(sampler->count);
  }
  return radiance;
}
