#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include "math3d\math3d.h"
#include "math3d\objframe.h"

// Geometric primitive definitions

// A point is just a vector
typedef CVector3f Point3f;

typedef CVector2f Point2f;

// Triangle has 3 vertices and face normal
// Normal is assumed to be (v0v1) x (v0v2)
struct Triangle
{
	CVector3f Normal;
	Point3f Vertices[3];
};

// Plane defined by p * n = d
struct Plane
{
	CVector3f Normal;
	Point3f Point;
};

// A ray is a starting point and a unit direction vector
struct Ray
{
	Point3f		Origin;
	CVector3f	Direction;
};

// Sphere has center and radius
struct Sphere
{
	Point3f		Center;
	float			Radius;
};

// Line segment has 2 endpoints
struct LineSegment
{
	Point3f		Start;
	Point3f		End;
};

// Axis aligned bounding box has min and max points. Other vertices can be obtained
// from these easily
struct AABB
{
	Point3f		Min;
	Point3f		Max;
};

struct OBB
{
	CObjFrame *Frame;
	float x;		// Half lengths
	float y;
	float z;
};

struct FloatRect
{
   Point2f Min;
   Point2f Max;
};

#endif