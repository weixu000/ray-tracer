#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <raytracer/integrators/analytic_direct_integrator.hpp>
#include <raytracer/integrators/direct_integrator.hpp>
#include <raytracer/integrators/path_integrator.hpp>
#include <raytracer/integrators/simple_integrator.hpp>
#include <raytracer/lights/directional_light.hpp>
#include <raytracer/lights/point_light.hpp>
#include <raytracer/lights/quad_light.hpp>
#include <raytracer/samplers/independent_multisampler.hpp>
#include <raytracer/samplers/square_sampler.hpp>
#include <raytracer/samplers/stratified_multisampler.hpp>
#include <raytracer/shapes/sphere.hpp>
#include <raytracer/shapes/triangle.hpp>

using namespace std;
using namespace glm;

namespace {
mat4 StackMatrices(const vector<mat4> &transform_stack) {
  mat4 m(1.0f);
  for (const auto &x : transform_stack) {
    m *= x;
  }
  return m;
}

template <class CharT, class Traits, length_t L, typename T, qualifier Q>
basic_istream<CharT, Traits> &operator>>(basic_istream<CharT, Traits> &is,
                                         vec<L, T, Q> &v) {
  for (int i = 0; i < v.length(); ++i) {
    is >> v[i];
  }
  return is;
}
}  // namespace

int main(int argc, char **argv) {
  Scene scene;
  vector<mat4> transform_stack;
  vector<unique_ptr<Shape>> shapes;
  vector<vec3> verts;
  Material current_material;
  int width, height;
  int max_depth = 5;
  Camera camera;
  unique_ptr<Integrator> integrator =
      make_unique<SimpleIntegrator>(scene, camera);
  unique_ptr<Multisampler> light_sampler =
      make_unique<IndependentMultisampler<SquareSampler>>();
  int num_light_samples = 1, num_pixel_samples = 1;
  string output_file;
  bool nexteventestimation = false;
  bool russianroulette = false;

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
        integrator = make_unique<SimpleIntegrator>(scene, camera);
      } else if (name == "analyticdirect") {
        integrator = make_unique<AnalyticDirectIntegrator>(scene, camera);
      } else if (name == "direct") {
        integrator = make_unique<DirectIntegrator>(scene, camera);
      } else if (name == "pathtracer") {
        integrator = make_unique<PathIntegrator>(scene, camera);
      }
    } else if (command == "lightsamples") {
      ss >> num_light_samples;
    } else if (command == "lightstratify") {
      string op;
      ss >> op;
      if (op == "on") {
        light_sampler = make_unique<StratifiedMultisampler>();
      }
    } else if (command == "spp") {
      ss >> num_pixel_samples;
    } else if (command == "nexteventestimation") {
      string op;
      ss >> op;
      if (op == "on") {
        nexteventestimation = true;
      }
    } else if (command == "russianroulette") {
      string op;
      ss >> op;
      if (op == "on") {
        russianroulette = true;
      }
    } else if (command == "size") {
      ss >> width >> height;
    } else if (command == "maxdepth") {
      ss >> max_depth;
    } else if (command == "output") {
      ss >> output_file;
    } else if (command == "camera") {
      vec3 look_from, look_at, up;
      float fov;
      ss >> look_from >> look_at >> up >> fov;
      camera = Camera(look_from, look_at, up, fov, width, height);
    } else if (command == "sphere") {
      vec3 position;
      float rad;
      ss >> position >> rad;
      shapes.emplace_back(make_unique<Sphere>(
          current_material, StackMatrices(transform_stack), position, rad));
    } else if (command == "maxverts") {
      continue;  // Not used
    } else if (command == "vertex") {
      vec3 v;
      ss >> v;
      verts.push_back(v);
    } else if (command == "tri") {
      ivec3 t;
      ss >> t;
      shapes.emplace_back(make_unique<Triangle>(
          current_material, StackMatrices(transform_stack), verts[t[0]],
          verts[t[1]], verts[t[2]]));
    } else if (command == "maxvertnorms") {
      continue;  // TODO: implement triangle-with-normal
    } else if (command == "vertexnormal") {
      continue;  // TODO: implement triangle-with-normal
    } else if (command == "trinormal") {
      continue;  // TODO: implement triangle-with-normal
    } else if (command == "translate") {
      vec3 v;
      ss >> v;
      transform_stack.back() = translate(transform_stack.back(), v);
    } else if (command == "rotate") {
      vec3 v;
      float angle;
      ss >> v >> angle;
      transform_stack.back() =
          rotate(transform_stack.back(), radians(angle), v);
    } else if (command == "scale") {
      vec3 v;
      ss >> v;
      transform_stack.back() = scale(transform_stack.back(), v);
    } else if (command == "pushTransform") {
      transform_stack.emplace_back(1.0f);
    } else if (command == "popTransform") {
      transform_stack.pop_back();
    } else if (command == "directional") {
      vec3 direction, color;
      ss >> direction >> color;
      scene.delta_lights.push_back(
          make_unique<DirectionalLight>(color, direction));
    } else if (command == "point") {
      vec3 position, color;
      ss >> position >> color;
      scene.delta_lights.push_back(make_unique<PointLight>(color, position));
    } else if (command == "quadLight") {
      vec3 v0, e1, e2, intensity;
      ss >> v0 >> e1 >> e2 >> intensity;
      scene.lights.push_back(make_unique<QuadLight>(intensity, v0, e1, e2));
    } else if (command == "attenuation") {
      // ss >> PointLight::attenuation;
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

  light_sampler->count = num_light_samples;

  if (const auto simple_integrator =
          dynamic_cast<SimpleIntegrator *>(integrator.get())) {
    simple_integrator->max_depth_ = max_depth;
  } else if (const auto direct_integrator =
                 dynamic_cast<DirectIntegrator *>(integrator.get())) {
    direct_integrator->sampler = move(light_sampler);
    if (const auto path_integrator =
            dynamic_cast<PathIntegrator *>(integrator.get())) {
      path_integrator->max_depth_ = max_depth;
      path_integrator->num_sample_ = num_pixel_samples;
      path_integrator->next_event_ = nexteventestimation;
      path_integrator->russian_roulette_ = russianroulette;
    }
  }

  cout << "Rendering..." << endl;
  integrator->Render().WriteTo(output_file);
}