#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>

#include "../samplers/random.hpp"

template <Sampling sampling>
glm::vec3 PathIntegratorSimple<sampling>::Sample(const Ray &ray,
                                                 int depth) const {
  using namespace glm;

  if (depth <= 0) {
    return vec3(0.f);
  }

  return scene_.Trace(
      ray, [&](const LightEmission &emission) { return emission.L_e; },
      [&](const RayHit &hit) {
        const auto x = ray(hit.t), n = normalize(hit.n),
                   w_o = -normalize(ray.direction);
        const auto [w_i, pdf] = SampleBrdf(hit.mat, hit.n, w_o);
        if (dot(w_i, n) <= 0 || pdf == 0) return vec3{0.f};
        const auto L_r = Sample({x + w_i * SHADOW_EPSILON, w_i}, depth - 1);
        return L_r * scene_.Brdf(hit.mat, hit.n, w_i, w_o) *
               max(0.f, dot(n, w_i)) / pdf;
      });
}

template <bool russian_roulette, typename LightSampler, Sampling sampling>
glm::vec3 PathIntegrator<russian_roulette, LightSampler, sampling>::Sample(
    const Ray &ray) const {
  using namespace glm;

  return scene_.Trace(
      ray, [&](const LightEmission &emission) { return emission.L_e; },
      [&](const RayHit &hit) {
        const auto x = ray(hit.t), n = hit.n;
        const auto w_o = -ray.direction;
        if constexpr (russian_roulette) {
          return LightDirect(x, n, w_o, hit.mat) +
                 LightIndirect(x, n, w_o, hit.mat, vec3(1.f));
        } else {
          return LightDirect(x, n, w_o, hit.mat) +
                 LightIndirect(x, n, w_o, hit.mat, max_depth_ - 1);
        }
      });
}

template <bool russian_roulette, typename LightSampler, Sampling sampling>
glm::vec3
PathIntegrator<russian_roulette, LightSampler, sampling>::LightIndirect(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_o,
    const MaterialRef &material,
    std::conditional_t<russian_roulette, const glm::vec3 &, int>
        throughput_or_depth) const {
  using namespace glm;

  float q;
  if constexpr (russian_roulette) {
    q = 1 - min(1.f, compMax(throughput_or_depth));
    if (Random() <= q) {
      return vec3(0.f);
    }
  } else {
    if (throughput_or_depth <= 0) {
      return vec3(0.f);
    }
  }

  const auto [w_i, pdf] = SampleBrdf(material, n, w_o);
  if (dot(w_i, n) <= 0 || pdf == 0) return vec3{0.f};
  const auto ray_i = Ray{x + w_i * SHADOW_EPSILON, w_i};

  return scene_.Trace(
      ray_i, [&](const LightEmission &emission) { return vec3(0.f); },
      [&](const RayHit &hit) {
        const auto x_i = ray_i(hit.t), n_i = hit.n;
        if constexpr (russian_roulette) {
          const auto throughput_RR = scene_.Brdf(material, n, w_i, w_o) *
                                     max(0.f, dot(n, w_i)) / pdf *
                                     throughput_or_depth / (1 - q);
          return LightDirect(x_i, n_i, -w_i, hit.mat) * throughput_RR +
                 LightIndirect(x_i, n_i, -w_i, hit.mat, throughput_RR);
        } else {
          const auto L =
              LightDirect(x_i, n_i, -w_i, hit.mat) +
              LightIndirect(x_i, n_i, -w_i, hit.mat, throughput_or_depth - 1);
          return L * scene_.Brdf(material, n, w_i, w_o) *
                 max(0.f, dot(n, w_i)) / pdf;
        }
      });
}

template <bool russian_roulette, typename LightSampler, Sampling sampling>
glm::vec3 PathIntegrator<russian_roulette, LightSampler, sampling>::LightDirect(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_o,
    const MaterialRef &material) const {
  using namespace glm;

  auto radiance = vec3(0.f);
  for (const auto &light : scene_.lights) {
    auto radiance_light = vec3(0.f);
    for (const auto &sample : light_sampler_.Sample()) {
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
    radiance += radiance_light / float(light_sampler_.Count());
  }
  return radiance;
}
