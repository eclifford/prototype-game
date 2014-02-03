#include <windows.h>
#include <gl/gl.h>
#include <mmsystem.h>
#include <math.h>
#include "gamelogic\player.h"
#include "camera\camera.h"

#include "utils\mmgr.h"

extern float g_time;

// First person camera
CFirstPersonCamera::CFirstPersonCamera(void)
{
	m_CameraFrame.Reset();

	// Default looking at -z
	m_CameraFrame.RotateWorldY(PI);
}

void CFirstPersonCamera::MoveForward(float Dist)
{
	m_CameraFrame.MoveForward(Dist);
}

void CFirstPersonCamera::MoveRight(float Dist)
{
	m_CameraFrame.MoveRight(Dist);
}

void CFirstPersonCamera::MoveUp(float Dist)
{
	CVector3f Displacement(0.0f, Dist, 0.0f);
	m_CameraFrame.TranslateWorld(&Displacement);
}

void CFirstPersonCamera::MoveDir(const CVector3f *v)
{
	m_CameraFrame.TranslateLocal(v);
}

void CFirstPersonCamera::TurnRight(float Angle)
{
	// Translate back to origin
	CVector3f Translate;
	Vec3fScale(&Translate, &m_CameraFrame.m_Position, -1.0f);
	m_CameraFrame.TranslateWorld(&Translate);

	// Rotate against world Y
	m_CameraFrame.RotateWorldY(-Angle);

	// Translate back
	Vec3fScale(&Translate, &Translate, -1.0f);
	m_CameraFrame.TranslateWorld(&Translate);
}

void CFirstPersonCamera::TurnUp(float Angle)
{
	m_CameraFrame.RotateLocalX(Angle);
}

void CFirstPersonCamera::GetPosition(CVector3f *pos)
{
	m_CameraFrame.GetPosition(pos);
}

void CFirstPersonCamera::ApplyTransform(void)
{
	CMatrix44 mat;
	CQuaternion inv;

	// Do inverse transform, rotation only
	QuatConjugate(&inv, &m_CameraFrame.m_Orientation);
	Mat44LoadIdentity(&mat);
	QuatToRotationMatrix(&mat, &inv);

	// Need to flip z and x
	mat.right.z = -mat.right.z;
	mat.up.z = -mat.up.z;
	mat.at.z = -mat.at.z;
	mat.right.x = -mat.right.x;
	mat.up.x = -mat.up.x;
	mat.at.x = -mat.at.x;
	glMultMatrixf((float *)&mat);

	// Now inverse translate
	glTranslatef(-m_CameraFrame.m_Position.x, -m_CameraFrame.m_Position.y, -m_CameraFrame.m_Position.z);
}

void CFirstPersonCamera::GetInverseTransformMatrix(CMatrix44 *mat)
{
	Mat44LoadIdentity(mat);
	QuatToRotationMatrix(mat, &m_CameraFrame.m_Orientation);

	// Need to flip z and x
	Vec3fScale(&mat->right, &mat->right, -1.0f);
	Vec3fScale(&mat->at, &mat->at, -1.0f);
	mat->pos = m_CameraFrame.m_Position;
}

//////////////////////////////////////////////////////////////////////////////
// Look-at camera
///////////////////////////////////////////////////////////////////////////////
CLookAtCamera::CLookAtCamera(void)
{
	m_Target = NULL;
	Vec3fZero(&m_CameraPos);
	Mat44LoadIdentity(&m_CameraMatrix);
}

void CLookAtCamera::MoveTo(const CVector3f *pos)
{
	m_CameraPos = *pos;
}

void CLookAtCamera::GetPosition(CVector3f *pos)
{
	*pos = m_CameraPos;
}

