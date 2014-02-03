/// \file stationaryobjects.cpp
/// The base class for all stationary objects
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/stationaryobjects.h"
#include "graphics\terrain.h"
#include "math3d\intersection.h"

#include "utils\mmgr.h"

extern CTerrain *g_pCurrLevel;

CStationaryObject::CStationaryObject()
{

}

CStationaryObject::~CStationaryObject()
{

}

void CStationaryObject::FollowTerrain(void)
{
   float offset = 0.0f;

   Plane p;

	// Clamp it to ground first
	CObjFrame frame = *m_OBB.Frame;
	CVector3f pos;

	frame.GetPosition(&pos);
	g_pCurrLevel->GetPlaneAtPoint(pos.x, pos.z, &p);
	pos.y += ClampToPlane(&pos, &p, m_OBB.y + offset);
	frame.SetPosition(&pos);

	// Figure out orientation
	float hw = m_OBB.x;
	float hh = m_OBB.y;
	float hl = m_OBB.z;

	CQuaternion q;

	CVector3f front(0.0f, -hh, hl);
	frame.RotateToWorldPoint(&front, &front);
	g_pCurrLevel->GetPlaneAtPoint(front.x, front.z, &p);
	front.y += ClampToPlane(&front, &p, 0.0f);
	
	CVector3f backleft(hw, -hh, -hl);
	frame.RotateToWorldPoint(&backleft, &backleft);
	g_pCurrLevel->GetPlaneAtPoint(backleft.x, backleft.z, &p);
	backleft.y += ClampToPlane(&backleft, &p, 0.0f);

	CVector3f backright(-hw, -hh, -hl);
	frame.RotateToWorldPoint(&backright, &backright);
	g_pCurrLevel->GetPlaneAtPoint(backright.x, backright.z, &p);
	backright.y += ClampToPlane(&backright, &p, 0.0f);

	GetOrientationFromPoints(&q, &front, &backleft, &backright);

	frame.SetOrientation(&q);
	SetFrame(&frame, 0);
}