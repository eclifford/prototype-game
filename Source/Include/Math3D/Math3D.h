/// \file Math3d.h
/// Declaration of vector, matrix, and quaternion classes
//////////////////////////////////////////////////////////////////////////

#pragma once

//#include <math.h>

#define PI 3.141592f

/// Degree conversion
#define DEGREE_TO_RADIAN(_x) ((_x) * PI / 180.0f)
#define RADIAN_TO_DEGREE(_x) ((_x) * 180.0f / PI)

class CVector2f
{
public:
   CVector2f() {}
   float x;
   float y;

   bool operator==(const CVector2f &v2)
	{
		return x == v2.x && y == v2.y;
	}
};

/// 3D vector class
class CVector3f
{
public:
	float x;
	float y;
	float z;

	/// Constructors
	CVector3f(void) {}
	CVector3f(float a, float b, float c)
	{
		x = a;
		y = b;
		z = c;
	}

	const CVector3f &operator=(const CVector3f &v2)
	{
		x = v2.x;
		y = v2.y;
		z = v2.z;

		return *this;
	}

	bool operator==(const CVector3f &v2)
	{
		return x == v2.x && y == v2.y && z == v2.z;
	}

	bool operator!=(const CVector3f &v2)
	{
		return x != v2.x || y != v2.y || z != v2.z;
	}
};

//////////////////////////////////////////////////////////////////////////
/// Find magnitude^2, saves the square root
//////////////////////////////////////////////////////////////////////////
float Vec3fMagnitudeSquare(const CVector3f *v);

//////////////////////////////////////////////////////////////////////////
/// Find magnitude
//////////////////////////////////////////////////////////////////////////
float Vec3fMagnitude(const CVector3f *v);

//////////////////////////////////////////////////////////////////////////
/// Makes it unit length
//////////////////////////////////////////////////////////////////////////
void Vec3fNormalize(CVector3f *result, const CVector3f *v);

//////////////////////////////////////////////////////////////////////////
/// Dot product
//////////////////////////////////////////////////////////////////////////
float Vec3fDotProduct(const CVector3f *v1, const CVector3f *v2); 

//////////////////////////////////////////////////////////////////////////
/// Cross product
//////////////////////////////////////////////////////////////////////////
void Vec3fCrossProduct(CVector3f *result, const CVector3f *v1, const CVector3f *v2);

//////////////////////////////////////////////////////////////////////////
/// Addition
//////////////////////////////////////////////////////////////////////////
void Vec3fAdd(CVector3f *result, const CVector3f *v1, const CVector3f *v2);

//////////////////////////////////////////////////////////////////////////
/// Subtraction
//////////////////////////////////////////////////////////////////////////
void Vec3fSubtract(CVector3f *result, const CVector3f *v1, const CVector3f *v2);

//////////////////////////////////////////////////////////////////////////
/// Scalar multiply
//////////////////////////////////////////////////////////////////////////
void Vec3fScale(CVector3f *result, const CVector3f *v, float k);

//////////////////////////////////////////////////////////////////////////
/// Negation
//////////////////////////////////////////////////////////////////////////
void Vec3fNegate(CVector3f *result, const CVector3f *v);

//////////////////////////////////////////////////////////////////////////
/// Projection
//////////////////////////////////////////////////////////////////////////
void Vec3fProject(CVector3f *result, const CVector3f *v, const CVector3f *n);

//////////////////////////////////////////////////////////////////////////
/// Reflection
//////////////////////////////////////////////////////////////////////////
void Vec3fReflect(CVector3f *result, const CVector3f *v, const CVector3f *n);

void Vec3fZero(CVector3f *v);

// 4x4 Matrix column major, specifically for transformation
struct CMatrix44
{
	CVector3f	right;
	float			m40;		// Always 0
	CVector3f	up;
	float			m41;		// Always 0
	CVector3f	at;
	float			m42;		// Always 0
	CVector3f	pos;
	float			m43;		// Always 1
};

void Mat44LoadIdentity(CMatrix44 *mat);
void Mat44Transpose(CMatrix44 *result, const CMatrix44 *mat);
void Mat44Multiply(CMatrix44 *result, const CMatrix44 *mat1, const CMatrix44 *mat2);
void Mat44MultiplyVector(CVector3f *result, const CMatrix44 *mat, const CVector3f *v);

// 3x3 Matrix
struct CMatrix33
{
	float m00, m10, m20;
	float m01, m11, m21;
	float m02, m12, m22;
};

void Mat33LoadIdentity(CMatrix33 *mat);
float Mat33Determinant(const CMatrix33 *mat);
void Mat33Inverse(CMatrix33 *result, const CMatrix33 *mat);
void Mat33Transpose(CMatrix33 *result, const CMatrix33 *mat);
void Mat33Multiply(CMatrix33 *result, const CMatrix33 *mat1, const CMatrix33 *mat2);
void Mat33MultiplyVector(CVector3f *result, const CMatrix33 *mat, const CVector3f *v);

void Mat44ToMat33(CMatrix33 *result, const CMatrix44 *mat);

// Cubic spline
void SplineGetControlPoint(CVector3f *result, const CVector3f *pti_minus_one, const CVector3f *pti_plus_one);
void SplineEvaluate(CVector3f *result, const CVector3f *p0, const CVector3f *p1, const CVector3f *m0, const CVector3f *m1, float h);