void CLookAtCamera::ApplyTransform(void)
{
	if (m_Target)
	{
		CVector3f Target;

		Mat44LoadIdentity(&m_CameraMatrix);
		m_Target->GetPosition(&Target, 0);

		Vec3fSubtract(&m_CameraMatrix.at, &m_CameraPos, &Target);
		Vec3fNormalize(&m_CameraMatrix.at, &m_CameraMatrix.at);

		CVector3f up(0.0f, 1.0f, 0.0f);
		Vec3fCrossProduct(&m_CameraMatrix.right, &up, &m_CameraMatrix.at);
		Vec3fNormalize(&m_CameraMatrix.right, &m_CameraMatrix.right);

		Vec3fCrossProduct(&m_CameraMatrix.up, &m_CameraMatrix.at, &m_CameraMatrix.right);
		Vec3fNormalize(&m_CameraMatrix.up, &m_CameraMatrix.up);

		// Transpose for inverse transform
		Mat44Transpose(&m_CameraMatrix, &m_CameraMatrix);
		glMultMatrixf((float*)&m_CameraMatrix);

		glTranslatef(-m_CameraPos.x, -m_CameraPos.y, -m_CameraPos.z);
	}
}

void CLookAtCamera::GetInverseTransformMatrix(CMatrix44 *mat)
{
	Mat44Transpose(mat, &m_CameraMatrix);
	mat->pos = m_CameraPos;
}

void CLookAtCamera::GetFrame(CObjFrame *frame)
{
	if (m_Target)
	{
		CQuaternion q;
		CMatrix33 mat;
		CVector3f *basis = (CVector3f *)&mat;
		CVector3f Target;

		Mat33LoadIdentity(&mat);
		m_Target->GetPosition(&Target, 0);

		Vec3fSubtract(&basis[2], &Target, &m_CameraPos);
		Vec3fNormalize(&basis[2], &basis[2]);

		CVector3f up(0.0f, 1.0f, 0.0f);
		Vec3fCrossProduct(&basis[0], &up, &basis[2]);
		Vec3fNormalize(&basis[0], &basis[0]);

		Vec3fCrossProduct(&basis[1], &basis[2], &basis[0]);
		Vec3fNormalize(&basis[1], &basis[1]);

		QuatFromRotationMatrix(&q, &mat);
		
		QuatConjugate(&q, &q);
		frame->SetOrientation(&q);
		frame->SetPosition(&m_CameraPos);
	}
	else
		frame->Reset();
}

//////////////////////////////////////////////////////////////////////////////
// follow camera
///////////////////////////////////////////////////////////////////////////////
CFollowCamera::CFollowCamera(void)
{
	m_Target = 0;
	Vec3fZero(&m_Offset);
	m_UpdatePeriod = 0.0;
	m_ControlIndex = 0;
	m_CameraFrame.Reset();
}

void CFollowCamera::SetTarget(const CGameObject *obj, const CVector3f *offset, float pitch)
{
	m_Target = const_cast<CGameObject *>(obj);
	m_T0 = timeGetTime();
	m_Offset = *offset;
	m_ControlIndex = 1;
	m_Pitch = pitch;

	m_CameraFrame = *(m_Target->GetFrame(0));
	m_CameraFrame.TranslateLocal(&m_Offset);
	m_CameraFrame.RotateLocalX(m_Pitch);

	// Fill in control frames
	m_ControlFrames[0] = m_CameraFrame;
	m_ControlFrames[1] = m_CameraFrame;
	m_ControlFrames[2] = m_CameraFrame;
	m_ControlFrames[3] = m_CameraFrame;
}

void CFollowCamera::ApplyTransform(void)
{
	CMatrix44 mat;
	CQuaternion inv;
	CVector3f pos;

	if (m_Target)
	{
		CMatrix44 mat;
		CQuaternion inv;

		Update();

		// Do inverse transform, rotation only
		QuatConjugate(&inv, &m_CameraFrame.m_Orientation);
		Mat44LoadIdentity(&mat);
		QuatToRotationMatrix(&mat, &inv);

		// Need to flip z and x
		mat.right.z = -mat.right.z;
		mat.up.z = -mat.up.z;
		mat.at.z = -mat.at.z;
		mat.right.x = -mat.right.x;
		mat.up.x = -mat.up.x;
		mat.at.x = -mat.at.x;
		glMultMatrixf((float *)&mat);

		// Now inverse translate
		glTranslatef(-m_CameraFrame.m_Position.x, -m_CameraFrame.m_Position.y, -m_CameraFrame.m_Position.z);
	}
}

