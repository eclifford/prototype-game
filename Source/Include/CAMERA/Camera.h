#pragma once

#include "math3d\objframe.h"

class CGameObject;

// ABS for all cameras
class CBaseCamera
{
public:
	virtual void ApplyTransform(void) = 0;
	virtual void GetInverseTransformMatrix(CMatrix44 *mat) = 0;
	virtual void GetPosition(CVector3f *pos) = 0;
	virtual void GetLookAt(CVector3f *v) = 0;
	virtual void GetUp(CVector3f *v) = 0;
	virtual void GetFrame(CObjFrame *frame) = 0;
};

// Basic first person camera
class CFirstPersonCamera : public CBaseCamera
{
private:
	CObjFrame	m_CameraFrame;
public:
	CFirstPersonCamera(void);
	void MoveForward(float Dist);
	void MoveRight(float Dist);
	void MoveUp(float Dist);
	void MoveDir(const CVector3f *v);
	void TurnRight(float Angle);
	void TurnUp(float Angle);

	void ApplyTransform(void);
	void GetInverseTransformMatrix(CMatrix44 *mat);
	void GetPosition(CVector3f *pos);
	void GetLookAt(CVector3f *v)
	{
		m_CameraFrame.GetForward(v);
	}
	void GetUp(CVector3f *v)
	{
		m_CameraFrame.GetUp(v);
	}
	void SetPosition(const CVector3f *pos)
	{
		m_CameraFrame.SetPosition(pos);
	}
	void GetFrame(CObjFrame *frame)
	{
		*frame = m_CameraFrame;
	}
};

// Look-at camera
class CLookAtCamera : public CBaseCamera
{
private:
	CGameObject	*m_Target;
	CVector3f	m_CameraPos;
	CMatrix44	m_CameraMatrix;
public:
	CLookAtCamera(void);
	void SetTarget(CGameObject *obj)
	{
		m_Target = obj;
	}

	void MoveTo(const CVector3f *pos);
	void ApplyTransform(void);
	void GetInverseTransformMatrix(CMatrix44 *mat);
	void GetPosition(CVector3f *pos);
	void GetLookAt(CVector3f *v)
	{
		*v = m_CameraMatrix.at;
	}
	void GetUp(CVector3f *v)
	{
		*v = m_CameraMatrix.up;
	}
	void GetFrame(CObjFrame *frame);
};

class CFollowCamera : public CBaseCamera
{
private:
	CGameObject *m_Target;
	CVector3f	m_Offset;
	CObjFrame m_CameraFrame;
	CObjFrame m_ControlFrames[4];
	int m_ControlIndex;		// Interpolating from i to i + 1
									// Need i - 1 and i + 2 also
									// ControlIndex is i
	double m_UpdatePeriod;	// Get a new frame from target every X ms
	double m_T0;				// Start time of current period
	float m_Pitch;

	void Update(void);
	int GetIndex(int n);
public:
	CFollowCamera(void);
	void SetTarget(const CGameObject *obj, const CVector3f *offset, float pitch);
	void SetOffset(const CVector3f *offset)
	{
		m_Offset = *offset;
	}
	void SetPeriod(double t)
	{
		m_UpdatePeriod = t;
	}
	void SetPitch(float angle)
	{
		m_Pitch = angle;
	}
	void ApplyTransform(void);
	void GetInverseTransformMatrix(CMatrix44 *mat);
	void GetPosition(CVector3f *pos);
	void GetLookAt(CVector3f *v)
	{
		m_CameraFrame.GetForward(v);
	}
	void GetUp(CVector3f *v)
	{
		m_CameraFrame.GetUp(v);
	}
	void GetFrame(CObjFrame *frame)
	{
		*frame = m_CameraFrame;
	}
};

class CPlayerCamera: public CBaseCamera
{
private:
	CObjFrame	m_CameraFrame;
	CGameObject *m_Player;
	CVector3f	m_Offset;
	float			m_Pitch;
	float			m_MaxPitch;
	float			m_MinPitch;
	float			m_DirScale;

	void Update(void);
	float ComputePitchLimit(float theta, float y, float z);
public:
	CPlayerCamera(void);
	void Setup(CGameObject *player, const CVector3f *offset);

	void TurnRight(float angle);
	void TurnUp(float angle);
	void ApplyTransform(void);
	void GetInverseTransformMatrix(CMatrix44 *mat);
	void GetPosition(CVector3f *pos);
	void GetLookAt(CVector3f *v);
	void GetUp(CVector3f *v);
	float	GetPitch(void) const
	{
		return m_Pitch;
	}
	float	GetDirScale(void) const
	{
		return m_DirScale;
	}
	void GetFrame(CObjFrame *frame)
	{
		*frame = m_CameraFrame;
	}
};

class CFlyByCamera : public CBaseCamera
{
private:
	CObjFrame	m_CameraFrame;
	int			m_FrameCount;
	CObjFrame	*m_ControlFrames;
	float			m_Length;
	float			m_UpdatePeriod;
	float			m_StartTime;

	void Update(void);
public:
	CFlyByCamera(void);
	~CFlyByCamera(void);

	void SetUp(int framecount, CObjFrame *frames, float t);
	void ApplyTransform(void);
	void GetInverseTransformMatrix(CMatrix44 *mat);
	void GetPosition(CVector3f *pos);
	void GetLookAt(CVector3f *v)
	{
		m_CameraFrame.GetForward(v);
	}
	void GetUp(CVector3f *v)
	{
		m_CameraFrame.GetUp(v);
	}
	void GetFrame(CObjFrame *frame)
	{
		*frame = m_CameraFrame;
	}
	float GetElapsedTime(void) const;
};