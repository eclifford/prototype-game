#ifndef _QUATERNION_H_
#define _QUATERNION_H_

#include "math3d\math3d.h"

// Quaternion for rotation
class CQuaternion
{
public:
	float w;
	CVector3f v;

	CQuaternion(void) {}
	CQuaternion(float w, const CVector3f *v);
	CQuaternion(float w, float x, float y, float z);
	void QuatSetIdentity(void);
	const CQuaternion &operator=(const CQuaternion &q)
	{
		w = q.w;
		v = q.v;
		return *this;
	}
	bool operator==(const CQuaternion &q)
	{
		return (w == q.w && v == q.v);
	}
};

// Negation (-q = [-w -v])
void QuatNegate(CQuaternion *result, const CQuaternion *q);

// Conjugate (q* = [w -v])
void QuatConjugate(CQuaternion *result, const CQuaternion *q);

// Magnitude
float QuatMagnitude(const CQuaternion *q);

// Magnitude square, saves the sqrt
float QuatMagnitudeSquare(const CQuaternion *q);

// Normalize
void QuatNormalize(CQuaternion *result, const CQuaternion *q);

// Scalar multiply
void QuatScale(CQuaternion *result, const CQuaternion *q, float k);

void QuatAdd(CQuaternion *result, const CQuaternion *q1, const CQuaternion *q2);

// Cross product
void QuatCrossProduct(CQuaternion *result, const CQuaternion *q1, const CQuaternion *q2);

// Dot product
float QuatDotProduct(const CQuaternion *q1, const CQuaternion *q2);
void QuatDifference(CQuaternion *result, const CQuaternion *q1, const CQuaternion *q2);
void QuatPower(CQuaternion *result, const CQuaternion *q, float power);
void QuatLog(CQuaternion *result, const CQuaternion *q);
void QuatExp(CQuaternion *result, const CQuaternion *q);

void QuatGetControlQuat(CQuaternion *result, const CQuaternion *qi_minus_one, const CQuaternion *qi, const CQuaternion *qi_plus_one);
void QuatSlerp(CQuaternion *result, const CQuaternion *q0, const CQuaternion *q1, float t);
void QuatSquad(CQuaternion *result, const CQuaternion *q0, const CQuaternion *q1, const CQuaternion *s0, const CQuaternion *s1, float h);

void QuatRotateVector(CVector3f *result, const CQuaternion *q, const CVector3f *v);

// Construct quaternion from angle-axis pair
void QuatFromAngleAxis(CQuaternion *result, float angle, const CVector3f *axis);
void QuatFromEulerAngles(CQuaternion *result, float x, float y, float z);
void QuatToEulerAngles(CVector3f *angles, const CQuaternion *q);

// Convert to rotational matrix
void QuatToRotationMatrix(CMatrix44 *result, const CQuaternion *q);
void QuatToRotationMatrix(CMatrix33 *result, const CQuaternion *q);

void QuatFromRotationMatrix(CQuaternion *result, const CMatrix33 *mat);

void QuatRotationArc(CQuaternion *result, const CVector3f *vstart, const CVector3f *vend);

#endif