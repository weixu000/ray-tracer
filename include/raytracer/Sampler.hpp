#pragma once
#include <glm/glm.hpp>

class Sampler {
public:
  Sampler(int w, int h);

  glm::vec2 Sample(int i, int j);

private:
  int width_, height_;
};
