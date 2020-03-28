#include "raytracer/Sampler.hpp"

Sampler::Sampler(int w, int h) : width_(w), height_(h) {}

glm::vec2 Sampler::Sample(int i, int j) {
  return {(j + .5f - width_ / 2.f) / (height_ / 2.f),
          (-i - .5f + height_ / 2.f) / (height_ / 2.f)};
}
