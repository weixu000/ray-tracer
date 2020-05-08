#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/component_wise.hpp>

#include "../samplers/random.hpp"

template <typename Sampler>
glm::vec3 PathIntegratorSimple<Sampler>::Sample(const Ray &ray,
                                                int depth) const {
  using namespace glm;

  if (depth <= 0) {
    return vec3(0.f);
  }

  return scene_.Trace(
      ray, [&](const LightEmission &emission) { return emission.L_e; },
      [&](const RayHit &hit) {
        const auto brdf = hit.material->GetBRDF(hit);
        const auto x = ray(hit.t), n = normalize(hit.normal),
                   w_o = -normalize(ray.direction);
        const auto [w_i, pdf] = sphere_sampler_.Sample(brdf, w_o);
        if (dot(w_i, n) <= 0 || pdf == 0) return vec3{0.f};
        const auto L_r = Sample({x + w_i * SHADOW_EPSILON, w_i}, depth - 1);
        return L_r * brdf(w_i, w_o) * max(0.f, dot(n, w_i)) / pdf;
      });
}

template <bool russian_roulette, typename LightSampler, typename Sampler>
glm::vec3 PathIntegrator<russian_roulette, LightSampler, Sampler>::Sample(
    const Ray &ray) const {
  using namespace glm;

  return Base::scene_.Trace(
      ray, [&](const LightEmission &emission) { return emission.L_e; },
      [&](const RayHit &hit) {
        const auto brdf = hit.material->GetBRDF(hit);
        const auto x = ray(hit.t), n = hit.normal;
        const auto w_o = -ray.direction;
        if constexpr (russian_roulette) {
          return Base::LightDirect(x, n, w_o, brdf) +
                 LightIndirect(x, n, w_o, brdf, vec3(1.f));
        } else {
          return Base::LightDirect(x, n, w_o, brdf) +
                 LightIndirect(x, n, w_o, brdf, max_depth_ - 1);
        }
      });
}

template <bool russian_roulette, typename LightSampler, typename Sampler>
glm::vec3
PathIntegrator<russian_roulette, LightSampler, Sampler>::LightIndirect(
    const glm::vec3 &x, const glm::vec3 &n, const glm::vec3 &w_o,
    const BRDF &brdf,
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

  const auto [w_i, pdf] = sphere_sampler_.Sample(brdf, w_o);
  if (dot(w_i, n) <= 0 || pdf == 0) return vec3{0.f};
  const auto ray_i = Ray{x + w_i * SHADOW_EPSILON, w_i};

  return Base::scene_.Trace(
      ray_i, [&](const LightEmission &emission) { return vec3(0.f); },
      [&](const RayHit &hit) {
        const auto brdf_i = hit.material->GetBRDF(hit);
        const auto x_i = ray_i(hit.t), n_i = hit.normal;
        if constexpr (russian_roulette) {
          const auto throughput_RR = brdf(w_i, w_o) * max(0.f, dot(n, w_i)) /
                                     pdf * throughput_or_depth / (1 - q);
          return Base::LightDirect(x_i, n_i, -w_i, brdf_i) * throughput_RR +
                 LightIndirect(x_i, n_i, -w_i, brdf_i, throughput_RR);
        } else {
          const auto L =
              Base::LightDirect(x_i, n_i, -w_i, brdf_i) +
              LightIndirect(x_i, n_i, -w_i, brdf_i, throughput_or_depth - 1);
          return L * brdf(w_i, w_o) * max(0.f, dot(n, w_i)) / pdf;
        }
      });
}
