#include <math.h>
#include "math3d\intersection.h"

#include "utils\mmgr.h"

float Distance(const Point3f *p1, const Point3f *p2)
{
	float dx = p1->x - p2->x;
	float dy = p1->y - p2->y;
	float dz = p1->z - p2->z;

	return (float)sqrt(dx * dx + dy * dy + dz * dz);
}

float DistanceSquare(const Point3f *p1, const Point3f *p2)
{
	float dx = p1->x - p2->x;
	float dy = p1->y - p2->y;
	float dz = p1->z - p2->z;

	return dx * dx + dy * dy + dz * dz;
}

bool SphereToSphere(const Sphere *sphere1, const Sphere *sphere2)
{
	float rsum = sphere1->Radius + sphere2->Radius;
	float CheckValue = DistanceSquare(&sphere1->Center, &sphere2->Center) - rsum * rsum;

	if (CheckValue < COLLISIONTOLERANCE)
		return true;
	else
		return false;
}

float PointToPlaneDistance(const Point3f *pt, const Plane *plane)
{
	return Vec3fDotProduct(pt, &plane->Normal) - Vec3fDotProduct(&plane->Point, &plane->Normal);
}

float PointToPlaneDistance(const Point3f *pt, const Point3f *p0, const CVector3f *n)
{
	return Vec3fDotProduct(pt, n) - Vec3fDotProduct(p0, n);
}

