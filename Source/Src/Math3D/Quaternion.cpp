#include <math.h>
#include "math3d\quaternion.h"

#include "utils\mmgr.h"

// Constructor
CQuaternion::CQuaternion(float w, const CVector3f *v)
{
	this->w = w;
	this->v = *v;
}

// Constructor
CQuaternion::CQuaternion(float w, float x, float y, float z)
{
	this->w = w;
	v.x = x;
	v.y = y;
	v.z = z;
}

// Set to identity [1 0]
void CQuaternion::QuatSetIdentity(void)
{
	w = 1.0f;
	v.x = v.y = v.z = 0.0f;
}

// Negation
void QuatNegate(CQuaternion *result, const CQuaternion *q)
{
	result->w = -q->w;
	Vec3fNegate(&result->v, &q->v);
}

// Magnitude
float QuatMagnitude(const CQuaternion *q)
{
	return (float)sqrt(q->w * q->w + q->v.x * q->v.x + q->v.y * q->v.y + q->v.z * q->v.z);
}

// Magnitude square
float QuatMagnitudeSquare(const CQuaternion *q)
{
	return q->w * q->w + q->v.x * q->v.x + q->v.y * q->v.y + q->v.z * q->v.z;
}

// Normalize
void QuatNormalize(CQuaternion *result, const CQuaternion *q)
{
	// Need to make sure magnitude is not 0
	if (q->w != 0.0f || q->v.x != 0.0f || q->v.y != 0.0f || q->v.z != 0.0f)
	{
		float factor = 1.0f / (float)sqrt(q->w * q->w + q->v.x * q->v.x + q->v.y * q->v.y + q->v.z * q->v.z); 
		result->w = q->w * factor;
		Vec3fScale(&result->v, &q->v, factor);
	}
	else
		// Don't know what to do, the only meaningful thing
		result->QuatSetIdentity();
}

void QuatScale(CQuaternion *result, const CQuaternion *q, float k)
{
	result->w = q->w * k;
	result->v.x = q->v.x * k;
	result->v.y = q->v.y * k;
	result->v.z = q->v.z * k;
}

void QuatFromAngleAxis(CQuaternion *result, float angle, const CVector3f *axis)
{
	float half_angle = angle / 2.0f;
	float sine = (float)sin(half_angle);

	result->w = (float)cos(half_angle);
	Vec3fScale(&result->v, axis, sine);
}

void QuatFromEulerAngles(CQuaternion *result, float x, float y, float z)
{
	CQuaternion qx, qy, qz;
	CVector3f vx(1.0f, 0.0f, 0.0f), vy(0.0f, 1.0f, 0.0f), vz(0.0f, 0.0f, 1.0f);

	QuatFromAngleAxis(&qx, x, &vx);
	QuatFromAngleAxis(&qy, y, &vy);
	QuatFromAngleAxis(&qz, z, &vz);

	QuatCrossProduct(result, &qy, &qx);
	QuatCrossProduct(result, result, &qz);
}

void QuatToEulerAngles(CVector3f *angles, const CQuaternion *q)
{
	float sp = -2.0f * (q->v.y * q->v.z - q->w * q->v.x);

	if (fabs(sp) > 0.9999f)
	{
		angles->x = PI / 2.0f * sp;
		angles->y = (float)atan2(-q->v.x * q->v.z + q->w * q->v.y, 0.5f - q->v.y * q->v.y - q->v.z * q->v.z); 
		angles->z = 0.0f;
	}
	else
	{
		angles->x = (float)asin(sp);
		angles->y = (float)atan2(q->v.x * q->v.z + q->w * q->v.y, 0.5f - q->v.x * q->v.x - q->v.y * q->v.y); 
		angles->z = (float)atan2(q->v.x * q->v.y + q->w * q->v.z, 0.5f - q->v.x * q->v.x - q->v.z * q->v.z); 
	}
}

void QuatConjugate(CQuaternion *result, const CQuaternion *q)
{
	result->w = q->w;
	Vec3fNegate(&result->v, &q->v);
}

void QuatAdd(CQuaternion *result, const CQuaternion *q1, const CQuaternion *q2)
{
	result->w = q1->w + q2->w;
	result->v.x = q1->v.x + q2->v.x;
	result->v.y = q1->v.y + q2->v.y;
	result->v.z = q1->v.z + q2->v.z;
}

