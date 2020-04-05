#include "raytracer/Image.hpp"

#include <stb_image_write.h>

void Image::WriteTo(const std::string &file_path) const {
  auto ext = file_path.substr(file_path.size() - 4);
  if (ext == ".png") {
    stbi_write_png(file_path.c_str(), width, height, 3, pixels.data(),
                   width * sizeof(glm::u8vec3));
  } else {
    // TODO: More format
  }
}
