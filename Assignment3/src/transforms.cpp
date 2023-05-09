#include "transforms.h"

#include "CGL/matrix3x3.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"

namespace CGL {

Vector2D operator*(const Matrix3x3 &m, const Vector2D &v) {
	Vector3D mv = m * Vector3D(v.x, v.y, 1);
	return Vector2D(mv.x / mv.z, mv.y / mv.z);
}

Matrix3x3 translate(float dx, float dy) {
	// Part 3: Fill this in.
	auto t = Matrix3x3::identity();
	t[2][0] = dx;
	t[2][1] = dy;
	return t;
}

Matrix3x3 scale(float sx, float sy) {
	// Part 3: Fill this in.
	auto s = Matrix3x3::identity();
	s[0][0] = sx;
	s[1][1] = sy;
	return s;
}

// The input argument is in degrees counterclockwise
Matrix3x3 rotate(float deg) {
	// Part 3: Fill this in.
	auto r = Matrix3x3::identity();
	r[0][0] = cos(radians(deg));
	r[1][0] = -sin(radians(deg));
	r[0][1] = sin(radians(deg));
	r[1][1] = cos(radians(deg));
	return r;
}

}