void QuatCrossProduct(CQuaternion *result, const CQuaternion *q1, const CQuaternion *q2)
{
	CVector3f v1, v2, v3;

	Vec3fScale(&v1, &q2->v, q1->w);
	Vec3fScale(&v2, &q1->v, q2->w);
	Vec3fCrossProduct(&v3, &q1->v, &q2->v);

	result->w = q1->w * q2->w - Vec3fDotProduct(&q1->v, &q2->v);
	Vec3fAdd(&result->v, &v1, &v2);
	Vec3fAdd(&result->v, &result->v, &v3);
}

float QuatDotProduct(const CQuaternion *q1, const CQuaternion *q2)
{
	return q1->w * q2->w + Vec3fDotProduct(&q1->v, &q2->v);
}

void QuatDifference(CQuaternion *result, const CQuaternion *q1, const CQuaternion *q2)
{
	CQuaternion temp;
	QuatConjugate(&temp, q1);
	QuatCrossProduct(result, &temp, q2);
}

void QuatPower(CQuaternion *result, const CQuaternion *q, float power)
{
	if (fabs(q->w) < 0.9999f)
	{
		float alpha = (float)acos(q->w);
		float alpha2 = alpha * power;
		
		result->w = (float)cos(alpha2);
		float scale = (float)(sin(alpha2) / sin(alpha));

		Vec3fScale(&result->v, &q->v, scale);
	}
}

void QuatLog(CQuaternion *result, const CQuaternion *q)
{
	float a = (float)acos(q->w);
	float sina = (float)sin(a);

	result->w = 0.0f;
	if (sina > 0.0f)
	{
		result->v.x = a * q->v.x / sina;
		result->v.y = a * q->v.y / sina;
		result->v.z = a * q->v.z / sina;
	}
	else
	{
		result->v.x = result->v.y = result->v.z = 0.0f;
	}
}

void QuatExp(CQuaternion *result, const CQuaternion *q)
{
	float a = Vec3fMagnitude(&q->v);
	float sina = (float)sin(a);
	float cosa = (float)cos(a);

	result->w = cosa;
	if(a > 0.0f)
	{
		result->v.x = sina * q->v.x / a;
		result->v.y = sina * q->v.y / a;
		result->v.z = sina * q->v.z / a;
	}
	else
	{
		result->v.x = result->v.y = result->v.z = 0.0f;
	}
}

void QuatGetControlQuat(CQuaternion *result, const CQuaternion *qi_minus_one, const CQuaternion *qi, const CQuaternion *qi_plus_one)
{
	CQuaternion qlog0, qlog1, qi_inv;

	QuatConjugate(&qi_inv, qi);
	
	QuatCrossProduct(&qlog0, qi_plus_one, &qi_inv);
	QuatLog(&qlog0, &qlog0);
	QuatCrossProduct(&qlog1, qi_minus_one, &qi_inv);
	QuatLog(&qlog1, &qlog1);
	QuatAdd(result, &qlog0, &qlog1);
	QuatScale(result, result, -0.25f);
	QuatExp(result, result);
	QuatCrossProduct(result, qi, result);
}

void QuatSlerp(CQuaternion *result, const CQuaternion *q0, const CQuaternion *q1, float t)
{
	float cosTheta = QuatDotProduct(q0, q1);
	float k0, k1;

	if (cosTheta > 0.9999f)
	{
		k0 = 1.0f - t;
		k1 = t;
	}
	else
	{
		float sinTheta = (float)sqrt(1.0f - cosTheta * cosTheta);
		float theta = (float)atan2(sinTheta, cosTheta);
		float denom = 1.0f / sinTheta;

		k0 = (float)sin((1.0f - t) * theta) * denom;
		k1 = (float)sin(t * theta) * denom;
	}

	CQuaternion temp0, temp1;
	QuatScale(&temp0, q0, k0);
	QuatScale(&temp1, q1, k1);
	QuatAdd(result, &temp0, &temp1);
}

void QuatSquad(CQuaternion *result, const CQuaternion *q0, const CQuaternion *q1, const CQuaternion *s0, const CQuaternion *s1, float h)
{
	CQuaternion slerpQ, slerpS;

	QuatSlerp(&slerpQ, q0, q1, h);
	QuatSlerp(&slerpS, s0, s1, h);
	QuatSlerp(result, &slerpQ, &slerpS, 2.0f * h * (1.0f - h));
}

void QuatRotateVector(CVector3f *result, const CQuaternion *q, const CVector3f *v)
{
	CQuaternion temp, inv;

	temp.w = 0.0f;
	temp.v = *v;
	QuatConjugate(&inv, q);

	QuatCrossProduct(&temp, q, &temp);
	QuatCrossProduct(&temp, &temp, &inv);

	*result = temp.v;
}

