#include "triangle.h"

#include "CGL/CGL.h"
#include "GL/glew.h"

namespace CGL {
namespace SceneObjects {

Triangle::Triangle(const Mesh *mesh, size_t v1, size_t v2, size_t v3) {
  p1 = mesh->positions[v1];
  p2 = mesh->positions[v2];
  p3 = mesh->positions[v3];
  n1 = mesh->normals[v1];
  n2 = mesh->normals[v2];
  n3 = mesh->normals[v3];
  bbox = BBox(p1);
  bbox.expand(p2);
  bbox.expand(p3);

  bsdf = mesh->get_bsdf();
}

BBox Triangle::get_bbox() const { return bbox; }

bool Triangle::has_intersection(const Ray &r) const {
  // Part 1, Task 3: implement ray-triangle intersection
  // The difference between this function and the next function is that the next
  // function records the "intersection" while this function only tests whether
  // there is a intersection.
  
  // intersection of ray and the triangle plane
  auto N = cross(p2-p1, p3-p1);
  if (fabs(dot(r.d, N)) < EPS_D) return false;
  double t = dot(p1 - r.o, n1) / dot(r.d, n1);

  if (r.min_t < t && t < r.max_t){
    auto N = cross(p3-p1, p2-p1);
    // auto A = cross(p3-p1, p2-p1).norm();
    auto alpha = dot(cross(r.at_time(t) - p3, p1-p3), N) / N.norm2();
    auto beta = dot(cross(r.at_time(t) - p2, p1-p2), N) / N.norm2();
    if (alpha > 0 && beta > 0 && 1-alpha-beta>0){
      r.max_t = t;
      return true;
    }
  }
  return false;

}

bool Triangle::intersect(const Ray &r, Intersection *isect) const {
  // Part 1, Task 3:
  // implement ray-triangle intersection. When an intersection takes
  // place, the Intersection data should be updated accordingly
  
  auto N = cross(p2-p1, p3-p1);
  if (fabs(dot(r.d, N)) < EPS_D) return false;
  double t = dot(p1 - r.o, N) / dot(r.d, N);

  if (r.min_t < t && t < r.max_t){
    auto alpha = dot(cross(p3-p2, r.at_time(t) - p2), N) / N.norm2();
    auto beta = dot(cross(p1-p3, r.at_time(t) - p1), N) / N.norm2();
    // if (alpha < 0 || beta < 0 || 1.-alpha-beta < 0) return false;
    if (alpha > 0 && beta > 0 && 1.-alpha-beta > 0){

      r.max_t = t;
      isect->t = t;
      isect->bsdf = get_bsdf();
      isect->primitive = this;

      isect->n = (alpha * n1 +  beta * n2 + (1-alpha-beta) * n3).unit();
      // isect->n = Vector3D(alpha,  beta, (1-alpha-beta)).unit();
      return true;
    }
  }
  return false;


    // // compute the plane's normal
    // Vector3D v0v1 = p2 - p1;
    // Vector3D v0v2 = p3 - p1;
    // // no need to normalize
    // Vector3D N = cross(v0v1, v0v2); // N
    // float denom = N.norm2();
    
    // // Step 1: finding P
    
    // // check if the ray and plane are parallel.
    // float NdotRayDirection = dot(N, r.d);
    // if (fabs(NdotRayDirection) < EPS_D) // almost 0
    //     return false; // they are parallel so they don't intersect! 

    // // compute d parameter using equation 2
    // float d = -dot(N, p1);
    
    // // compute t (equation 3)
    // float t = -(dot(N, r.o) + d) / NdotRayDirection;
    // // check if the triangle is behind the ray
    // if (t < 0) return false; // the triangle is behind
 
    // // compute the intersection point using equation 1
    // Vector3D P = r.o + t * r.d;
 
    // // Step 2: inside-outside test
    // Vector3D C; // vector perpendicular to triangle's plane
    // float u,v;
    // // edge 0
    // Vector3D edge0 = p2 - p1; 
    // Vector3D vp0 = P - p1;
    // C = cross(edge0, vp0);
    // if (dot(N, C) < 0) return false; // P is on the right side
 
    // // edge 1
    // Vector3D edge1 = p3 - p2; 
    // Vector3D vp1 = P - p2;
    // C = cross(edge1, vp1);
    // if ((u = dot(N, C)) < 0)  return false; // P is on the right side
 
    // // edge 2
    // Vector3D edge2 = p1 - p3; 
    // Vector3D vp2 = P - p3;
    // C = cross(edge2, vp2);
    // if ((v = dot(N, C)) < 0) return false; // P is on the right side;

    // u /= denom;
    // v /= denom;

    // isect->bsdf = get_bsdf();
    // isect->n = u * n1 + v * n2 + (1.-u-v)*n3;
    // r.max_t = t;
    // isect->t = t;
    // isect->primitive = this;

    // return true; // this ray hits the triangle
}

void Triangle::draw(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_TRIANGLES);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

void Triangle::drawOutline(const Color &c, float alpha) const {
  glColor4f(c.r, c.g, c.b, alpha);
  glBegin(GL_LINE_LOOP);
  glVertex3d(p1.x, p1.y, p1.z);
  glVertex3d(p2.x, p2.y, p2.z);
  glVertex3d(p3.x, p3.y, p3.z);
  glEnd();
}

} // namespace SceneObjects
} // namespace CGL
