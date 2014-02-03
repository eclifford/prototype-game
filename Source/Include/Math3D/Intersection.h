#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "math3d\math3d.h"
#include "math3d\quaternion.h"
#include "math3d\primitives.h"
#include <windows.h>

// Intersection tests and distance calculations
// All points must be in world space

// Distance calculation
float Distance(const Point3f *p1, const Point3f *p2);
float DistanceSquare(const Point3f *p1, const Point3f *p2);

#define COLLISIONTOLERANCE		0.1f

bool SphereToSphere(const Sphere *sphere1, const Sphere *sphere2);

float PointToPlaneDistance(const Point3f *pt, const Plane *plane);
float PointToPlaneDistance(const Point3f *pt, const Point3f *p0, const CVector3f *n);

bool OBBToOBB(const OBB *box1, const OBB *box2);
void OBBToOBBContactPoint(CVector3f *pt, const OBB *box1, const OBB *box2);
void SphereToSphereContactPlane(Plane *plane, const Sphere *s1, const Sphere *s2);

bool RayToSphere(const Ray *ray, const Sphere *sphere);
bool RayToAABB(const Ray *ray, const AABB *box);
bool RayToAABB(const Ray *ray, const AABB *box, CVector3f *n);
float RayToTriangle(const Ray *ray, const Triangle *triangle);

bool RectToPoint(const RECT* pRect, const POINT* pPoint);
bool FloatRectToPoint(const FloatRect* pRect, const Point2f* pPoint);

float ClampToPlane(const CVector3f *pt, const Plane *p, float offset);
void GetOrientationFromPoints(CQuaternion *q, const CVector3f *front, const CVector3f *backleft, const CVector3f *backright);

#endif