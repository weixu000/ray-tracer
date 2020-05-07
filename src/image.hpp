#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

struct Image {
public:
  Image(int w, int h) : width(w), height(h), pixels(w * h) {}

  const glm::u8vec3 &At(int i, int j) const { return pixels.at(i * width + j); }

  glm::u8vec3 &At(int i, int j) { return pixels.at(i * width + j); }

  void WriteTo(const std::string &file_path) const;

private:
  std::vector<glm::u8vec3> pixels;
  int width, height;
};