bool OBBToOBB(const OBB *boxA, const OBB *boxB)
{
	// Axes of boxes
	CMatrix44 matA, matB;

	// Coefficient matrices
	CMatrix33 C;
	CMatrix33 AbsC;

	// Interval radii
	float R, R0, R1;

	// Temp variables
	CVector3f D, posA, posB, v0, v1;
	CQuaternion qA, qB;

	// Get the box axes
	boxA->Frame->GetOrientation(&qA);
	QuatToRotationMatrix(&matA, &qA);
	boxB->Frame->GetOrientation(&qB);
	QuatToRotationMatrix(&matB, &qB);

	// D vector is from center of A to center of B
	boxA->Frame->GetPosition(&posA);
	boxB->Frame->GetPosition(&posB);
	Vec3fSubtract(&D, &posB, &posA);

	// Test 15 possible separating axes

	// AX
	R0 = boxA->x;
	C.m00 = Vec3fDotProduct(&matA.right, &matB.right);
	AbsC.m00 = (float)fabs(C.m00);
	C.m01 = Vec3fDotProduct(&matA.right, &matB.up);
	AbsC.m01 = (float)fabs(C.m01);
	C.m02 = Vec3fDotProduct(&matA.right, &matB.at);
	AbsC.m02 = (float)fabs(C.m02);
	R1 = boxB->x * AbsC.m00 + boxB->y * AbsC.m01 + boxB->z * AbsC.m02;
	R = (float)fabs(Vec3fDotProduct(&matA.right, &D));
	if (R > R0 + R1)
		return false;

	// AY
	R0 = boxA->y;
	C.m10 = Vec3fDotProduct(&matA.up, &matB.right);
	AbsC.m10 = (float)fabs(C.m10);
	C.m11 = Vec3fDotProduct(&matA.up, &matB.up);
	AbsC.m11 = (float)fabs(C.m11);
	C.m12 = Vec3fDotProduct(&matA.up, &matB.at);
	AbsC.m12 = (float)fabs(C.m12);
	R1 = boxB->x * AbsC.m10 + boxB->y * AbsC.m11 + boxB->z * AbsC.m12;
	R = (float)fabs(Vec3fDotProduct(&matA.up, &D));
	if (R > R0 + R1)
		return false;

	// AZ
	R0 = boxA->z;
	C.m20 = Vec3fDotProduct(&matA.at, &matB.right);
	AbsC.m20 = (float)fabs(C.m20);
	C.m21 = Vec3fDotProduct(&matA.at, &matB.up);
	AbsC.m21 = (float)fabs(C.m21);
	C.m22 = Vec3fDotProduct(&matA.at, &matB.at);
	AbsC.m22 = (float)fabs(C.m22);
	R1 = boxB->x * AbsC.m20 + boxB->y * AbsC.m21 + boxB->z * AbsC.m22;
	R = (float)fabs(Vec3fDotProduct(&matA.at, &D));
	if (R > R0 + R1)
		return false;

	// BX
	R0 = boxA->x * AbsC.m00 + boxA->y * AbsC.m10 + boxA->z * AbsC.m20;
	R1 = boxB->x;
	R = (float)fabs(Vec3fDotProduct(&matB.right, &D));
	if (R > R0 + R1)
		return false;

	// BY
	R0 = boxA->x * AbsC.m01 + boxA->y * AbsC.m11 + boxA->z * AbsC.m21;
	R1 = boxB->y;
	R = (float)fabs(Vec3fDotProduct(&matB.up, &D));
	if (R > R0 + R1)
		return false;

	// BZ
	R0 = boxA->x * AbsC.m02 + boxA->y * AbsC.m12 + boxA->z * AbsC.m22;
	R1 = boxB->z;
	R = (float)fabs(Vec3fDotProduct(&matB.at, &D));
	if (R > R0 + R1)
		return false;

	// AX x BX
	R0 = boxA->y * AbsC.m20 + boxA->z * AbsC.m10;
	R1 = boxB->y * AbsC.m02 + boxB->z * AbsC.m01;
	Vec3fScale(&v0, &matA.at, AbsC.m10);
	Vec3fScale(&v1, &matA.up, AbsC.m20);
	R = (float)fabs(Vec3fDotProduct(&v0, &D) - Vec3fDotProduct(&v1, &D));
	if (R > R0 + R1)
		return false;

	// AX x BY
	R0 = boxA->y * AbsC.m21 + boxA->z * AbsC.m11;
	R1 = boxB->x * AbsC.m02 + boxB->z * AbsC.m00;
	Vec3fScale(&v0, &matA.at, AbsC.m11);
	Vec3fScale(&v1, &matA.up, AbsC.m21);
	R = (float)fabs(Vec3fDotProduct(&v0, &D) - Vec3fDotProduct(&v1, &D));
	if (R > R0 + R1)
		return false;

	// AX x BZ
	R0 = boxA->y * AbsC.m22 + boxA->z * AbsC.m12;
	R1 = boxB->x * AbsC.m01 + boxB->y * AbsC.m00;
	Vec3fScale(&v0, &matA.at, AbsC.m12);
	Vec3fScale(&v1, &matA.up, AbsC.m22);
	R = (float)fabs(Vec3fDotProduct(&v0, &D) - Vec3fDotProduct(&v1, &D));
	if (R > R0 + R1)
		return false;

	// AY x BX
	R0 = boxA->x * AbsC.m20 + boxA->z * AbsC.m00;
	R1 = boxB->y * AbsC.m12 + boxB->z * AbsC.m11;
	Vec3fScale(&v0, &matA.right, AbsC.m20);
	Vec3fScale(&v1, &matA.at, AbsC.m00);
	R = (float)fabs(Vec3fDotProduct(&v0, &D) - Vec3fDotProduct(&v1, &D));
	if (R > R0 + R1)
		return false;

	// AY x BY
	R0 = boxA->x * AbsC.m21 + boxA->z * AbsC.m01;
	R1 = boxB->x * AbsC.m12 + boxB->z * AbsC.m10;
	Vec3fScale(&v0, &matA.right, AbsC.m21);
	Vec3fScale(&v1, &matA.at, AbsC.m01);
	R = (float)fabs(Vec3fDotProduct(&v0, &D) - Vec3fDotProduct(&v1, &D));
	if (R > R0 + R1)
		return false;

	// AY x BZ
	R0 = boxA->x * AbsC.m22 + boxA->z * AbsC.m02;
	R1 = boxB->x * AbsC.m11 + boxB->y * AbsC.m10;
	Vec3fScale(&v0, &matA.right, AbsC.m22);
	Vec3fScale(&v1, &matA.at, AbsC.m02);
	R = (float)fabs(Vec3fDotProduct(&v0, &D) - Vec3fDotProduct(&v1, &D));
	if (R > R0 + R1)
		return false;

	// AZ x BX
	R0 = boxA->x * AbsC.m10 + boxA->y * AbsC.m00;
	R1 = boxB->y * AbsC.m22 + boxB->z * AbsC.m21;
	Vec3fScale(&v0, &matA.up, AbsC.m00);
	Vec3fScale(&v1, &matA.right, AbsC.m10);
	R = (float)fabs(Vec3fDotProduct(&v0, &D) - Vec3fDotProduct(&v1, &D));
	if (R > R0 + R1)
		return false;

	// AZ x BY
	R0 = boxA->x * AbsC.m11 + boxA->y * AbsC.m01;
	R1 = boxB->x * AbsC.m22 + boxB->z * AbsC.m20;
	Vec3fScale(&v0, &matA.up, AbsC.m01);
	Vec3fScale(&v1, &matA.right, AbsC.m11);
	R = (float)fabs(Vec3fDotProduct(&v0, &D) - Vec3fDotProduct(&v1, &D));
	if (R > R0 + R1)
		return false;

	// AZ x BZ
	R0 = boxA->x * AbsC.m12 + boxA->y * AbsC.m02;
	R1 = boxB->x * AbsC.m21 + boxB->y * AbsC.m20;
	Vec3fScale(&v0, &matA.up, AbsC.m02);
	Vec3fScale(&v1, &matA.right, AbsC.m12);
	R = (float)fabs(Vec3fDotProduct(&v0, &D) - Vec3fDotProduct(&v1, &D));
	if (R > R0 + R1)
		return false;

	return true;
}

