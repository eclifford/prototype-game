#include <memory.h>
#include <math.h>
#include "math3d\math3d.h"

#include "utils\mmgr.h"

//////////////////////////////////////////////////////////////////////////
/// Find magnitude^2, saves the square root
//////////////////////////////////////////////////////////////////////////
float Vec3fMagnitudeSquare(const CVector3f *v)
{
	return v->x * v->x + v->y * v->y + v->z * v->z;
}

//////////////////////////////////////////////////////////////////////////
/// Find magnitude
//////////////////////////////////////////////////////////////////////////
float Vec3fMagnitude(const CVector3f *v)
{
	return (float)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

//////////////////////////////////////////////////////////////////////////
/// Makes it unit length
//////////////////////////////////////////////////////////////////////////
void Vec3fNormalize(CVector3f *result, const CVector3f *v) 
{
	if (v->x != 0.0f || v->y != 0.0f || v->z != 0.0f)
	{
		float factor = 1.0f / (float)(sqrt(v->x * v->x + v->y * v->y + v->z * v->z));
		result->x = v->x * factor;
		result->y = v->y * factor;
		result->z = v->z * factor;
	}
	else
		Vec3fZero(result);
}

//////////////////////////////////////////////////////////////////////////
/// Dot product
//////////////////////////////////////////////////////////////////////////
float Vec3fDotProduct(const CVector3f *v1, const CVector3f *v2) 
{
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

//////////////////////////////////////////////////////////////////////////
/// Cross product
//////////////////////////////////////////////////////////////////////////
void Vec3fCrossProduct(CVector3f *result, const CVector3f *v1, const CVector3f *v2)
{
	CVector3f temp;

	temp.x = v1->y * v2->z - v2->y * v1->z;
	temp.y = -v1->x * v2->z + v2->x * v1->z;
	temp.z = v1->x * v2->y - v2->x * v1->y;

	*result = temp;
}

//////////////////////////////////////////////////////////////////////////
/// Addition
//////////////////////////////////////////////////////////////////////////
void Vec3fAdd(CVector3f *result, const CVector3f *v1, const CVector3f *v2)
{
	result->x = v1->x + v2->x;
	result->y = v1->y + v2->y;
	result->z = v1->z + v2->z;
}

//////////////////////////////////////////////////////////////////////////
/// Subtraction
//////////////////////////////////////////////////////////////////////////
void Vec3fSubtract(CVector3f *result, const CVector3f *v1, const CVector3f *v2)
{
	result->x = v1->x - v2->x;
	result->y = v1->y - v2->y;
	result->z = v1->z - v2->z;
}

//////////////////////////////////////////////////////////////////////////
/// Scalar multiply
//////////////////////////////////////////////////////////////////////////
void Vec3fScale(CVector3f *result, const CVector3f *v, float k)
{
	result->x = v->x * k;
	result->y = v->y * k;
	result->z = v->z * k;
}

//////////////////////////////////////////////////////////////////////////
/// Negation
//////////////////////////////////////////////////////////////////////////
void Vec3fNegate(CVector3f *result, const CVector3f *v)
{
	result->x = -v->x;
	result->y = -v->y;
	result->z = -v->z;
}

void Vec3fZero(CVector3f *v)
{
	v->x = v->y = v->z = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
/// Projection
//////////////////////////////////////////////////////////////////////////
void Vec3fProject(CVector3f *result, const CVector3f *v, const CVector3f *n)
{
	Vec3fScale(result, n, Vec3fDotProduct(v, n));
}

//////////////////////////////////////////////////////////////////////////
/// Reflection
//////////////////////////////////////////////////////////////////////////
void Vec3fReflect(CVector3f *result, const CVector3f *v, const CVector3f *n)
{
	Vec3fProject(result, v, n);
	Vec3fScale(result, result, 2.0f);
	Vec3fSubtract(result, result, v);
}

///////////////////////////////////
/// Matrix
///////////////////////////////////
void Mat44LoadIdentity(CMatrix44 *mat)
{
	memset((float *)mat, 0, sizeof(float) << 4);
	mat->right.x = mat->up.y = mat->at.z = mat->m43 = 1.0f;
}

void Mat44Transpose(CMatrix44 *result, const CMatrix44 *mat)
{
	CMatrix44 temp;

	temp.right.x = mat->right.x;
	temp.right.y = mat->up.x;
	temp.right.z = mat->at.x;
	temp.m40 = mat->pos.x;

	temp.up.x = mat->right.y;
	temp.up.y = mat->up.y;
	temp.up.z = mat->at.y;
	temp.m41 = mat->pos.y;

	temp.at.x = mat->right.z;
	temp.at.y = mat->up.z;
	temp.at.z = mat->at.z;
	temp.m42 = mat->pos.z;

	temp.pos.x = mat->m40;
	temp.pos.y = mat->m41;
	temp.pos.z = mat->m42;
	temp.m43 = mat->m43;

	memcpy(result, &temp, sizeof(float) << 4);
}

void Mat44Multiply(CMatrix44 *result, const CMatrix44 *mat1, const CMatrix44 *mat2)
{
	CMatrix44 temp;

	temp.right.x = mat1->right.x * mat2->right.x + mat1->up.x * mat2->right.y + mat1->at.x * mat2->right.z;
	temp.right.y = mat1->right.y * mat2->right.x + mat1->up.y * mat2->right.y + mat1->at.y * mat2->right.z;
	temp.right.z = mat1->right.z * mat2->right.x + mat1->up.z * mat2->right.y + mat1->at.z * mat2->right.z;

	temp.up.x = mat1->right.x * mat2->up.x + mat1->up.x * mat2->up.y + mat1->at.x * mat2->up.z;
	temp.up.y = mat1->right.y * mat2->up.x + mat1->up.y * mat2->up.y + mat1->at.y * mat2->up.z;
	temp.up.z = mat1->right.z * mat2->up.x + mat1->up.z * mat2->up.y + mat1->at.z * mat2->up.z;

	temp.at.x = mat1->right.x * mat2->at.x + mat1->up.x * mat2->at.y + mat1->at.x * mat2->at.z;
	temp.at.y = mat1->right.y * mat2->at.x + mat1->up.y * mat2->at.y + mat1->at.y * mat2->at.z;
	temp.at.z = mat1->right.z * mat2->at.x + mat1->up.z * mat2->at.y + mat1->at.z * mat2->at.z;

	temp.pos.x = mat1->right.x * mat2->pos.x + mat1->up.x * mat2->pos.y + mat1->at.x * mat2->pos.z;
	temp.pos.y = mat1->right.y * mat2->pos.x + mat1->up.y * mat2->pos.y + mat1->at.y * mat2->pos.z;
	temp.pos.z = mat1->right.z * mat2->pos.x + mat1->up.z * mat2->pos.y + mat1->at.z * mat2->pos.z;

	temp.m40 = temp.m41 = temp.m42 = 0.0f;
	temp.m43 = 1.0f;

	memcpy(result, &temp, sizeof(float) << 4);
}

void Mat44MultiplyVector(CVector3f *result, const CMatrix44 *mat, const CVector3f *v)
{
	CVector3f temp;
	temp.x = mat->right.x * v->x + mat->up.x * v->y + mat->at.x * v->z + mat->pos.x;
	temp.y = mat->right.y * v->x + mat->up.y * v->y + mat->at.y * v->z + mat->pos.y;
	temp.z = mat->right.z * v->x + mat->up.z * v->y + mat->at.z * v->z + mat->pos.z;
	*result = temp;
}

void Mat33LoadIdentity(CMatrix33 *mat)
{
	mat->m00 = mat->m11 = mat->m22 = 1.0f;
	mat->m01 = mat->m02 = mat->m10 = mat->m12 = mat->m20 = mat->m21 = 0.0f;
}

float Mat33Determinant(const CMatrix33 *mat)
{
	return mat->m00 * (mat->m11 * mat->m22 - mat->m12 * mat->m21) 
		  + mat->m01 * (mat->m12 * mat->m20 - mat->m10 * mat->m22) 
		  + mat->m02 * (mat->m10 * mat->m21 - mat->m11 * mat->m20);
}

void Mat33Inverse(CMatrix33 *result, const CMatrix33 *mat)
{
	float m00, m01, m02;
	float m10, m11, m12;
	float m20, m21, m22;

	float d = Mat33Determinant(mat);
	if (d == 0.0f)
		d = 1.0f;

	m00 = (mat->m11 * mat->m22 - mat->m21 * mat->m12) / d;
	m01 = -(mat->m10 * mat->m22 - mat->m20 * mat->m12) / d;
	m02 = (mat->m10 * mat->m21 - mat->m20 * mat->m11) / d;
	m10 = -(mat->m01 * mat->m22 - mat->m21 * mat->m02) / d;
	m11 = (mat->m00 * mat->m22 - mat->m20 * mat->m02) / d;
	m12 = -(mat->m00 * mat->m21 - mat->m20 * mat->m01) / d;
	m20 = (mat->m01 * mat->m12 - mat->m11 * mat->m02) / d;
	m21 = -(mat->m00 * mat->m12 - mat->m10 * mat->m02) / d;
	m22 = (mat->m00 * mat->m11 - mat->m10 * mat->m01) / d;

	result->m00 = m00;
	result->m01 = m01;
	result->m02 = m02;
	result->m10 = m10;
	result->m11 = m11;
	result->m12 = m12;
	result->m20 = m20;
	result->m21 = m21;
	result->m22 = m22;
}

void Mat33Transpose(CMatrix33 *result, const CMatrix33 *mat)
{
	CMatrix33 temp;

	temp.m00 = mat->m00;
	temp.m01 = mat->m10;
	temp.m02 = mat->m20;
	temp.m10 = mat->m01;
	temp.m11 = mat->m11;
	temp.m12 = mat->m21;
	temp.m20 = mat->m02;
	temp.m21 = mat->m12;
	temp.m22 = mat->m22;

	memcpy(result, &temp, sizeof(CMatrix33));
}

void Mat33Multiply(CMatrix33 *result, const CMatrix33 *mat1, const CMatrix33 *mat2)
{
	CMatrix33 temp;

	temp.m00 = mat1->m00 * mat2->m00 + mat1->m01 * mat2->m10 + mat1->m02 * mat2->m20;
	temp.m10 = mat1->m10 * mat2->m00 + mat1->m11 * mat2->m10 + mat1->m12 * mat2->m20;
	temp.m20 = mat1->m20 * mat2->m00 + mat1->m21 * mat2->m10 + mat1->m22 * mat2->m20;

	temp.m01 = mat1->m00 * mat2->m01 + mat1->m01 * mat2->m11 + mat1->m02 * mat2->m21;
	temp.m11 = mat1->m10 * mat2->m01 + mat1->m11 * mat2->m11 + mat1->m12 * mat2->m21;
	temp.m21 = mat1->m20 * mat2->m01 + mat1->m21 * mat2->m11 + mat1->m22 * mat2->m21;

	temp.m02 = mat1->m00 * mat2->m02 + mat1->m01 * mat2->m12 + mat1->m02 * mat2->m22;
	temp.m12 = mat1->m10 * mat2->m02 + mat1->m11 * mat2->m12 + mat1->m12 * mat2->m22;
	temp.m22 = mat1->m20 * mat2->m02 + mat1->m21 * mat2->m12 + mat1->m22 * mat2->m22;

	memcpy(result, &temp, sizeof(float) * 9);
}


void Mat33MultiplyVector(CVector3f *result, const CMatrix33 *mat, const CVector3f *v)
{
	CVector3f temp;

	temp.x = mat->m00 * v->x + mat->m01 * v->y + mat->m02 * v->z;
	temp.y = mat->m10 * v->x + mat->m11 * v->y + mat->m12 * v->z;
	temp.z = mat->m20 * v->x + mat->m21 * v->y + mat->m22 * v->z;

	*result = temp;
}

void Mat44ToMat33(CMatrix33 *result, const CMatrix44 *mat)
{
	result->m00 = mat->right.x;
	result->m10 = mat->right.y;
	result->m20 = mat->right.z;

	result->m01 = mat->up.x;
	result->m11 = mat->up.y;
	result->m21 = mat->up.z;

	result->m01 = mat->at.x;
	result->m11 = mat->at.y;
	result->m21 = mat->at.z;
}

// Cubic spline
void SplineGetControlPoint(CVector3f *result, const CVector3f *pti_minus_one, const CVector3f *pti_plus_one)
{
	Vec3fSubtract(result, pti_plus_one, pti_minus_one);
	Vec3fScale(result, result, 0.5f);
}

void SplineEvaluate(CVector3f *result, const CVector3f *p0, const CVector3f *p1, const CVector3f *m0, const CVector3f *m1, float h)
{
	float a = 2.0f * h * h * h - 3.0f * h * h + 1.0f;
	float b = h * h * h - 2.0f * h * h + h;
	float c = h * h * h - h * h;
	float d = -2.0f * h * h * h + 3.0f * h * h;

	CVector3f temp;
	Vec3fScale(result, p0, a);
	Vec3fScale(&temp, m0, b);
	Vec3fAdd(result, result, &temp);
	Vec3fScale(&temp, m1, c);
	Vec3fAdd(result, result, &temp);
	Vec3fScale(&temp, p1, d);
	Vec3fAdd(result, result, &temp);
}