void QuatToRotationMatrix(CMatrix44 *result, const CQuaternion *q)
{
	float xx2 = q->v.x * q->v.x * 2.0f;
	float yy2 = q->v.y * q->v.y * 2.0f;
	float zz2 = q->v.z * q->v.z * 2.0f;
	float wx2 = q->w * q->v.x * 2.0f;
	float wy2 = q->w * q->v.y * 2.0f;
	float wz2 = q->w * q->v.z * 2.0f;
	float xy2 = q->v.x * q->v.y * 2.0f;
	float xz2 = q->v.x * q->v.z * 2.0f;
	float yz2 = q->v.y * q->v.z * 2.0f;

	result->right.x = 1 - yy2 - zz2;
	result->right.y = xy2 + wz2;
	result->right.z = xz2 - wy2;
	result->up.x = xy2 - wz2;
	result->up.y = 1- xx2 - zz2;
	result->up.z = yz2 + wx2;
	result->at.x = xz2 + wy2;
	result->at.y = yz2 - wx2;
	result->at.z = 1 - xx2 - yy2;
}

void QuatToRotationMatrix(CMatrix33 *result, const CQuaternion *q)
{
	float xx2 = q->v.x * q->v.x * 2.0f;
	float yy2 = q->v.y * q->v.y * 2.0f;
	float zz2 = q->v.z * q->v.z * 2.0f;
	float wx2 = q->w * q->v.x * 2.0f;
	float wy2 = q->w * q->v.y * 2.0f;
	float wz2 = q->w * q->v.z * 2.0f;
	float xy2 = q->v.x * q->v.y * 2.0f;
	float xz2 = q->v.x * q->v.z * 2.0f;
	float yz2 = q->v.y * q->v.z * 2.0f;

	result->m00 = 1 - yy2 - zz2;
	result->m01 = xy2 + wz2;
	result->m02 = xz2 - wy2;
	result->m10 = xy2 - wz2;
	result->m11 = 1- xx2 - zz2;
	result->m12 = yz2 + wx2;
	result->m20 = xz2 + wy2;
	result->m21 = yz2 - wx2;
	result->m22 = 1 - xx2 - yy2;
}

void QuatFromRotationMatrix(CQuaternion *result, const CMatrix33 *mat)
{
	float w = mat->m00 + mat->m11 + mat->m22;
	float x = mat->m00 - mat->m11 - mat->m22;
	float y = mat->m11 - mat->m00 - mat->m22;
	float z = mat->m22 - mat->m00 - mat->m11;

	float max = w;
	int index = 0;

	if (x > max)
	{
		max = x;
		index = 1;
	}
	if (y > max)
	{
		max = y;
		index = 2;
	}
	if (z > max)
	{
		max = z;
		index = 3;
	}

	max = (float)(sqrt(max + 1.0f) * 0.5f);
	float m = 0.25f / max;

	switch (index)
	{
	case 0:
		result->w = max;
		result->v.x = (mat->m12 - mat->m21) * m;
		result->v.y = (mat->m20 - mat->m02) * m;
		result->v.z = (mat->m01 - mat->m10) * m;
		break;
	case 1:
		result->w = (mat->m12 - mat->m21) * m;
		result->v.x = max;
		result->v.y = (mat->m01 + mat->m10) * m;
		result->v.z = (mat->m20 + mat->m02) * m;
		break;
	case 2:
		result->w = (mat->m20 - mat->m02) * m;
		result->v.x = (mat->m01 + mat->m10) * m;
		result->v.y = max;
		result->v.z = (mat->m12 + mat->m21) * m;
		break;
	case 3:
		result->w = (mat->m01 - mat->m10) * m;
		result->v.x = (mat->m20 + mat->m02) * m;
		result->v.y = (mat->m12 + mat->m21) * m;
		result->v.z = max;
		break;
	}
}

void QuatRotationArc(CQuaternion *result, const CVector3f *vstart, const CVector3f *vend)
{
	CVector3f v0, v1, vc;
	float d, s;

	Vec3fNormalize(&v0, vstart);
	Vec3fNormalize(&v1, vend);
	Vec3fCrossProduct(&vc, &v0, &v1);
	d = Vec3fDotProduct(&v0, &v1);
	s = (float)sqrt((1.0f + d) * 2.0f);

	if (s != 0.0f)
	{
		result->w = s / 2.0f;
		result->v.x = vc.x / s;
		result->v.y = vc.y / s;
		result->v.z = vc.z / s;
	}
	else
		result->QuatSetIdentity();
}