void CFollowCamera::GetInverseTransformMatrix(CMatrix44 *mat)
{
	Mat44LoadIdentity(mat);
	QuatToRotationMatrix(mat, &m_CameraFrame.m_Orientation);

	// Need to flip z and x
	Vec3fScale(&mat->right, &mat->right, -1.0f);
	Vec3fScale(&mat->at, &mat->at, -1.0f);
	mat->pos = m_CameraFrame.m_Position;
}

int CFollowCamera::GetIndex(int n)
{
	int index = -1;

	switch (n)
	{
	case -1:
		index = m_ControlIndex - 1;
		if (index < 0)
			index = 3;
		break;
	case 0:
		index = m_ControlIndex;
		break;
	case 1:
		index = (m_ControlIndex + 1) % 4;
		break;
	case 2:
		index = (m_ControlIndex + 2) % 4;
		break;
	}

	return index;
}

void CFollowCamera::GetPosition(CVector3f *pos)
{
	m_CameraFrame.GetPosition(pos);
}

void CFollowCamera::Update(void)
{
	double time = timeGetTime();

	// If update is needed, get a new frame and store in old i - 1
	// then advance i
	if (time - m_T0 > m_UpdatePeriod)
	{
		m_T0 += m_UpdatePeriod;
		m_ControlFrames[GetIndex(-1)] = *(m_Target->GetFrame(0));
		m_ControlFrames[GetIndex(-1)].TranslateLocal(&m_Offset);
		m_ControlFrames[GetIndex(-1)].RotateLocalX(m_Pitch);
		m_ControlIndex++;
		if (m_ControlIndex > 3)
			m_ControlIndex = 0;
	}

	float h = float((time - m_T0) / m_UpdatePeriod);

	m_CameraFrame = m_ControlFrames[m_ControlIndex];

	// Interpolate orientation using squad
	// Compute the 2 "tangent" orientation
	CQuaternion a, b;
	QuatGetControlQuat(&a, &m_ControlFrames[GetIndex(-1)].m_Orientation, 
									&m_ControlFrames[GetIndex(0)].m_Orientation,
									&m_ControlFrames[GetIndex(1)].m_Orientation);
	QuatGetControlQuat(&b, &m_ControlFrames[GetIndex(0)].m_Orientation, 
									&m_ControlFrames[GetIndex(1)].m_Orientation,
									&m_ControlFrames[GetIndex(2)].m_Orientation);
	QuatSquad(&m_CameraFrame.m_Orientation,
				 &m_ControlFrames[GetIndex(0)].m_Orientation,
				 &m_ControlFrames[GetIndex(1)].m_Orientation,
				 &a, &b, h);

	// Interpolate position using cubic spline
	CVector3f m0, m1;
	SplineGetControlPoint(&m0, &m_ControlFrames[GetIndex(-1)].m_Position, &m_ControlFrames[GetIndex(1)].m_Position);
	SplineGetControlPoint(&m1, &m_ControlFrames[GetIndex(0)].m_Position, &m_ControlFrames[GetIndex(2)].m_Position);
	SplineEvaluate(&m_CameraFrame.m_Position, 
						&m_ControlFrames[GetIndex(0)].m_Position,
						&m_ControlFrames[GetIndex(1)].m_Position,
						&m0, &m1, h);
}

////////////////////////////////////////////////
// Player camera
////////////////////////////////////////////////
CPlayerCamera::CPlayerCamera(void)
{
	m_CameraFrame.Reset();
	m_Player = 0;
}

float CPlayerCamera::ComputePitchLimit(float theta, float y, float z)
{
	if (theta > 0.0f)
	{
		float beta = (float)atan2(m_DirScale - z, y);
		float delta = (float)atan2(z, y);
		float phi = PI - theta - beta - delta;
		return PI / 2.0f - delta - phi;
	}
	else if (theta < 0.0f)
	{
		float beta = (float)atan2(y, m_DirScale - z);
		return -(beta - theta);
	}
	else
		return (float)atan2(y, m_DirScale - z);
}

