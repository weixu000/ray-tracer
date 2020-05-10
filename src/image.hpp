#pragma once

#include <stb_image_write.h>

#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Image {
 public:
  Image(int w, int h) : width(w), height(h), pixels(w * h) {}

  const glm::u8vec3 &At(int i, int j) const { return pixels.at(i * width + j); }

  glm::u8vec3 &At(int i, int j) { return pixels.at(i * width + j); }

  void WriteTo(const std::string &file_path) const {
    auto ext = file_path.substr(file_path.size() - 4);
    if (ext == ".png") {
      stbi_write_png(file_path.c_str(), width, height, 3, pixels.data(),
                     width * sizeof(glm::u8vec3));
    } else {
      // TODO: More format
    }
  }

 private:
  std::vector<glm::u8vec3> pixels;
  int width, height;
};