void OBBToOBBContactPoint(CVector3f *pt, const OBB *box1, const OBB *box2)
{

}

void SphereToSphereContactPlane(Plane *plane, const Sphere *s1, const Sphere *s2)
{
	// Normal points from s2 to s1
	CVector3f s2s1;
	Vec3fSubtract(&s2s1, &s1->Center, &s2->Center);
	Vec3fNormalize(&plane->Normal, &s2s1);

	float scale = Distance(&s1->Center, &s2->Center) * s2->Radius / (s1->Radius * s2->Radius);
	Vec3fScale(&plane->Point, &plane->Normal, scale);
	Vec3fAdd(&plane->Point, &plane->Point, &s2->Center);
}

bool RayToSphere(const Ray *ray, const Sphere *sphere)
{
	CVector3f D, proj;

	// Check if ray origin is inside sphere
	if (DistanceSquare(&ray->Origin, &sphere->Center) <= sphere->Radius * sphere->Radius)
		return true;

	Vec3fSubtract(&D, &sphere->Center, &ray->Origin);
	float t = Vec3fDotProduct(&D, &ray->Direction);

	if (t <= 0.0f)
		return false;

	Vec3fScale(&proj, &ray->Direction, t);
	Vec3fAdd(&proj, &proj, &ray->Origin);
	if (DistanceSquare(&proj, &sphere->Center) <= sphere->Radius * sphere->Radius)
		return true;
	else
		return false;
}

bool RayToAABB(const Ray *ray, const AABB *box)
{
	// Check if ray origin inside box
	CVector3f pt = ray->Origin;

	if (pt.x < box->Min.x)
		pt.x = box->Min.x;
	else if (pt.x > box->Max.x)
		pt.x = box->Max.x;

	if (pt.y < box->Min.y)
		pt.y = box->Min.y;
	else if (pt.y > box->Max.y)
		pt.y = box->Max.y;

	if (pt.z < box->Min.z)
		pt.z = box->Min.z;
	else if (pt.z > box->Max.z)
		pt.z = box->Max.z;

	if (pt == ray->Origin)
		return true;

	// Reject as many planes as possible
	bool minX = true, maxX = true, minY = true, maxY = true, minZ = true, maxZ = true;

	if (ray->Direction.x == 0.0f)
	{
		minX = maxX = false;
	}
	else if (ray->Direction.x > 0.0f)
		maxX = false;
	else
		minX = false;

	if (ray->Direction.y == 0.0f)
	{
		minY = maxY = false;
	}
	else if (ray->Direction.y > 0.0f)
		maxY = false;
	else
		minY = false;

	if (ray->Direction.z == 0.0f)
	{
		minZ = maxZ = false;
	}
	else if (ray->Direction.z > 0.0f)
		maxZ = false;
	else
		minZ = false;

	float t, tx, ty, tz;

	// Test all planes not rejected, return true if one intersection is found
	// since we don't care about intersection point
	if (minX)
	{
		t = (box->Min.x - ray->Origin.x) / ray->Direction.x;	

		if (t > 0.0f)
		{
			ty = ray->Origin.y + ray->Direction.y * t;
			tz = ray->Origin.z + ray->Direction.z * t;

			if (ty >= box->Min.y && ty <= box->Max.y && tz >= box->Min.z && tz <= box->Max.z)
				return true;
		}
	}
	if (maxX)
	{
		t = (box->Max.x - ray->Origin.x) / ray->Direction.x;	

		if (t > 0.0f)
		{
			ty = ray->Origin.y + ray->Direction.y * t;
			tz = ray->Origin.z + ray->Direction.z * t;

			if (ty >= box->Min.y && ty <= box->Max.y && tz >= box->Min.z && tz <= box->Max.z)
				return true;
		}
	}
	if (minY)
	{
		t = (box->Min.y - ray->Origin.y) / ray->Direction.y;	

		if (t > 0.0f)
		{
			tx = ray->Origin.x + ray->Direction.x * t;
			tz = ray->Origin.z + ray->Direction.z * t;

			if (tx >= box->Min.x && tx <= box->Max.x && tz >= box->Min.z && tz <= box->Max.z)
				return true;
		}
	}
	if (maxY)
	{
		t = (box->Max.y - ray->Origin.y) / ray->Direction.y;	

		if (t > 0.0f)
		{
			tx = ray->Origin.x + ray->Direction.x * t;
			tz = ray->Origin.z + ray->Direction.z * t;

			if (tx >= box->Min.x && tx <= box->Max.x && tz >= box->Min.z && tz <= box->Max.z)
				return true;
		}
	}
	if (minZ)
	{
		t = (box->Min.z - ray->Origin.z) / ray->Direction.z;	

		if (t > 0.0f)
		{
			ty = ray->Origin.y + ray->Direction.y * t;
			tx = ray->Origin.x + ray->Direction.x * t;

			if (ty >= box->Min.y && ty <= box->Max.y && tx >= box->Min.x && tx <= box->Max.x)
				return true;
		}
	}
	if (maxZ)
	{
		t = (box->Max.z - ray->Origin.z) / ray->Direction.z;	

		if (t > 0.0f)
		{
			ty = ray->Origin.y + ray->Direction.y * t;
			tx = ray->Origin.x + ray->Direction.x * t;

			if (ty >= box->Min.y && ty <= box->Max.y && tx >= box->Min.x && tx <= box->Max.x)
				return true;
		}
	}

	return false;
}