void CPlayerCamera::Setup(CGameObject *player, const CVector3f *offset)
{
	m_Player = player;
	m_Offset = *offset;
	m_DirScale = 250.0f;
	m_Pitch = 0.0f;

	CVector3f pos0, pforward, forward;
	float theta, dot, min, max;

	m_CameraFrame.Reset();

	((CPlayer *)player)->GetMinMaxElevations(min, max);

	m_MinPitch = ComputePitchLimit(min, fabs(offset->y), fabs(offset->z));
	m_MaxPitch = ComputePitchLimit(max, fabs(offset->y), fabs(offset->z));

	// Set to player position
	player->GetFrame(0)->GetPosition(&pos0);
	m_CameraFrame.SetPosition(&pos0);

	// Rotate Y so camera faces player's forward
	player->GetFrame(0)->GetForward(&pforward);
	m_CameraFrame.GetForward(&forward);
	pforward.y = forward.y = 0.0f;
	Vec3fNormalize(&pforward, &pforward);
	Vec3fNormalize(&forward, &forward);
	dot = Vec3fDotProduct(&forward, &pforward);
	theta = (float)acos(dot);

	CVector3f cross;
	Vec3fCrossProduct(&cross, &forward, &pforward);
	if (cross.y > 0.0f)
		m_CameraFrame.RotateLocalY(theta);
	else if (cross.y < 0.0f)
		m_CameraFrame.RotateLocalY(2.0f * PI - theta);

	m_CameraFrame.TranslateLocal(offset);
}

void CPlayerCamera::TurnRight(float angle)
{
	// Translate back to origin
	CVector3f Translate;
	Vec3fScale(&Translate, &m_CameraFrame.m_Position, -1.0f);
	m_CameraFrame.TranslateWorld(&Translate);

	// Rotate against world Y
	m_CameraFrame.RotateWorldY(-angle);

	// Translate back
	Vec3fScale(&Translate, &Translate, -1.0f);
	m_CameraFrame.TranslateWorld(&Translate);
}

void CPlayerCamera::TurnUp(float angle)
{
	float pitch = m_Pitch + angle;
	if (pitch > m_MaxPitch)
		pitch = m_MaxPitch;
	else if (pitch < m_MinPitch)
		pitch = m_MinPitch;

	angle = pitch - m_Pitch;
	m_Pitch += angle;
	m_CameraFrame.RotateLocalX(-angle);
}

void CPlayerCamera::Update(void)
{
	CVector3f pos0, offset;
	if (m_Player)
	{
		m_Player->GetPosition(&pos0, 0);
		m_CameraFrame.SetPosition(&pos0);
		m_CameraFrame.TranslateLocal(&m_Offset);
	}
}

void CPlayerCamera::ApplyTransform(void)
{
	CMatrix44 mat;
	CQuaternion inv;

	Update();

	// Do inverse transform, rotation only
	QuatConjugate(&inv, &m_CameraFrame.m_Orientation);
	Mat44LoadIdentity(&mat);
	QuatToRotationMatrix(&mat, &inv);

	// Need to flip z and x
	mat.right.z = -mat.right.z;
	mat.up.z = -mat.up.z;
	mat.at.z = -mat.at.z;
	mat.right.x = -mat.right.x;
	mat.up.x = -mat.up.x;
	mat.at.x = -mat.at.x;
	glMultMatrixf((float *)&mat);

	// Now inverse translate
	glTranslatef(-m_CameraFrame.m_Position.x, -m_CameraFrame.m_Position.y, -m_CameraFrame.m_Position.z);
}

void CPlayerCamera::GetInverseTransformMatrix(CMatrix44 *mat)
{
	Mat44LoadIdentity(mat);
	QuatToRotationMatrix(mat, &m_CameraFrame.m_Orientation);

	// Need to flip z and x
	Vec3fScale(&mat->right, &mat->right, -1.0f);
	Vec3fScale(&mat->at, &mat->at, -1.0f);
	mat->pos = m_CameraFrame.m_Position;
}

