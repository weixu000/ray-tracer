#include <glm/gtx/norm.hpp>

template <typename Sampler>
glm::vec3 DirectIntegrator<Sampler>::LightDirect(
    const glm::vec3& x, const glm::vec3& n, const glm::vec3& w_o,
    const MaterialRef& material) const {
  using namespace glm;

  auto radiance = vec3(0.f);
  for (const auto& light : scene_.lights) {
    auto radiance_light = vec3(0.f);
    for (const auto& sample : sampler_.Sample()) {
      const auto light_sample = light->GetSample(x, sample);
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
        radiance_light += light_sample.radience * light_sample.jacobian *
                          scene_.Brdf(material, n, w_i, w_o) * G;
      }
    }
    radiance += radiance_light / float(sampler_.Count());
  }
  return radiance;
}
