#include <glm/gtc/constants.hpp>

#include <raytracer/integrators/analytic_direct_integrator.hpp>
#include <raytracer/lights/quad_light.hpp>

glm::vec3 AnalyticDirectIntegrator::ShadePixel(const glm::vec2 &pixel) const {
  using namespace glm;

  const auto ray = camera_.GenerateEyeRay(pixel + .5f);

  return scene_.Trace(
      ray, [&](const LightEmission &emission) { return emission.L_e; },
      [&](const RayHit &hit) {
        const auto &mat = *hit.material;
        const auto incident = ray(hit.t), N = hit.normal;
        auto radiance = glm::vec3(0.f);
        for (const auto &light : scene_.lights) {
          if (const auto quad = dynamic_cast<const QuadLight *>(light.get())) {
            radiance += mat.diffuse / pi<float>() * quad->GetRadiance() *
                        max(0.f, dot(quad->GetIrradianceVector(incident), N));
          }
        }
        return radiance;
      });
}
