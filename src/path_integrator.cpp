#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/component_wise.hpp>

#include <raytracer/integrators/path_integrator.hpp>
#include <raytracer/samplers/random.hpp>

template class PathIntegratorNEE<false, SquareMultiampler>;
template class PathIntegratorNEE<false, StratifiedMultisampler>;
template class PathIntegratorNEE<true, SquareMultiampler>;
template class PathIntegratorNEE<true, StratifiedMultisampler>;

glm::vec3 PathIntegrator::Sample(const Ray &ray, int depth) const {
  using namespace glm;

  if (depth <= 0) {
    return vec3(0.f);
  }

  return scene_.Trace(
      ray, [&](const LightEmission &emission) { return emission.L_e; },
      [&](const RayHit &hit) {
        const auto brdf = hit.material->GetBRDF(hit);
        const auto x = ray(hit.t), n = normalize(hit.normal);
        const auto w_i = normalize(sphere_sampler_.Sample(n)),
                   w_o = -normalize(ray.direction);
        const auto L_r = Sample({x + w_i * SHADOW_EPSILON, w_i}, depth - 1);
        return 2 * glm::pi<float>() * L_r * brdf(w_i, w_o) * dot(n, w_i);
      });
}

template <bool russian_roulette, typename Sampler>
glm::vec3 PathIntegratorNEE<russian_roulette, Sampler>::Sample(
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

template <bool russian_roulette, typename Sampler>
glm::vec3 PathIntegratorNEE<russian_roulette, Sampler>::LightIndirect(
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

  const auto w_i = normalize(sphere_sampler_.Sample(n));
  const auto ray_i = Ray{x + w_i * SHADOW_EPSILON, w_i};

  return Base::scene_.Trace(
      ray_i, [&](const LightEmission &emission) { return vec3(0.f); },
      [&](const RayHit &hit) {
        const auto brdf_i = hit.material->GetBRDF(hit);
        const auto x_i = ray_i(hit.t), n_i = hit.normal;
        if constexpr (russian_roulette) {
          const auto throughput_RR = 2 * glm::pi<float>() * brdf(w_i, w_o) *
                                     dot(n, w_i) * throughput_or_depth /
                                     (1 - q);
          return Base::LightDirect(x_i, n_i, -w_i, brdf_i) * throughput_RR +
                 LightIndirect(x_i, n_i, -w_i, brdf_i, throughput_RR);
        } else {
          const auto L =
              Base::LightDirect(x_i, n_i, -w_i, brdf_i) +
              LightIndirect(x_i, n_i, -w_i, brdf_i, throughput_or_depth - 1);
          return 2 * glm::pi<float>() * L * brdf(w_i, w_o) * dot(n, w_i);
        }
      });
}