bool RayToAABB(const Ray *ray, const AABB *box, CVector3f *n)
{
	// Check if ray origin inside box
	CVector3f pt = ray->Origin;

	Vec3fZero(n);

	if (pt.x < box->Min.x)
		pt.x = box->Min.x;
	else if (pt.x > box->Max.x)
		pt.x = box->Max.x;

	if (pt.y < box->Min.y)
		pt.y = box->Min.y;
	else if (pt.y > box->Max.y)
		pt.y = box->Max.y;

	if (pt.z < box->Min.z)
		pt.z = box->Min.z;
	else if (pt.z > box->Max.z)
		pt.z = box->Max.z;

	if (pt == ray->Origin)
		return true;

	// Reject as many planes as possible
	bool minX = true, maxX = true, minY = true, maxY = true, minZ = true, maxZ = true;

	if (ray->Direction.x == 0.0f)
	{
		minX = maxX = false;
	}
	else if (ray->Direction.x > 0.0f)
		maxX = false;
	else
		minX = false;

	if (ray->Direction.y == 0.0f)
	{
		minY = maxY = false;
	}
	else if (ray->Direction.y > 0.0f)
		maxY = false;
	else
		minY = false;

	if (ray->Direction.z == 0.0f)
	{
		minZ = maxZ = false;
	}
	else if (ray->Direction.z > 0.0f)
		maxZ = false;
	else
		minZ = false;

	float t, tx, ty, tz;

	// Test all planes not rejected, return true if one intersection is found
	// since we don't care about intersection point
	if (minX)
	{
		t = (box->Min.x - ray->Origin.x) / ray->Direction.x;	

		if (t > 0.0f)
		{
			ty = ray->Origin.y + ray->Direction.y * t;
			tz = ray->Origin.z + ray->Direction.z * t;

			if (ty >= box->Min.y && ty <= box->Max.y && tz >= box->Min.z && tz <= box->Max.z)
			{
				
				return true;
			}
		}
	}
	if (maxX)
	{
		t = (box->Max.x - ray->Origin.x) / ray->Direction.x;	

		if (t > 0.0f)
		{
			ty = ray->Origin.y + ray->Direction.y * t;
			tz = ray->Origin.z + ray->Direction.z * t;

			if (ty >= box->Min.y && ty <= box->Max.y && tz >= box->Min.z && tz <= box->Max.z)
				return true;
		}
	}
	if (minY)
	{
		t = (box->Min.y - ray->Origin.y) / ray->Direction.y;	

		if (t > 0.0f)
		{
			tx = ray->Origin.x + ray->Direction.x * t;
			tz = ray->Origin.z + ray->Direction.z * t;

			if (tx >= box->Min.x && tx <= box->Max.x && tz >= box->Min.z && tz <= box->Max.z)
				return true;
		}
	}
	if (maxY)
	{
		t = (box->Max.y - ray->Origin.y) / ray->Direction.y;	

		if (t > 0.0f)
		{
			tx = ray->Origin.x + ray->Direction.x * t;
			tz = ray->Origin.z + ray->Direction.z * t;

			if (tx >= box->Min.x && tx <= box->Max.x && tz >= box->Min.z && tz <= box->Max.z)
				return true;
		}
	}
	if (minZ)
	{
		t = (box->Min.z - ray->Origin.z) / ray->Direction.z;	

		if (t > 0.0f)
		{
			ty = ray->Origin.y + ray->Direction.y * t;
			tx = ray->Origin.x + ray->Direction.x * t;

			if (ty >= box->Min.y && ty <= box->Max.y && tx >= box->Min.x && tx <= box->Max.x)
				return true;
		}
	}
	if (maxZ)
	{
		t = (box->Max.z - ray->Origin.z) / ray->Direction.z;	

		if (t > 0.0f)
		{
			ty = ray->Origin.y + ray->Direction.y * t;
			tx = ray->Origin.x + ray->Direction.x * t;

			if (ty >= box->Min.y && ty <= box->Max.y && tx >= box->Min.x && tx <= box->Max.x)
				return true;
		}
	}

	return false;
}

