#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <RayTracer/ImageWriter.hpp>
#include <RayTracer/Material.hpp>
#include <RayTracer/Sampler.hpp>
#include <RayTracer/Scene.hpp>
#include <RayTracer/Sphere.hpp>

using namespace std;

static Scene scene;

int main() {
  scene.output_file = "hello.png";

  scene.width = scene.height = 400;
  scene.camera = Camera(glm::vec3(-5.f), glm::vec3(0.f, 0.f, 0.f),
                        glm::vec3(0.f, 1.f, 0.f), 75);

  Material material;
  material.ambient = glm::vec3(1.f);
  auto s = Sphere(glm::translate(glm::vec3(0.f)), material,
                  glm::vec3(0.f, 0.f, 0.f), 1.f);
  scene.geometries.push_back(make_unique<Sphere>(move(s)));

  Sampler renderer;
  auto image = renderer.Render(scene);
  ImageWriter::WriteTo(scene.output_file, scene.width, scene.height, image);

  return EXIT_SUCCESS;
}