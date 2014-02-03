#include <math.h>
#include "math3d\objframe.h"

#include "utils\mmgr.h"

//////////////////////////////////////////////////
// Future Improvement:
// For the translation/rotation of the XYZ axes, we
// should take advantage of all those 0's.
// We should have specialized RotateVector functions
// for them.
//////////////////////////////////////////////////

CObjFrame::CObjFrame(void)
{
	Reset();
}

void CObjFrame::MoveForward(float Dist)
{
	CVector3f Displacement(0.0f, 0.0f, Dist);
	QuatRotateVector(&Displacement, &m_Orientation, &Displacement);
	Vec3fAdd(&m_Position, &m_Position, &Displacement);
}

void CObjFrame::MoveRight(float Dist)
{
	CVector3f Displacement(-Dist, 0.0f, 0.0f);
	QuatRotateVector(&Displacement, &m_Orientation, &Displacement);
	Vec3fAdd(&m_Position, &m_Position, &Displacement);
}

void CObjFrame::MoveUp(float Dist)
{
	CVector3f Displacement(0.0f, Dist, 0.0f);
	QuatRotateVector(&Displacement, &m_Orientation, &Displacement);
	Vec3fAdd(&m_Position, &m_Position, &Displacement);
}

void CObjFrame::TranslateLocal(const CVector3f *v)
{
	CVector3f Displacement;
	QuatRotateVector(&Displacement, &m_Orientation, v);
	Vec3fAdd(&m_Position, &m_Position, &Displacement);
}

void CObjFrame::TranslateWorld(const CVector3f *v)
{
	Vec3fAdd(&m_Position, &m_Position, v);
}

void CObjFrame::RotateLocalX(float Angle)
{
	CVector3f Axis(1.0f, 0.0f, 0.0f);
	RotateLocalAxis(Angle, &Axis);
}

void CObjFrame::RotateLocalY(float Angle)
{
	CVector3f Axis(0.0f, 1.0f, 0.0f);
	RotateLocalAxis(Angle, &Axis);
}

void CObjFrame::RotateLocalZ(float Angle)
{
	CVector3f Axis(0.0f, 0.0f, 1.0f);
	RotateLocalAxis(Angle, &Axis);
}

void CObjFrame::RotateLocalAxis(float Angle, const CVector3f *Axis)
{
	CQuaternion q;
	CVector3f LocalAxis;

	QuatRotateVector(&LocalAxis, &m_Orientation, Axis);
	QuatFromAngleAxis(&q, Angle, &LocalAxis);
	QuatCrossProduct(&m_Orientation, &q, &m_Orientation);
}

void CObjFrame::RotateWorldX(float Angle)
{
	CVector3f axis(1.0f, 0.0f, 0.0f);
	RotateWorldAxis(Angle, &axis);
}

void CObjFrame::RotateWorldY(float Angle)
{
	CVector3f axis(0.0f, 1.0f, 0.0f);
	RotateWorldAxis(Angle, &axis);
}

void CObjFrame::RotateWorldZ(float Angle)
{
	CVector3f axis(0.0f, 0.0f, 1.0f);
	RotateWorldAxis(Angle, &axis);
}

void CObjFrame::RotateWorldAxis(float Angle, const CVector3f *Axis)
{
	CQuaternion q;

	QuatFromAngleAxis(&q, Angle, Axis);
	QuatCrossProduct(&m_Orientation, &q, &m_Orientation);
	QuatRotateVector(&m_Position, &q, &m_Position);
}

void CObjFrame::Reset(void)
{
	m_Orientation.QuatSetIdentity();
	m_Position.x = m_Position.y = m_Position.z = 0.0f;
	m_ScaleX = m_ScaleY = m_ScaleZ = 1.0f;
}

void CObjFrame::GetOpenGLMatrix(float *result)
{
	CMatrix44 *mat = (CMatrix44 *)result;

	mat->m40 = mat->m41 = mat->m42 = 0.0f;
	mat->m43 = 1.0f;

	QuatNormalize(&m_Orientation, &m_Orientation);
	QuatToRotationMatrix(mat, &m_Orientation);
	mat->pos = m_Position;

	if (m_ScaleX != 1.0f)
		Vec3fScale(&mat->right, &mat->right, m_ScaleX);
	if (m_ScaleY != 1.0f)
		Vec3fScale(&mat->up, &mat->up, m_ScaleY);
	if (m_ScaleZ != 1.0f)
		Vec3fScale(&mat->at, &mat->at, m_ScaleZ);
}

void CObjFrame::GetUp(CVector3f *up) const
{
	CVector3f v(0.0f, 1.0f, 0.0f);
	RotateToWorldVector(up, &v);
}

void CObjFrame::GetForward(CVector3f *forward) const
{
	CVector3f v(0.0f, 0.0f, 1.0f);
	RotateToWorldVector(forward, &v);
}

// This is more like left to the object......
void CObjFrame::GetRight(CVector3f *right) const
{
	CVector3f v(1.0f, 0.0f, 0.0f);
	RotateToWorldVector(right, &v);
}

void CObjFrame::GetEulerAngles(CVector3f *angles) const
{
	QuatToEulerAngles(angles, &m_Orientation);
}

// Rotate a local vector into world space
void CObjFrame::RotateToWorldVector(CVector3f *result, const CVector3f *v) const
{
	QuatRotateVector(result, &m_Orientation, v);
}

void CObjFrame::RotateToLocalVector(CVector3f *result, const CVector3f *v) const
{
	CQuaternion inv;
	QuatConjugate(&inv, &m_Orientation);
	QuatRotateVector(result, &inv, v);
}

void CObjFrame::RotateToWorldPoint(CVector3f *result, const CVector3f *pt) const
{
	QuatRotateVector(result, &m_Orientation, pt);
	Vec3fAdd(result, result, &m_Position);
}

void CObjFrame::RotateToLocalPoint(CVector3f *result, const CVector3f *pt) const
{
	CQuaternion qinv;

	Vec3fSubtract(result, pt, &m_Position);
	QuatConjugate(&qinv, &m_Orientation);
	QuatRotateVector(result, &qinv, result);
}

void CObjFrame::SetToEulerAngles(float x, float y, float z)
{
	QuatFromEulerAngles(&m_Orientation, x, y, z);
}

void ObjFrameInterpolate(CObjFrame *result, const CObjFrame *frames, int index, float t)
{
	CQuaternion a, b;
	QuatGetControlQuat(&a, &frames[index - 1].m_Orientation, 
									&frames[index].m_Orientation,
									&frames[index + 1].m_Orientation);
	QuatGetControlQuat(&b, &frames[index].m_Orientation, 
									&frames[index + 1].m_Orientation,
									&frames[index + 2].m_Orientation);
	QuatSquad(&result->m_Orientation,
				 &frames[index].m_Orientation,
				 &frames[index + 1].m_Orientation,
				 &a, &b, t);

	CVector3f m0, m1;
	SplineGetControlPoint(&m0, &frames[index - 1].m_Position, &frames[index + 1].m_Position);
	SplineGetControlPoint(&m1, &frames[index].m_Position, &frames[index + 2].m_Position);
	SplineEvaluate(&result->m_Position, 
						&frames[index].m_Position,
						&frames[index + 1].m_Position,
						&m0, &m1, t);
}