float RayToTriangle(const Ray *ray, const Triangle *triangle)
{
	CVector3f E0, E1, pt, D, Q;
	float t, dot, s0, s1, delta;

	// Check if ray intersect plane
	dot = Vec3fDotProduct(&triangle->Normal, &ray->Direction);
	if (dot == 0.0f)
		return -1.0f;

	t = (Vec3fDotProduct(&triangle->Normal, &triangle->Vertices[0]) - Vec3fDotProduct(&ray->Origin, &triangle->Normal)) / dot;
	if (t < 0.0f)
		return -1.0f;

	// Check if point inside triangle using barycentric coordinates
	Vec3fSubtract(&E0, &triangle->Vertices[1], &triangle->Vertices[0]);
	Vec3fSubtract(&E1, &triangle->Vertices[2], &triangle->Vertices[0]);
	Vec3fScale(&D, &ray->Direction, t);
	Vec3fAdd(&pt, &D, &ray->Origin);
	Vec3fSubtract(&Q, &pt, &triangle->Vertices[0]);

	s0 = Vec3fDotProduct(&E1, &E1) * Vec3fDotProduct(&E0, &Q) - Vec3fDotProduct(&E0, &E1) * Vec3fDotProduct(&E1, &Q);
	s1 = Vec3fDotProduct(&E0, &E0) * Vec3fDotProduct(&E1, &Q) - Vec3fDotProduct(&E0, &E1) * Vec3fDotProduct(&E0, &Q);
	delta = Vec3fDotProduct(&E0, &E0) * Vec3fDotProduct(&E1, &E1) - Vec3fDotProduct(&E0, &E1) * Vec3fDotProduct(&E0, &E1);

	if (s0 >= 0.0f && s1 >= 0.0f && s0 + s1 <= delta)
		return t;
	else
		return -1.0f;
}

bool RectToPoint(const RECT *pRect, const POINT *pPoint)
{
   if (pPoint->x < pRect->left || pPoint->x > pRect->right || pPoint->y > pRect->bottom ||
       pPoint->y < pRect->top)
      return false;
   
   return true;
}

bool FloatRectToPoint(const FloatRect* pRect, const Point2f* pPoint)
{
   if (pPoint->x < pRect->Min.x || pPoint->x > pRect->Max.x || pPoint->y < pRect->Min.y ||
       pPoint->y > pRect->Max.y)
      return false;
   
   return true;
}

float ClampToPlane(const CVector3f *pt, const Plane *p, float offset)
{
	float d = Vec3fDotProduct(&p->Point, &p->Normal);
	CVector3f D(0.0f, -1.0f, 0.0f);
	float t = (d - Vec3fDotProduct(&p->Normal, pt)) / Vec3fDotProduct(&p->Normal, &D);

	return -t + offset;
}

void GetOrientationFromPoints(CQuaternion *q, const CVector3f *front, const CVector3f *backleft, const CVector3f *backright)
{
	CMatrix33 mat;
	CVector3f mid;
	CVector3f *basis = (CVector3f *)&mat;

	// X
	Vec3fSubtract(&basis[0], backleft, backright);
	Vec3fNormalize(&basis[0], &basis[0]);

	// Z
	Vec3fAdd(&mid, backleft, backright);
	Vec3fScale(&mid, &mid, 0.5f);
	Vec3fSubtract(&basis[2], front, &mid);
	Vec3fNormalize(&basis[2], &basis[2]);

	// Y
	Vec3fCrossProduct(&basis[1], &basis[2], &basis[0]);
	Vec3fNormalize(&basis[1], &basis[1]);

	Mat33Transpose(&mat, &mat);
	QuatFromRotationMatrix(q, &mat);
}