void CPlayerCamera::GetPosition(CVector3f *pos)
{
	m_CameraFrame.GetPosition(pos);
}

void CPlayerCamera::GetLookAt(CVector3f *v)
{
	m_CameraFrame.GetForward(v);
}

void CPlayerCamera::GetUp(CVector3f *v)
{
	m_CameraFrame.GetUp(v);
}

////////////////////////////////////////////////
// Flyby camera
////////////////////////////////////////////////
CFlyByCamera::CFlyByCamera(void)
{
	m_FrameCount = 0;
	m_ControlFrames = 0;
	m_StartTime = -1.0f;
	m_Length = 0;
}

CFlyByCamera::~CFlyByCamera(void)
{
	if (m_ControlFrames)
	{
		delete [] m_ControlFrames;
		m_ControlFrames = 0;
	}
}

void CFlyByCamera::Update(void)
{
	if (m_StartTime == -1.0f)
		m_StartTime = g_time;

	if (m_StartTime > 0.0f && m_FrameCount > 0)
	{
		float t = g_time - m_StartTime;
		int segment;

		if (t >= 0.0f)
		{
			segment = int(t / m_UpdatePeriod);

			if (segment > m_FrameCount - 3)
				segment = m_FrameCount - 3;

			if (m_ControlFrames[segment + 1] == m_ControlFrames[segment + 2])
				return;

			if (t > m_Length)
				t = 1.0f;
			else
				t = (t - segment * m_UpdatePeriod) / m_UpdatePeriod;
			ObjFrameInterpolate(&m_CameraFrame, m_ControlFrames, segment + 1, t);
		}
	}
}

void CFlyByCamera::SetUp(int framecount, CObjFrame *frames, float t)
{
	m_StartTime = -1.0f;
	m_FrameCount = framecount + 2;

	if (m_ControlFrames)
		delete [] m_ControlFrames;

	m_ControlFrames = new CObjFrame[m_FrameCount];
	for (int i = 1; i < m_FrameCount - 1; i++)
		m_ControlFrames[i] = frames[i - 1];

	m_ControlFrames[0] = m_ControlFrames[1];
	m_ControlFrames[m_FrameCount - 1] = m_ControlFrames[m_FrameCount - 2];
	m_Length = t;
	m_UpdatePeriod = t / (framecount - 1);

	m_CameraFrame = m_ControlFrames[0];
}

void CFlyByCamera::ApplyTransform(void)
{
	CMatrix44 mat;
	CQuaternion inv;

	Update();

	// Do inverse transform, rotation only
	QuatConjugate(&inv, &m_CameraFrame.m_Orientation);
	Mat44LoadIdentity(&mat);
	QuatToRotationMatrix(&mat, &inv);

	// Need to flip z and x
	mat.right.z = -mat.right.z;
	mat.up.z = -mat.up.z;
	mat.at.z = -mat.at.z;
	mat.right.x = -mat.right.x;
	mat.up.x = -mat.up.x;
	mat.at.x = -mat.at.x;
	glMultMatrixf((float *)&mat);

	// Now inverse translate
	glTranslatef(-m_CameraFrame.m_Position.x, -m_CameraFrame.m_Position.y, -m_CameraFrame.m_Position.z);
}

void CFlyByCamera::GetInverseTransformMatrix(CMatrix44 *mat)
{
	Mat44LoadIdentity(mat);
	QuatToRotationMatrix(mat, &m_CameraFrame.m_Orientation);

	// Need to flip z and x
	Vec3fScale(&mat->right, &mat->right, -1.0f);
	Vec3fScale(&mat->at, &mat->at, -1.0f);
	mat->pos = m_CameraFrame.m_Position;
}

void CFlyByCamera::GetPosition(CVector3f *pos)
{
	m_CameraFrame.GetPosition(pos);
}

float CFlyByCamera::GetElapsedTime(void) const
{
	if (m_StartTime == -1.0f)
		return 0.0f;
	else
		return g_time - m_StartTime;
}
