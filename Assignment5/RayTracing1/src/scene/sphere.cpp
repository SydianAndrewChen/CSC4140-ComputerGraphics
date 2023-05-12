#include "sphere.h"

#include <cmath>

#include "pathtracer/bsdf.h"
#include "util/sphere_drawing.h"

namespace CGL {
namespace SceneObjects {

bool Sphere::test(const Ray &r, double &t1, double &t2) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.

  auto a = 1;
  auto b = 2 * dot((r.o-o), r.d);
  auto c = dot((r.o-o), (r.o-o)) - r2;
  auto sqrt_term = b*b - 4*a*c;
  if (sqrt_term < 0) return false;
  t1 = (-b+sqrt_term)/(2*a);
  t2 = (-b-sqrt_term)/(2*a);
  
  return t1 > 0 || t2 > 0;

}

bool Sphere::has_intersection(const Ray &r) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
  double t1, t2;
  return test(r, t1, t2);
}

bool Sphere::intersect(const Ray &r, Intersection *i) const {

  // TODO (Part 1.4):
  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.
  double t1, t2;
  if (!test(r, t1, t2)) return false;
  auto t = min(t1, t2);
  if (t > r.min_t && t < r.max_t){
    r.max_t = t;  
    i->t = t;
    i->n = (r.at_time(i->t) - o).unit();
    i->bsdf = get_bsdf();
    i->primitive = this;
    return true;
  } 
  return false;
}

void Sphere::draw(const Color &c, float alpha) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color &c, float alpha) const {
  // Misc::draw_sphere_opengl(o, r, c);
}

} // namespace SceneObjects
} // namespace CGL
