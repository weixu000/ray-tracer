#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "integrators/path_tracer.hpp"
#include "lights/quad_light.hpp"
#include "registry_factory.hpp"
#include "shapes/sphere.hpp"
#include "shapes/triangle.hpp"

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

using Options = unordered_map<string, string>;

template <typename Material, typename... Args>
MaterialRef AddMaterial(Scene &scene, Args... args) {
  const auto ref = scene.materials.AddIfNew<Material>(forward<Args>(args)...);
  return MaterialRef{ref.type, ref.id};
}

auto LoadScene(ifstream &fs) {
  Scene scene;
  Options others;
  vector<vec3> verts;
  vector<mat4> transform_stack;
  vec3 k_d, k_s;
  float s, alpha;
  auto use_ggx = false;

  string line;
  while (getline(fs, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }
    istringstream ss(line);
    string command;
    ss >> command;
    if (command == "sphere") {
      vec3 position;
      float rad;
      ss >> position >> rad;
      const auto mat = use_ggx ? AddMaterial<GGX>(scene, alpha, k_d, k_s)
                               : AddMaterial<Phong>(scene, s, k_d, k_s);
      scene.shapes.emplace_back(make_unique<Sphere>(
          position, rad, StackMatrices(transform_stack), mat));
    } else if (command == "maxverts") {
      continue;  // Not used
    } else if (command == "vertex") {
      vec3 v;
      ss >> v;
      verts.push_back(v);
    } else if (command == "tri") {
      ivec3 t;
      ss >> t;
      const auto mat = use_ggx ? AddMaterial<GGX>(scene, alpha, k_d, k_s)
                               : AddMaterial<Phong>(scene, s, k_d, k_s);
      scene.shapes.emplace_back(
          make_unique<Triangle>(StackMatrices(transform_stack), verts[t[0]],
                                verts[t[1]], verts[t[2]], mat));
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
      throw std::runtime_error(
          "Directional light removed form implementation.");
    } else if (command == "point") {
      throw std::runtime_error("Point light removed form implementation.");
    } else if (command == "quadLight") {
      vec3 v0, e1, e2, intensity;
      ss >> v0 >> e1 >> e2 >> intensity;
      scene.lights.push_back(make_unique<QuadLight>(intensity, v0, e1, e2));
    } else if (command == "diffuse") {
      ss >> k_d;
    } else if (command == "specular") {
      ss >> k_s;
    } else if (command == "shininess") {
      ss >> s;
    } else if (command == "roughness") {
      ss >> alpha;
    } else if (command == "brdf") {
      string option;
      getline(ss >> ws, option);
      use_ggx = option == "ggx";
    } else {
      string option;
      getline(ss >> ws, option);
      others[command] = option;
    }
  }
  return make_tuple<Scene, Options>(move(scene), move(others));
}

auto LoadCamera(const Options &options) {
  istringstream ss;

  int width, height;
  ss.str(options.at("size"));
  ss >> width >> height;
  vec3 look_from, look_at, up;
  float fov;
  ss.clear();
  ss.str(options.at("camera"));
  ss >> look_from >> look_at >> up >> fov;
  return Camera(look_from, look_at, up, fov, width, height);
}

template <Sampling sampling, bool mis, template <auto...> typename T>
using Entry =
    RegistryFactory<Integrator, Sampling, bool>::Entry<sampling, mis, T>;
template <typename... Ts>
using Registry = RegistryFactory<Integrator, Sampling, bool>::Registry<Ts...>;
using RegistryNEE = Registry<Entry<Sampling::Hemisphere, false, PathTracerNEE>,
                             Entry<Sampling::Cosine, false, PathTracerNEE>,
                             Entry<Sampling::BRDF, false, PathTracerNEE>,
                             Entry<Sampling::Hemisphere, true, PathTracerNEE>,
                             Entry<Sampling::Cosine, true, PathTracerNEE>,
                             Entry<Sampling::BRDF, true, PathTracerNEE>>;
using RegistryRR = Registry<Entry<Sampling::Hemisphere, false, PathTracerRR>,
                            Entry<Sampling::Cosine, false, PathTracerRR>,
                            Entry<Sampling::BRDF, false, PathTracerRR>,
                            Entry<Sampling::Hemisphere, true, PathTracerRR>,
                            Entry<Sampling::Cosine, true, PathTracerRR>,
                            Entry<Sampling::BRDF, true, PathTracerRR>>;

template <typename C, typename K, typename V>
V GetDefault(const C &m, const K &key, const V &defval) {
  const auto it = m.find(key);
  if (it == m.end()) return defval;
  istringstream ss(it->second);
  V ret;
  ss >> ret;
  return ret;
}

unique_ptr<Integrator> LoadIntegrator(const Options &options, Scene scene,
                                      Camera camera) {
  if (options.at("integrator") == "pathtracer") {
    if (GetDefault(options, "nexteventestimation", "off"s) == "off") {
      throw std::runtime_error("Simple path tracer removed");
    }

    const auto mis =
        GetDefault(options, "nexteventestimation", "off"s) == "mis";
    if (mis) cout << "Multiple Importance Sampling enabled" << endl;

    const auto russian_roulette =
        GetDefault(options, "russianroulette", "off"s) == "on";
    if (russian_roulette) cout << "Russian Roulette enabled" << endl;

    const auto spp = GetDefault(options, "spp", 1);
    cout << "Sample per pixel: " << spp << endl;

    const auto max_depth = GetDefault(options, "maxdepth", 5);
    if (!russian_roulette) cout << "Max depth: " << max_depth << endl;

    const auto gamma = GetDefault(options, "gamma", 1.f);
    cout << "Gamma: " << gamma << endl;

    Sampling importance_sampling = Sampling::Hemisphere;
    if (options.count("importancesampling")) {
      const auto method = options.at("importancesampling");
      if (method == "cosine") {
        importance_sampling = Sampling::Cosine;
      } else if (method == "brdf") {
        importance_sampling = Sampling::BRDF;
      }
    }

    if (russian_roulette) {
      return RegistryRR::Get(importance_sampling, mis, spp, std::move(scene),
                             std::move(camera), gamma);
    } else {
      return RegistryNEE::Get(importance_sampling, mis, max_depth, spp,
                              std::move(scene), std::move(camera), gamma);
    }
  } else {
    throw std::runtime_error("Unknown integrator: " + options.at("integrator"));
  }
}
}  // namespace

int main(int argc, char **argv) {
  if (argc != 2) {
    throw std::runtime_error("Incorrect command-line options");
  }
  string input_path = argv[1];
  ifstream fs(input_path);
  if (!fs.is_open()) {
    throw std::runtime_error("Cannot open input file: " + input_path);
  }
  cout << "Parsing: " << input_path << endl;

  auto [scene, options] = LoadScene(fs);
  auto camera = LoadCamera(options);
  const auto integrator =
      LoadIntegrator(options, std::move(scene), std::move(camera));

  istringstream ss;
  string output_file;
  ss.str(options.at("output"));
  ss >> output_file;

  integrator->Render().WriteTo(output_file);
  cout << "Output: " << output_file << endl;
}