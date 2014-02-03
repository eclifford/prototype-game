#ifndef _FRAME_H_
#define _FRAME_H_

#include "math3d\math3d.h"
#include "math3d\quaternion.h"

class CObjFrame
{
private:
	CQuaternion m_Orientation;
	CVector3f	m_Position;
	float			m_ScaleX;
	float			m_ScaleY;
	float			m_ScaleZ;
public:
	CObjFrame(void);

	void GetOrientation(CQuaternion *q)
	{
		*q = m_Orientation;
	}

	void SetOrientation(const CQuaternion *q)
	{
		m_Orientation = *q;
	}

	void GetPosition(CVector3f *v) const
	{
		*v = m_Position;
	}

	// BAD!!!!!!!!!!!!
	const CVector3f *GetPosition(void) const
	{
		return &m_Position;
	}

	void SetPosition(const CVector3f *v)
	{
		m_Position = *v;
	}

	const CObjFrame &operator=(const CObjFrame &frame)
	{
		m_Orientation = frame.m_Orientation;
		m_Position = frame.m_Position;
		return *this;
	}

	bool operator==(const CObjFrame &frame)
	{
		return (m_Orientation == frame.m_Orientation && m_Position == frame.m_Position);
	}

	void MoveForward(float Dist);
	void MoveRight(float Dist);
	void MoveUp(float Dist);
	void TranslateLocal(const CVector3f *v);
	void TranslateWorld(const CVector3f *v);
	void RotateLocalX(float Angle);
	void RotateLocalY(float Angle);
	void RotateLocalZ(float Angle);
	void RotateLocalAxis(float Angle, const CVector3f *Axis);
	void RotateWorldX(float Angle);
	void RotateWorldY(float Angle);
	void RotateWorldZ(float Angle);
	void RotateWorldAxis(float Angle, const CVector3f *Axis);
	void SetScaleXYZ(float sx, float sy, float sz)
	{
		m_ScaleX = sx;
		m_ScaleY = sy;
		m_ScaleZ = sz;
	}
	void SetScaleZ(float sz)
	{
		m_ScaleZ = sz;
	}
	void SetToEulerAngles(float x, float y, float z);

	void GetOpenGLMatrix(float *result);
	void RotateToWorldVector(CVector3f *result, const CVector3f *v) const;
	void RotateToLocalVector(CVector3f *result, const CVector3f *v) const;
	void RotateToWorldPoint(CVector3f *result, const CVector3f *pt) const;
	void RotateToLocalPoint(CVector3f *result, const CVector3f *pt) const;
	void GetUp(CVector3f *up) const;
	void GetForward(CVector3f *forward) const;
	void GetRight(CVector3f *right) const;
	void GetEulerAngles(CVector3f *angles) const;

	void Reset(void);

	friend class CFirstPersonCamera;
	friend class CLookAtCamera;
	friend class CFollowCamera;
	friend class CPlayerCamera;
	friend class CFlyByCamera;
	friend class CRigidBody;
	friend class CMothership;

	friend void ObjFrameInterpolate(CObjFrame *result, const CObjFrame *frames, int index, float t);
};

void ObjFrameInterpolate(CObjFrame *result, const CObjFrame *frames, int index, float t);
#endif