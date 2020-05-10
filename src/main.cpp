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

#include "integrators/direct_integrator.hpp"
#include "integrators/path_integrator.hpp"
#include "lights/quad_light.hpp"
#include "registry_factory.hpp"
#include "samplers/independent_multisampler.hpp"
#include "samplers/stratified_multisampler.hpp"
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

template <BRDF type,
          typename Material = conditional_t<type == BRDF::GGX, GGX, Phong>,
          typename... Args>
MaterialRef AddMaterial(vector<Material> &materials, Args... args) {
  Material mat(forward<Args>(args)...);
  if (materials.empty() || materials.back() != mat) {
    materials.push_back(mat);
  }
  return {type, materials.size() - 1};
}

auto LoadScene(ifstream &fs) {
  Scene scene;
  Options others;
  vector<unique_ptr<const Shape>> shapes;
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
      const auto mat = use_ggx
                           ? AddMaterial<BRDF::GGX>(scene.ggx, alpha, k_d, k_s)
                           : AddMaterial<BRDF::Phong>(scene.phong, s, k_d, k_s);
      shapes.emplace_back(make_unique<Sphere>(
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
      const auto mat = use_ggx
                           ? AddMaterial<BRDF::GGX>(scene.ggx, alpha, k_d, k_s)
                           : AddMaterial<BRDF::Phong>(scene.phong, s, k_d, k_s);
      shapes.emplace_back(make_unique<Triangle>(StackMatrices(transform_stack),
                                                verts[t[0]], verts[t[1]],
                                                verts[t[2]], mat));
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
  scene.group = move(BVH(move(shapes)));

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

template <bool light_stratify, Sampling sampling, typename T>
using Entry = RegistryFactory<Integrator, bool, Sampling>::Entry<light_stratify,
                                                                 sampling, T>;
template <typename... Ts>
using Registry = RegistryFactory<Integrator, bool, Sampling>::Registry<Ts...>;

using RegistryNEE =
    Registry<Entry<false, Sampling::Hemisphere,
                   IntegratorNEE<SquareMultiampler, Sampling::Hemisphere>>,
             Entry<false, Sampling::Cosine,
                   IntegratorNEE<SquareMultiampler, Sampling::Cosine>>,
             Entry<false, Sampling::BRDF,
                   IntegratorNEE<SquareMultiampler, Sampling::BRDF>>,
             Entry<true, Sampling::Hemisphere,
                   IntegratorNEE<StratifiedMultisampler, Sampling::Hemisphere>>,
             Entry<true, Sampling::Cosine,
                   IntegratorNEE<StratifiedMultisampler, Sampling::Cosine>>,
             Entry<true, Sampling::BRDF,
                   IntegratorNEE<StratifiedMultisampler, Sampling::BRDF>>>;

using RegistryRR =
    Registry<Entry<false, Sampling::Hemisphere,
                   IntegratorRR<SquareMultiampler, Sampling::Hemisphere>>,
             Entry<false, Sampling::Cosine,
                   IntegratorRR<SquareMultiampler, Sampling::Cosine>>,
             Entry<false, Sampling::BRDF,
                   IntegratorRR<SquareMultiampler, Sampling::BRDF>>,
             Entry<true, Sampling::Hemisphere,
                   IntegratorRR<StratifiedMultisampler, Sampling::Hemisphere>>,
             Entry<true, Sampling::Cosine,
                   IntegratorRR<StratifiedMultisampler, Sampling::Cosine>>,
             Entry<true, Sampling::BRDF,
                   IntegratorRR<StratifiedMultisampler, Sampling::BRDF>>>;

unique_ptr<Integrator> LoadIntegrator(const Options &options,
                                      const Scene &scene,
                                      const Camera &camera) {
  if (options.at("integrator") == "raytracer") {
    throw std::runtime_error("Simple ray tracer removed form implementation.");
  } else if (options.at("integrator") == "analyticdirect") {
    throw std::runtime_error(
        "Analytic direct integrator removed form implementation.");
  } else {
    int num_light_samples = 1;
    if (options.count("lightsamples")) {
      istringstream ss(options.at("lightsamples"));
      ss >> num_light_samples;
    }
    const auto light_stratify =
        options.count("lightstratify") && options.at("lightstratify") == "on";

    if (options.at("integrator") == "direct") {
      if (light_stratify) {
        return make_unique<DirectIntegrator<StratifiedMultisampler>>(
            num_light_samples, scene, camera);
      } else {
        return make_unique<DirectIntegrator<SquareMultiampler>>(
            num_light_samples, scene, camera);
      }
    } else if (options.at("integrator") == "pathtracer") {
      const auto next_event = options.count("nexteventestimation") &&
                              options.at("nexteventestimation") == "on";

      const auto russian_roulette = options.count("russianroulette") &&
                                    options.at("russianroulette") == "on";
      Sampling importance_sampling = Sampling::Hemisphere;
      if (options.count("importancesampling")) {
        const auto method = options.at("importancesampling");
        if (method == "cosine") {
          importance_sampling = Sampling::Cosine;
        } else if (method == "brdf") {
          importance_sampling = Sampling::BRDF;
        }
      }

      int spp = 1;
      if (options.count("spp")) {
        istringstream ss(options.at("spp"));
        ss >> spp;
      }

      int max_depth = 5;
      if (options.count("maxdepth")) {
        istringstream ss(options.at("maxdepth"));
        ss >> max_depth;
      }

      if (!next_event) {
        switch (importance_sampling) {
          case Sampling::Hemisphere:
            return make_unique<PathIntegratorSimple<Sampling::Hemisphere>>(
                spp, max_depth, scene, camera);
          case Sampling::Cosine:
            return make_unique<PathIntegratorSimple<Sampling::Cosine>>(
                spp, max_depth, scene, camera);
          case Sampling::BRDF:
            return make_unique<PathIntegratorSimple<Sampling::BRDF>>(
                spp, max_depth, scene, camera);
        }
      } else if (russian_roulette) {
        return RegistryRR::Get(light_stratify, importance_sampling, spp,
                               num_light_samples, scene, camera);
      } else {
        return RegistryNEE::Get(light_stratify, importance_sampling, max_depth,
                                spp, num_light_samples, scene, camera);
      }
    } else {
      throw std::runtime_error("Unknown integrator: " +
                               options.at("integrator"));
    }
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

  const auto [scene, options] = LoadScene(fs);
  const auto camera = LoadCamera(options);
  const auto integrator = LoadIntegrator(options, scene, camera);

  istringstream ss;
  string output_file;
  ss.str(options.at("output"));
  ss >> output_file;

  integrator->Render().WriteTo(output_file);
}