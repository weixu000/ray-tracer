#include "raytracer/integrators/Integrator.hpp"

Integrator::Integrator(const Scene &scene, const Camera &camera)
    : scene_(scene), camera_(camera) {}