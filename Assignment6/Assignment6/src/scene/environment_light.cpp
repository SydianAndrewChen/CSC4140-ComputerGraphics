#include "environment_light.h"
#include "util/lodepng.h"

namespace CGL { namespace SceneObjects {

  EnvironmentLight::EnvironmentLight(const HDRImageBuffer* envMap)
    : envMap(envMap) {
    init();
  }

  EnvironmentLight::~EnvironmentLight() {
    delete[] pdf_envmap;
    delete[] conds_y;
    delete[] marginal_y;
  }


  void EnvironmentLight::init() {
    uint32_t w = envMap->w, h = envMap->h;
    pdf_envmap = new double[w * h];
    conds_y = new double[w * h];
    marginal_y = new double[h];

    std::cout << "[PathTracer] Initializing environment light...";

    // TODO Assignment 7 Part 3 Task 3 Steps 1,2,3
    // Store the environment map pdf to pdf_envmap
    // Store the marginal distribution for y to marginal_y
    // Store the conditional distribution for x given y to conds_y

    double sum = 0;
    for (int j = 0; j < h; ++j) {
      for (int i = 0; i < w; ++i) {
        pdf_envmap[w * j + i] = envMap->data[w * j + i].illum() * sin(PI * (j + .5) / h);
        sum += pdf_envmap[w * j + i];
      }
    }

    for (int index = 0; index < w*h; ++index){
      pdf_envmap[index] /= sum;
    }
    
    for (int j = 0; j < h; ++j) {
      if (j != 0) marginal_y[j] = marginal_y[j-1];
      for (int i = 0; i < w; ++i) {
        marginal_y[j] += pdf_envmap[j*w+i];
      }
    }
    for (int j = 0; j < h; ++j) {
      double p_j = 0.0;
      for (int i = 0; i < w; ++i) p_j += pdf_envmap[w*j+i];
      conds_y[j*w] = 0;
      for (int i = 1; i < w; ++i) {
        conds_y[w*j+i] = conds_y[w*j+i-1] + pdf_envmap[w*j+i] / p_j;
      }
    }

    if (true)
      std::cout << "Saving out probability_debug image for debug." << std::endl;
    save_probability_debug();

    std::cout << "done." << std::endl;
  }

  // Helper functions

  void EnvironmentLight::save_probability_debug() {
    uint32_t w = envMap->w, h = envMap->h;
    uint8_t* img = new uint8_t[4 * w * h];

    for (int j = 0; j < h; ++j) {
      for (int i = 0; i < w; ++i) {
        img[4 * (j * w + i) + 3] = 255;
        img[4 * (j * w + i) + 0] = 255 * marginal_y[j];
        img[4 * (j * w + i) + 1] = 255 * conds_y[j * w + i];
        img[4 * (j * w + i) + 2] = 0;
      }
    }

    lodepng::encode("probability_debug.png", img, w, h);
    delete[] img;
  }

  Vector2D EnvironmentLight::theta_phi_to_xy(const Vector2D& theta_phi) const {
    uint32_t w = envMap->w, h = envMap->h;
    double x = theta_phi.y / 2. / PI * w;
    double y = theta_phi.x / PI * h;
    return Vector2D(x, y);
  }

  Vector2D EnvironmentLight::xy_to_theta_phi(const Vector2D& xy) const {
    uint32_t w = envMap->w, h = envMap->h;
    double x = xy.x;
    double y = xy.y;
    double phi = x / w * 2.0 * PI;
    double theta = y / h * PI;
    return Vector2D(theta, phi);
  }

  Vector2D EnvironmentLight::dir_to_theta_phi(const Vector3D dir) const {
    Vector3D unit_dir = dir.unit();
    double theta = acos(unit_dir.y);
    double phi = atan2(-unit_dir.z, unit_dir.x) + PI;
    return Vector2D(theta, phi);
  }

  Vector3D EnvironmentLight::theta_phi_to_dir(const Vector2D& theta_phi) const {
    double theta = theta_phi.x;
    double phi = theta_phi.y;

    double y = cos(theta);
    double x = cos(phi - PI) * sin(theta);
    double z = -sin(phi - PI) * sin(theta);

    return Vector3D(x, y, z);
  }

  // Credits to Luowen Qian from Spring 2018 for this more robust bilerp
  Vector3D EnvironmentLight::bilerp(const Vector2D& xy) const {
    long right = lround(xy.x), left, v = lround(xy.y);
    double u1 = right - xy.x + .5, v1;
    if (right == 0 || right == envMap->w) {
      left = envMap->w - 1;
      right = 0;
    }
    else left = right - 1;
    if (v == 0) v1 = v = 1; else if (v == envMap->h) {
      v = envMap->h - 1;
      v1 = 0;
    }
    else v1 = v - xy.y + .5;
    auto bottom = envMap->w * v, top = bottom - envMap->w;
    auto u0 = 1 - u1;
    return (envMap->data[top + left] * u1 + envMap->data[top + right] * u0) * v1 +
      (envMap->data[bottom + left] * u1 + envMap->data[bottom + right] * u0) * (1 - v1);
  }


  Vector3D EnvironmentLight::sample_L(const Vector3D p, Vector3D* wi,
    double* distToLight,
    double* pdf) const {
    // TODO: Assignment 7 Part 3 Tasks 2 and 3 (step 4)
    // First implement uniform sphere sampling for the environment light
    // Later implement full importance sampling

    // Uniform
    // *wi = sampler_uniform_sphere.get_sample();
    // *distToLight = INF_D;
    // *pdf = 1.0 / (4.0 * PI);
    // return bilerp(theta_phi_to_xy(dir_to_theta_phi(*wi)));

    // Importance Lighting
    auto s = sampler_uniform2d.get_sample();
    auto y_ub = std::upper_bound(marginal_y, marginal_y+envMap->h, s.y);
    int index_y = std::distance(marginal_y, y_ub);
    auto xy_ub = std::upper_bound(conds_y+index_y*envMap->w, conds_y+(index_y+1)*envMap->w, s.x);
    int index_x = std::distance(conds_y + index_y * envMap->w, xy_ub);

    Vector2D xy(index_x, index_y);
    *distToLight = INF_D;

    *wi = theta_phi_to_dir(xy_to_theta_phi(xy));
    *pdf = pdf_envmap[(static_cast<int>(xy.y))*envMap->w + static_cast<int>(xy.x)] * envMap->w * envMap->h / (2 * M_PI * M_PI * sin(xy_to_theta_phi(xy).x));
    return envMap->data[(static_cast<int>(xy.y))*envMap->w + static_cast<int>(xy.x)];

    

  }

  Vector3D EnvironmentLight::sample_dir(const Ray& r) const {
    // TODO: Assignment 7 Part 3 Task 1
    // Use the helper functions to convert r.d into (x,y)
    // then bilerp the return value
    return bilerp(theta_phi_to_xy(dir_to_theta_phi(r.d)));
  }

} // namespace SceneObjects
} // namespace CGL
