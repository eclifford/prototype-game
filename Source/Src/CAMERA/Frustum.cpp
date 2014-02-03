#include <math.h>
#include <memory.h>
#include "camera\frustum.h"
#include "math3d\intersection.h"

#include "utils\mmgr.h"

CFrustum::CFrustum(void)
{
	m_FoV = 0.0f;
	m_AspectRatio = 1.0f;
	m_Near = m_Far = 0.0f;
}

CFrustum::CFrustum(float fov, float aspect, float nearp, float farp)
{
	InitFrustum(fov, aspect, nearp, farp);
}

void CFrustum::InitFrustum(float fov, float aspect, float nearp, float farp)
{
	m_FoV = fov;
	m_AspectRatio = aspect;
	m_Near = nearp;
	m_Far = farp;
	SetupFrustum();
}

void CFrustum::SetupFrustum(void)
{
	// Setup corner points
	m_NearLowerLeft.y = -m_Near * (float)tan(m_FoV * PI / 360.0f);
	m_NearLowerLeft.x = m_NearLowerLeft.y * m_AspectRatio;
	m_NearLowerLeft.z = -m_Near;

	m_FarUpperRight.y = m_Far * (float)tan(m_FoV * PI / 360.0f);
	m_FarUpperRight.x = m_FarUpperRight.y * m_AspectRatio;
	m_FarUpperRight.z = -m_Far;

	// Setup normals
	CVector3f e1, e2;

	// Near and far planes
	m_NearNorm.x = m_NearNorm.y = 0.0f;
	m_NearNorm.z = -1.0f;
	m_FarNorm.x = m_FarNorm.y = 0.0f;
	m_FarNorm.z = 1.0f;

	// Top plane
	e1.x = -m_NearLowerLeft.x - m_FarUpperRight.x;
	e1.y = -m_NearLowerLeft.y - m_FarUpperRight.y;
	e1.z = m_NearLowerLeft.z - m_FarUpperRight.z;

	e2.x = -m_FarUpperRight.x * 2.0f;
	e2.y = e2.z = 0.0f;
	Vec3fCrossProduct(&m_TopNorm, &e1, &e2);
	Vec3fNormalize(&m_TopNorm, &m_TopNorm);

	// Right plane
	e2.y = -m_FarUpperRight.y * 2.0f;
	e2.x = e2.z = 0.0f;

	Vec3fCrossProduct(&m_RightNorm, &e2, &e1);
	Vec3fNormalize(&m_RightNorm, &m_RightNorm);

	// Bottom plane
	e1.x = -m_NearLowerLeft.x * 2.0f;
	e1.y = e1.z = 0.0f;

	e2.x = -m_FarUpperRight.x - m_NearLowerLeft.x;
	e2.y = -m_FarUpperRight.y - m_NearLowerLeft.y;
	e2.z = m_FarUpperRight.z - m_NearLowerLeft.z;

	Vec3fCrossProduct(&m_BottomNorm, &e1, &e2);
	Vec3fNormalize(&m_BottomNorm, &m_BottomNorm);

	// Left plane
	e1.x = e1.z = 0.0f;
	e1.y = -m_NearLowerLeft.y * 2.0f;

	Vec3fCrossProduct(&m_LeftNorm, &e2, &e1);
	Vec3fNormalize(&m_LeftNorm, &m_LeftNorm);
}

void CFrustum::TransformFrustum(CBaseCamera *camera)
{
	// Need to undo camera transform
	CMatrix44 mat;
	camera->GetInverseTransformMatrix(&mat);

	// Inverse rotate and transform of points
	Mat44MultiplyVector(&m_FarUpperRightT, &mat, &m_FarUpperRight);
	Mat44MultiplyVector(&m_NearLowerLeftT, &mat, &m_NearLowerLeft);

	// Only need to inverse rotate normals
	Vec3fZero(&mat.pos);
	Mat44MultiplyVector(&m_NearNormT, &mat, &m_NearNorm);
	Mat44MultiplyVector(&m_FarNormT, &mat, &m_FarNorm);
	Mat44MultiplyVector(&m_TopNormT, &mat, &m_TopNorm);
	Mat44MultiplyVector(&m_BottomNormT, &mat, &m_BottomNorm);
	Mat44MultiplyVector(&m_LeftNormT, &mat, &m_LeftNorm);
	Mat44MultiplyVector(&m_RightNormT, &mat, &m_RightNorm);
}

