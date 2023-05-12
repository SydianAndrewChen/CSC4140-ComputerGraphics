#include "pathtracer.h"

#include "scene/light.h"
#include "scene/sphere.h"
#include "scene/triangle.h"


using namespace CGL::SceneObjects;

namespace CGL {

PathTracer::PathTracer() {
  gridSampler = new UniformGridSampler2D();
  hemisphereSampler = new UniformHemisphereSampler3D();

  tm_gamma = 2.2f;
  tm_level = 1.0f;
  tm_key = 0.18;
  tm_wht = 5.0f;
}

PathTracer::~PathTracer() {
  delete gridSampler;
  delete hemisphereSampler;
}

void PathTracer::set_frame_size(size_t width, size_t height) {
  sampleBuffer.resize(width, height);
  sampleCountBuffer.resize(width * height);
}

void PathTracer::clear() {
  bvh = NULL;
  scene = NULL;
  camera = NULL;
  sampleBuffer.clear();
  sampleCountBuffer.clear();
  sampleBuffer.resize(0, 0);
  sampleCountBuffer.resize(0, 0);
}

void PathTracer::write_to_framebuffer(ImageBuffer &framebuffer, size_t x0,
                                      size_t y0, size_t x1, size_t y1) {
  sampleBuffer.toColor(framebuffer, x0, y0, x1, y1);
}

Vector3D
PathTracer::estimate_direct_lighting_hemisphere(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // For this function, sample uniformly in a hemisphere.

  // Note: When comparing Cornel Box (CBxxx.dae) results to importance sampling, you may find the "glow" around the light source is gone.
  // This is totally fine: the area lights in importance sampling has directionality, however in hemisphere sampling we don't model this behaviour.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);

  // This is the same number of total samples as
  // estimate_direct_lighting_importance (outside of delta lights). We keep the
  // same number of samples for clarity of comparison.
  int num_samples = scene->lights.size() * ns_area_light;
  Vector3D L_out;

  // TODO (Part 3): Write your sampling loop here
  // TODO BEFORE YOU BEGIN
  // UPDATE `est_radiance_global_illumination` to return direct lighting instead of normal shading 
  for (int i = 0; i < num_samples; ++i){
    auto w_in = hemisphereSampler->get_sample();
    Ray r_in(hit_p, o2w * w_in);
    r_in.min_t = EPS_D;
    Intersection isect_direct;
    if (bvh->intersect(r_in, &isect_direct)){
      double cosine = w_in.z;
      auto bsdf = isect.bsdf->f(w_out, w_in);
      auto L_in = isect_direct.bsdf->get_emission();
      // L_out += bsdf * L_in * fabs(cosine);
      L_out += bsdf * L_in;
    }
  }

  return L_out;

}

Vector3D
PathTracer::estimate_direct_lighting_importance(const Ray &r,
                                                const Intersection &isect) {
  // Estimate the lighting from this intersection coming directly from a light.
  // To implement importance sampling, sample only from lights, not uniformly in
  // a hemisphere.

  // make a coordinate system for a hit point
  // with N aligned with the Z direction.
  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  // w_out points towards the source of the ray (e.g.,
  // toward the camera if this is a primary ray)
  const Vector3D hit_p = r.o + r.d * isect.t;
  const Vector3D w_out = w2o * (-r.d);
  Vector3D L_out;

  for (auto light: scene->lights)
  {
    const int ns = light->is_delta_light() ? 1 : ns_area_light;
    for (int i = 0; i < ns; ++i){
      Vector3D w_in;
      double distToLight;
      double pdf;
      auto L_in = light->sample_L(hit_p, &w_in, &distToLight, &pdf);

      Ray r_in(hit_p, w_in);
      r_in.min_t = EPS_F;
      r_in.max_t = distToLight - EPS_F;
      Intersection isect_direct;
      auto w_in_obj = w2o * w_in;
      if (w_in_obj[2] >= 0 && !bvh->intersect(r_in, &isect_direct)){
        double cosine = dot(isect.n, w_in);
        auto bsdf = isect.bsdf->f(w_out, w_in_obj);
        // Attenuation
        // L_out += bsdf * L_in * fabs(cosine) / (pdf * ns);
        L_out += bsdf * L_in / (pdf * ns);
      }
    }
  }
  

  return L_out;

}

Vector3D PathTracer::zero_bounce_radiance(const Ray &r,
                                          const Intersection &isect) {
  // TODO: Part 3, Task 2
  // Returns the light that results from no bounces of light
  return isect.bsdf->get_emission();
}

Vector3D PathTracer::one_bounce_radiance(const Ray &r,
                                         const Intersection &isect) {
  // TODO: Part 3, Task 3
  // Returns either the direct illumination by hemisphere or importance sampling
  // depending on `direct_hemisphere_sample`

  // return  estimate_direct_lighting_hemisphere(r, isect);
  return direct_hemisphere_sample ? estimate_direct_lighting_hemisphere(r, isect) : estimate_direct_lighting_importance(r, isect);
}

Vector3D PathTracer::at_least_one_bounce_radiance(const Ray &r,
                                                  const Intersection &isect) {

  // TODO: Part 4, Task 2
  // Returns the one bounce radiance + radiance from extra bounces at this point.
  // Should be called recursively to simulate extra bounces.

  Vector3D L_out;
  double p = 0.3;
  if (r.depth == 0 || coin_flip(p)) { 
    return L_out;
  }
  if (!isect.bsdf->is_delta()) L_out += one_bounce_radiance(r, isect);

  Matrix3x3 o2w;
  make_coord_space(o2w, isect.n);
  Matrix3x3 w2o = o2w.T();

  Vector3D hit_p = r.o + r.d * isect.t;
  Vector3D w_out = w2o * (-r.d);

  Vector3D w_in;
  double pdf;
  Vector3D bsdf = isect.bsdf->sample_f(w_out, &w_in, &pdf);

  Ray r_in(hit_p, o2w * w_in);
  r_in.depth = r.depth - 1;
  r_in.min_t = EPS_F;
  Intersection isect_sample;

  if (bvh->intersect(r_in, &isect_sample)) {
    Vector3D L_bounce = at_least_one_bounce_radiance(r_in, isect_sample);
    if (isect.bsdf->is_delta()) L_bounce += zero_bounce_radiance(r_in, isect_sample);
    double cosine = w_in.z;
    L_out += L_bounce * bsdf * fabs(cosine) / (pdf*(1. - p));
  }

  return L_out;
}

Vector3D PathTracer::est_radiance_global_illumination(const Ray &r) {
  Intersection isect;
  Vector3D L_out;

  // You will extend this in assignment 3-2.
  // If no intersection occurs, we simply return black.
  // This changes if you implement hemispherical lighting for extra credit.

  // The following line of code returns a debug color depending
  // on whether ray intersection with triangles or spheres has
  // been implemented.
  //
  // REMOVE THIS LINE when you are ready to begin Part 3.
  
  if (!bvh->intersect(r, &isect))
    return envLight ? envLight->sample_dir(r) : L_out;


  // L_out = (isect.t == INF_D) ? debug_shading(r.d) : normal_shading(isect.n);

  // TODO (Part 3): Return the direct illumination.
  
  // L_out = (isect.t == INF_D) ? debug_shading(r.d) : zero_bounce_radiance(r, isect) + one_bounce_radiance(r, isect);

  // TODO (Part 4): Accumulate the "direct" and "indirect"
  // parts of global illumination into L_out rather than just direct
  
  L_out = (isect.t == INF_D) ? debug_shading(r.d) : zero_bounce_radiance(r, isect) + at_least_one_bounce_radiance(r, isect);
  
  return L_out;
}

void PathTracer::raytrace_pixel(size_t x, size_t y) {
  // TODO (Part 1.2):
  // Make a loop that generates num_samples camera rays and traces them
  // through the scene. Return the average Vector3D.
  // You should call est_radiance_global_illumination in this function.
  
  // int num_samples = ns_aa;          // total samples to evaluate
  // Vector2D origin = Vector2D(x, y); // bottom left corner of the pixel
  // Intersection isect;

  // Vector3D c;
  // for (int i = 1; i <= num_samples; i++) {
  //   auto s = origin + gridSampler->get_sample();
  //   Ray r = camera->generate_ray(s.x / sampleBuffer.w, s.y / sampleBuffer.h);
  //   r.depth = max_ray_depth;
  //   c += est_radiance_global_illumination(r);
  // }
  // // std::cout << "raytrace_pixel \t\t" << std::endl; 
  // // std::cout << "x \t\t" << x << std::endl; 
  // // std::cout << "y \t\t" << y << std::endl; 
  // c /= num_samples;
  // sampleBuffer.update_pixel(c, x, y);
  // sampleCountBuffer[x + y * sampleBuffer.w] = num_samples;

  // TODO (Part 5):
  // Modify your implementation to include adaptive sampling.
  // Use the command line parameters "samplesPerBatch" and "maxTolerance"

  int num_samples = ns_aa;          // total samples to evaluate
  Vector2D origin(x, y); // bottom left corner of the pixel
  Vector3D L, L_sum;

  double s_sum = 0.0;
  double s_square_sum = 0.0;

  int num_samples_adaptive;
  for (num_samples_adaptive = 1; num_samples_adaptive <= num_samples; num_samples_adaptive++) {
    Vector2D sample = origin + gridSampler->get_sample();

    Ray r = camera->generate_ray(sample[0] / sampleBuffer.w, sample[1] / sampleBuffer.h);
    r.depth = max_ray_depth;
    
    L = est_radiance_global_illumination(r);
    L_sum += L;

    double illuminance = L.illum();
    s_sum += illuminance;
    s_square_sum += illuminance * illuminance;


    if (num_samples_adaptive % samplesPerBatch == 0) {
      double sigma_square = (s_square_sum - s_sum * s_sum / num_samples_adaptive) * (1. / (num_samples_adaptive - 1));
      double u = s_sum / num_samples_adaptive;
      double I = 1.96 * sqrt(sigma_square) / sqrt(num_samples_adaptive);
      
      if (I <= maxTolerance * u) break;
    }
  }

  L = L_sum / num_samples_adaptive;

  sampleBuffer.update_pixel(L, x, y);
  sampleCountBuffer[x + y * sampleBuffer.w] = num_samples;


}

void PathTracer::autofocus(Vector2D loc) {
  Ray r = camera->generate_ray(loc.x / sampleBuffer.w, loc.y / sampleBuffer.h);
  Intersection isect;

  bvh->intersect(r, &isect);

  camera->focalDistance = isect.t;
}

} // namespace CGL
