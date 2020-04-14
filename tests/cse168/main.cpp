#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <raytracer/Material.hpp>
#include <raytracer/Scene.hpp>
#include <raytracer/integrators/AnalyticDirectIntegrator.hpp>
#include <raytracer/integrators/DirectIntegrator.hpp>
#include <raytracer/integrators/SimpleIntegrator.hpp>
#include <raytracer/lights/DirectionalLight.hpp>
#include <raytracer/lights/PointLight.hpp>
#include <raytracer/lights/QuadLight.hpp>
#include <raytracer/samplers/RandomSampler.hpp>
#include <raytracer/samplers/StratifiedSampler.hpp>
#include <raytracer/shapes/Sphere.hpp>
#include <raytracer/shapes/Triangle.hpp>

using namespace std;

static Scene scene;
static vector<glm::mat4> transform_stack;
static std::vector<unique_ptr<Shape>> shapes;
static std::vector<glm::vec3> verts;
static Material current_material;
static int width, height;
static int max_depth = 5;
static Camera camera;
static std::unique_ptr<Integrator> integrator =
    std::make_unique<SimpleIntegrator>(scene, camera);
static std::unique_ptr<Sampler> sampler = std::make_unique<RandomSampler>();
static int num_samples = 1;
static string output_file;

static glm::mat4 StackMatrices() {
  glm::mat4 m(1.0f);
  for (const auto &x : transform_stack) {
    m *= x;
  }
  return m;
}

template <class CharT, class Traits, glm::length_t L, typename T,
          glm::qualifier Q>
static std::basic_istream<CharT, Traits> &
operator>>(std::basic_istream<CharT, Traits> &is, glm::vec<L, T, Q> &v) {
  for (int i = 0; i < v.length(); ++i) {
    is >> v[i];
  }
  return is;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "Incorrect command-line options" << endl;
    return EXIT_FAILURE;
  }
  string input_path = argv[1];
  ifstream fs(input_path);
  if (!fs.is_open()) {
    cerr << "Cannot open input file: " << input_path << endl;
    return EXIT_FAILURE;
  }
  cout << "Parsing: " << input_path << endl;
  string line;
  while (getline(fs, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    istringstream ss(line);
    string command;
    ss >> command;
    if (command == "integrator") {
      string name;
      ss >> name;
      if (name == "raytracer") {
        integrator = std::make_unique<SimpleIntegrator>(scene, camera);
      } else if (name == "analyticdirect") {
        integrator = std::make_unique<AnalyticDirectIntegrator>(scene, camera);
      } else if (name == "direct") {
        integrator = std::make_unique<DirectIntegrator>(scene, camera);
      }
    } else if (command == "lightsamples") {
      ss >> num_samples;
      sampler = std::make_unique<RandomSampler>();
    } else if (command == "lightstratify") {
      string op;
      ss >> op;
      if (op == "on") {
        sampler = std::make_unique<StratifiedSampler>();
      }
    } else if (command == "size") {
      ss >> width >> height;
    } else if (command == "maxdepth") {
      ss >> max_depth;
    } else if (command == "output") {
      ss >> output_file;
    } else if (command == "camera") {
      glm::vec3 look_from, look_at, up;
      float fov;
      ss >> look_from >> look_at >> up >> fov;
      camera = Camera(look_from, look_at, up, fov, width, height);
    } else if (command == "sphere") {
      glm::vec3 position;
      float rad;
      ss >> position >> rad;
      shapes.emplace_back(make_unique<Sphere>(current_material, StackMatrices(),
                                              position, rad));
    } else if (command == "maxverts") {
      continue; // Not used
    } else if (command == "vertex") {
      glm::vec3 v;
      ss >> v;
      verts.push_back(v);
    } else if (command == "tri") {
      glm::ivec3 t;
      ss >> t;
      shapes.emplace_back(make_unique<Triangle>(current_material,
                                                StackMatrices(), verts[t[0]],
                                                verts[t[1]], verts[t[2]]));
    } else if (command == "maxvertnorms") {
      continue; // TODO: implement triangle-with-normal
    } else if (command == "vertexnormal") {
      continue; // TODO: implement triangle-with-normal
    } else if (command == "trinormal") {
      continue; // TODO: implement triangle-with-normal
    } else if (command == "translate") {
      glm::vec3 v;
      ss >> v;
      transform_stack.back() = glm::translate(transform_stack.back(), v);
    } else if (command == "rotate") {
      glm::vec3 v;
      float angle;
      ss >> v >> angle;
      transform_stack.back() =
          glm::rotate(transform_stack.back(), glm::radians(angle), v);
    } else if (command == "scale") {
      glm::vec3 v;
      ss >> v;
      transform_stack.back() = glm::scale(transform_stack.back(), v);
    } else if (command == "pushTransform") {
      transform_stack.emplace_back(1.0f);
    } else if (command == "popTransform") {
      transform_stack.pop_back();
    } else if (command == "directional") {
      glm::vec3 direction, color;
      ss >> direction >> color;
      scene.lights.push_back(make_unique<DirectionalLight>(color, direction));
    } else if (command == "point") {
      glm::vec3 position, color;
      ss >> position >> color;
      scene.lights.push_back(make_unique<PointLight>(color, position));
    } else if (command == "quadLight") {
      glm::vec3 v0, e1, e2, intensity;
      ss >> v0 >> e1 >> e2 >> intensity;
      scene.lights.push_back(make_unique<QuadLight>(intensity, v0, e1, e2));
    } else if (command == "attenuation") {
      ss >> PointLight::attenuation;
    } else if (command == "ambient") {
      ss >> current_material.ambient;
    } else if (command == "diffuse") {
      ss >> current_material.diffuse;
    } else if (command == "specular") {
      ss >> current_material.specular;
    } else if (command == "shininess") {
      ss >> current_material.shininess;
    } else if (command == "emission") {
      ss >> current_material.emission;
    } else {
      cerr << "Unknown command in input: " << line << endl;
      return EXIT_FAILURE;
    }
  }
  scene.group = move(PrimitiveGroup(move(shapes)));

  if (const auto simple_integrator =
          dynamic_cast<SimpleIntegrator *>(integrator.get())) {
    simple_integrator->max_depth_ = max_depth;
  } else if (const auto direct_integrator =
                 dynamic_cast<DirectIntegrator *>(integrator.get())) {
    direct_integrator->sampler = sampler.get();
    sampler->count = num_samples;
  }

  cout << "Rendering..." << endl;
  integrator->Render().WriteTo(output_file);

  return EXIT_SUCCESS;
}