#include "bsdf.h"

#include <algorithm>
#include <iostream>
#include <utility>

#include "application/visual_debugger.h"

using std::max;
using std::min;
using std::swap;

namespace CGL {

// Mirror BSDF //

Vector3D MirrorBSDF::f(const Vector3D wo, const Vector3D wi) {
  return Vector3D();
}

Vector3D MirrorBSDF::sample_f(const Vector3D wo, Vector3D* wi, double* pdf) {

  // TODO:
  // Implement MirrorBSDF
  reflect(wo, wi);
  *pdf = 1; 
  return reflectance / abs_cos_theta(*wi); // Perfect mirror reflection do not cause Lambertian falloff
}

void MirrorBSDF::render_debugger_node()
{
  if (ImGui::TreeNode(this, "Mirror BSDF"))
  {
    DragDouble3("Reflectance", &reflectance[0], 0.005);
    ImGui::TreePop();
  }
}

// Microfacet BSDF //

double MicrofacetBSDF::G(const Vector3D wo, const Vector3D wi) {
  return 1.0 / (1.0 + Lambda(wi) + Lambda(wo));
}

double MicrofacetBSDF::D(const Vector3D h) {
  // TODO: proj3-2, part 3
  // Compute Beckmann normal distribution function (NDF) here.
  // You will need the roughness alpha.
  
  return 1.0;
}

Vector3D MicrofacetBSDF::F(const Vector3D wi) {
  // TODO: proj3-2, part 3
  // Compute Fresnel term for reflection on dielectric-conductor interface.
  // You will need both eta and etaK, both of which are Vector3D.

  double cosTheta = cos_theta(wi);
  
  return Vector3D();
}

Vector3D MicrofacetBSDF::f(const Vector3D wo, const Vector3D wi) {
  // TODO: proj3-2, part 3
  // Implement microfacet model here.

  return Vector3D();
}

Vector3D MicrofacetBSDF::sample_f(const Vector3D wo, Vector3D* wi, double* pdf) {
  // TODO: proj3-2, part 3
  // *Importance* sample Beckmann normal distribution function (NDF) here.
  // Note: You should fill in the sampled direction *wi and the corresponding *pdf,
  //       and return the sampled BRDF value.



  *wi = cosineHemisphereSampler.get_sample(pdf);

  return MicrofacetBSDF::f(wo, *wi);
}

void MicrofacetBSDF::render_debugger_node()
{
  if (ImGui::TreeNode(this, "Micofacet BSDF"))
  {
    DragDouble3("eta", &eta[0], 0.005);
    DragDouble3("K", &k[0], 0.005);
    DragDouble("alpha", &alpha, 0.005);
    ImGui::TreePop();
  }
}

// Refraction BSDF //

Vector3D RefractionBSDF::f(const Vector3D wo, const Vector3D wi) {
  return Vector3D();
}

Vector3D RefractionBSDF::sample_f(const Vector3D wo, Vector3D* wi, double* pdf) {
  // return Vector3D(1, 0, 0);
  // TODO:
  // Implement RefractionBSDF

  *pdf = 1;
  if (!refract(wo, wi, ior)) return Vector3D();

  bool is_entering = (wo.z > 0);
  auto eta = (is_entering ? (1. / ior) : ior);
  return transmittance / abs_cos_theta(*wi) / (eta * eta);
}

void RefractionBSDF::render_debugger_node()
{
  if (ImGui::TreeNode(this, "Refraction BSDF"))
  {
    DragDouble3("Transmittance", &transmittance[0], 0.005);
    DragDouble("ior", &ior, 0.005);
    ImGui::TreePop();
  }
}

// Glass BSDF //

Vector3D GlassBSDF::f(const Vector3D wo, const Vector3D wi) {
  return Vector3D();
}

Vector3D GlassBSDF::sample_f(const Vector3D wo, Vector3D* wi, double* pdf) {

  // TODO:
  // Compute Fresnel coefficient and either reflect or refract based on it.

  // compute Fresnel coefficient and use it as the probability of reflection
  // - Fundamentals of Computer Graphics page 305
  // if (!refract(wo, wi, ior)){
  //   reflect(wo, wi);
  //   *pdf = 1; 
  //   return reflectance / abs_cos_theta(*wi); // Perfect mirror reflection do not cause Lambertian falloff
  // }
  // else{
  //   bool is_entering = (wo.z > 0);
  //   auto eta = (is_entering ? (1. / ior) : ior);
  //   double R0 = pow((1-eta) / (1+eta), 2);
  //   double R = R0 + (1-R0) * pow(1-wo.z, 5.);
  //   if (coin_flip(R)){
  //     reflect(wo, wi);
  //     *pdf = R;
  //     return R * reflectance / abs_cos_theta(*wi);
  //   }
  //   else{
  //     refract(wo, wi, ior);
  //     *pdf = 1.-R;

  //     return (1-R) * transmittance / abs_cos_theta(*wi) / (eta * eta);
  //   }
  // }

  return Vector3D();
}

void GlassBSDF::render_debugger_node()
{
  if (ImGui::TreeNode(this, "Refraction BSDF"))
  {
    DragDouble3("Reflectance", &reflectance[0], 0.005);
    DragDouble3("Transmittance", &transmittance[0], 0.005);
    DragDouble("ior", &ior, 0.005);
    ImGui::TreePop();
  }
}

void BSDF::reflect(const Vector3D wo, Vector3D* wi) {

  // TODO:
  // Implement reflection of wo about normal (0,0,1) and store result in wi.
  *wi = wo;
  (*wi).x *= -1;
  (*wi).y *= -1;
}

bool BSDF::refract(const Vector3D wo, Vector3D* wi, double ior) {

  // TODO:
  // Use Snell's Law to refract wo surface and store result ray in wi.
  // Return false if refraction does not occur due to total internal reflection
  // and true otherwise. When dot(wo,n) is positive, then wo corresponds to a
  // ray entering the surface through vacuum.

  bool is_entering = (wo.z > 0);
  auto eta = (is_entering ? (1./ior):ior);
  auto new_z_square = 1-eta*eta*(1-wo.z*wo.z);
  // if (wo.z > 0 && new_z_square <= EPS_F) return false; 
  if (new_z_square <= EPS_F) return false; 
  (*wi).x = -eta*wo.x;
  (*wi).y = -eta*wo.y;
  (*wi).z = (!is_entering - is_entering) * sqrt(new_z_square);
  // (*wi).z = (is_entering?-1:1) * sqrt(new_z_square);

  return true;

}

} // namespace CGL
