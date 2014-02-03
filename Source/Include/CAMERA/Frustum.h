#pragma once

#include "camera\camera.h"
#include "math3d\primitives.h"

enum {FRUSTUM_IN, FRUSTUM_INTERSECT, FRUSTUM_OUT};

class CFrustum
{
public:
	float		m_FoV;
	float		m_AspectRatio;
	float		m_Near;
	float		m_Far;

	CVector3f m_NearLowerLeft;		// Points for planes, untransformed
	CVector3f m_FarUpperRight;	

	CVector3f m_NearNorm;			// Normals of clipping planes, untransformed
	CVector3f m_FarNorm;				// All normals point into the frustum
	CVector3f m_RightNorm;
	CVector3f m_LeftNorm;
	CVector3f m_TopNorm;
	CVector3f m_BottomNorm; 

	CVector3f m_NearLowerLeftT;		// Points for planes, transformed
	CVector3f m_FarUpperRightT;	

	CVector3f m_NearNormT;			// Normals of clipping planes, transformed
	CVector3f m_FarNormT;				// All normals point into the frustum
	CVector3f m_RightNormT;
	CVector3f m_LeftNormT;
	CVector3f m_TopNormT;
	CVector3f m_BottomNormT; 

	void SetupFrustum(void);
public:
	CFrustum(void);
	CFrustum(float fov, float aspect, float nearp, float farp);
	void InitFrustum(float fov, float aspect, float nearp, float farp);
	void TransformFrustum(CBaseCamera *camera);
	bool TestSphere(const CVector3f *pt, float r);
	int TestAABB(const CVector3f *max, const CVector3f *min);
};