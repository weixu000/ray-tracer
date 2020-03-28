#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <raytracer/ImageWriter.hpp>
#include <raytracer/Material.hpp>
#include <raytracer/Scene.hpp>
#include <raytracer/integrators/SimpleIntegrator.hpp>
#include <raytracer/shapes/Sphere.hpp>

using namespace std;

static Scene scene;

int main() {
  scene.output_file = "hello.png";

  int width = 400, height = 400;
  auto camera = Camera(glm::vec3(-5.f), glm::vec3(0.f, 0.f, 0.f),
                       glm::vec3(0.f, 1.f, 0.f), 75, width, height);

  Material material;
  material.ambient = glm::vec3(1.f);
  auto s =
      Sphere(glm::translate(glm::vec3(0.f)), glm::vec3(0.f, 0.f, 0.f), 1.f);
  scene.primitives.emplace_back(make_unique<Sphere>(move(s)), material);

  SimpleIntegrator renderer(scene, camera);
  auto image = renderer.Render();
  ImageWriter::WriteTo(scene.output_file, width, height, image);

  return EXIT_SUCCESS;
}