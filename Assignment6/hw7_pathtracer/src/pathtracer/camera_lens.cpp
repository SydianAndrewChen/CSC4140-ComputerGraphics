#include "camera.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "CGL/misc.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"

using std::cout;
using std::endl;
using std::max;
using std::min;
using std::ifstream;
using std::ofstream;

namespace CGL {

using Collada::CameraInfo;

Ray Camera::generate_ray_for_thin_lens(double x, double y, double rndR, double rndTheta) const {

  // TODO Assignment 7: Part 4
  // compute position and direction of ray from the input sensor sample coordinate.
  // Note: use rndR and rndTheta to uniformly sample a unit disk.

  // Calculate half dimensions
  const double halfWidth = std::tan(hFov * M_PI / 360.0);
  const double halfHeight = std::tan(vFov * M_PI / 360.0);

  // Calculate sensor coordinates
  const double xCoord = (2.0 * x - 1.0) * halfWidth;
  const double yCoord = (2.0 * y - 1.0) * halfHeight;
  const Vector3D sensorCoord(xCoord, yCoord, -1.0);

  // Transform sensor coordinates to world coordinates
  Vector3D d(xCoord, yCoord, -1);


  double t = -focalDistance / d.z;
  auto pFocus = t*d;
  Vector3D pLens(lensRadius*sqrt(rndR)*cos(rndTheta), lensRadius*sqrt(rndR)*sin(rndTheta), 0);
  auto dirFocus = (pFocus - pLens).unit();
  Ray rFocus(c2w*pLens+pos, c2w*dirFocus);
  rFocus.min_t = nClip;
  rFocus.max_t = fClip;
  
  return rFocus;
}


} // namespace CGL