bool CFrustum::TestSphere(const CVector3f *pt, float r)
{
	if (PointToPlaneDistance(pt, &m_NearLowerLeftT, &m_NearNormT) < -r)
		return false;
	if (PointToPlaneDistance(pt, &m_FarUpperRightT, &m_FarNormT) < -r)
		return false;
	if (PointToPlaneDistance(pt, &m_FarUpperRightT, &m_TopNormT) < -r)
		return false;
	if (PointToPlaneDistance(pt, &m_NearLowerLeftT, &m_BottomNormT) < -r)
		return false;
	if (PointToPlaneDistance(pt, &m_NearLowerLeftT, &m_LeftNormT) < -r)
		return false;
	if (PointToPlaneDistance(pt, &m_FarUpperRightT, &m_RightNormT) < -r)
		return false;
	return true;
}

int CFrustum::TestAABB(const CVector3f *max, const CVector3f *min)
{
	// Generate the vertices
	CVector3f vertices[8];
	int InFrustumScore = 0, FrontHalfSpaceScores[6], CurrentVertexInScore;

	memset(FrontHalfSpaceScores, 0, sizeof(int) * 6);

	vertices[0] = *max;

	vertices[1].x = max->x;
	vertices[1].y = max->y;
	vertices[1].z = min->z;

	vertices[2].x = max->x;
	vertices[2].y = min->y;
	vertices[2].z = max->z;

	vertices[3].x = max->x;
	vertices[3].y = min->y;
	vertices[3].z = min->z;

	vertices[4].x = min->x;
	vertices[4].y = max->y;
	vertices[4].z = max->z;

	vertices[5].x = min->x;
	vertices[5].y = max->y;
	vertices[5].z = min->z;

	vertices[6].x = min->x;
	vertices[6].y = min->y;
	vertices[6].z = max->z;

	vertices[7] = *min;

	// Test and get the scores
	for (int i = 0; i < 8; i++)
	{
		CurrentVertexInScore = 0;
		if (PointToPlaneDistance(&vertices[i], &m_NearLowerLeftT, &m_NearNormT) >= 0.0f)
		{
			// In front of this plane
			CurrentVertexInScore++;
			FrontHalfSpaceScores[0]++;
		}
		if (PointToPlaneDistance(&vertices[i], &m_FarUpperRightT, &m_FarNormT) >= 0.0f)
		{
			// In front of this plane
			CurrentVertexInScore++;
			FrontHalfSpaceScores[1]++;
		}
		if (PointToPlaneDistance(&vertices[i], &m_FarUpperRightT, &m_TopNormT) >= 0.0f)
		{
			// In front of this plane
			CurrentVertexInScore++;
			FrontHalfSpaceScores[2]++;
		}
		if (PointToPlaneDistance(&vertices[i], &m_NearLowerLeftT, &m_BottomNormT) >= 0.0f)
		{
			// In front of this plane
			CurrentVertexInScore++;
			FrontHalfSpaceScores[3]++;
		}
		if (PointToPlaneDistance(&vertices[i], &m_NearLowerLeftT, &m_LeftNormT) >= 0.0f)
		{
			// In front of this plane
			CurrentVertexInScore++;
			FrontHalfSpaceScores[4]++;
		}
		if (PointToPlaneDistance(&vertices[i], &m_FarUpperRightT, &m_RightNormT) >= 0.0f)
		{
			// In front of this plane
			CurrentVertexInScore++;
			FrontHalfSpaceScores[5]++;
		}

		if (CurrentVertexInScore == 6)
			InFrustumScore++;
	}

	// If all are in, then FRUSTUM_IN
	if (InFrustumScore == 8)
		return FRUSTUM_IN;
	// If some are in, then FRUSTUM_INTERSECT
	else if (InFrustumScore > 0)
		return FRUSTUM_INTERSECT;
	else
	{
		// If all are out and a separating plane can be found, then FRUSTUM_OUT
		// else FRUSTUM_INTERSECT
		for (int j = 0; j < 6; j++)
		{
			if (FrontHalfSpaceScores[j] == 0)
				return FRUSTUM_OUT;
		}
	}

	return FRUSTUM_INTERSECT;
}

