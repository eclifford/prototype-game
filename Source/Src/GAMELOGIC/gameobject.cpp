#include "gamelogic\gameobject.h"
#include "graphics\quadtree.h"
#include "math3d\intersection.h"
#include "graphics\terrain.h"
#include "physics\physics.h"

#include "utils\mmgr.h"

extern CTerrain *g_pCurrLevel;

void CGameObject::GetVelocity(CVector3f *v)
{
	if (m_RigidBody)
		m_RigidBody->GetVelocity(v);
	else
	{
		Vec3fZero(v);
	}
}

CGameObject::CGameObject(CGameObject& GameObject)
{
    m_RenderObj.Initialize(GameObject.m_RenderObj);
    m_RenderObj.m_pOwner = this;
    m_RigidBody = NULL;
    m_Type = GameObject.m_Type;
    m_Active = GameObject.m_Active;
    m_OBB = GameObject.m_OBB;
    m_OBB.Frame = m_RenderObj.GetFrame(0);
    m_GuyNextToMe = m_Next = m_Prev = m_CollisionAvoidanceListNext = m_CollisionAvoidanceListPrev = NULL;

    m_InMiniMap = false;
	m_StaticObj = false;
}

void CGameObject::Initialize(unsigned int GeometryIndex, unsigned int NumParts,
                      CVector3f* pTranslations, int TextureIndex, const Sphere* pSphere,
                      AABB* pAABB, CQuadtree* pQuadtree)
{
    m_RenderObj.Initialize(pSphere, GeometryIndex, NumParts, pTranslations, TextureIndex);
    m_OBB.Frame = m_RenderObj.GetFrame(0);
    m_OBB.x = pAABB->Max.x;
    m_OBB.y = pAABB->Max.y;
    m_OBB.z = pAABB->Max.z;
    m_RenderObj.m_pOwner = this;

    m_InMiniMap = false;
	m_StaticObj = false;
}

CGameObject::~CGameObject(void)
{
	if (m_RigidBody)
	{
		delete m_RigidBody;
		m_RigidBody = 0;
	}
}

void CGameObject::Unload()
{
    if (m_RigidBody)
	{
		delete m_RigidBody;
		m_RigidBody = 0;
	}
	
	m_RenderObj.Unload();
}

void CGameObject::Update()
{
   m_InMiniMap = false;
   m_RenderObj.Update();
}

void CGameObject::MoveRight(float Dist, unsigned int PartIndex)
{
   m_RenderObj.MoveRight(Dist, PartIndex);
}
void CGameObject::MoveUp(float Dist, unsigned int PartIndex)
{
   m_RenderObj.MoveUp(Dist, PartIndex);
}
void CGameObject::MoveForward(float Dist, unsigned int PartIndex)
{
   m_RenderObj.MoveForward(Dist, PartIndex);
}

void CGameObject::RotateLocalX(float Angle, unsigned int PartIndex)
{
   m_RenderObj.RotateLocalX(Angle, PartIndex);
}

void CGameObject::RotateLocalY(float Angle, unsigned int PartIndex)
{
   m_RenderObj.RotateLocalY(Angle, PartIndex);
}

void CGameObject::RotateLocalZ(float Angle, unsigned int PartIndex)
{
   m_RenderObj.RotateLocalZ(Angle, PartIndex);
}

void CGameObject::GetPosition(CVector3f* pPosition, unsigned int PartIndex)
{
   m_RenderObj.GetPosition(pPosition, PartIndex);
}

void CGameObject::SetPosition(const Point3f* pPosition, unsigned int PartIndex)
{
   m_RenderObj.SetPosition(pPosition, PartIndex);
}

void CGameObject::RemoveFromQuadtree()
{
   m_RenderObj.RemoveFromQuadtree();
}

void CGameObject::SetObjectPosition(float x, float y, float z)
{
   MoveRight(x, 0);
   MoveUp(y, 0);
   MoveForward(z, 0);
}

void CGameObject::RBMoveForward(void)
{
	if (m_RigidBody)
		m_RigidBody->MoveForward();
}

void CGameObject::RBMoveBackward(void)
{
	if (m_RigidBody)
		m_RigidBody->MoveBackward();
}

void CGameObject::RBMoveLeft(void)
{
	if (m_RigidBody)
		m_RigidBody->MoveLeft();
}

void CGameObject::RBMoveRight(void)
{
	if (m_RigidBody)
		m_RigidBody->MoveRight();
}

void CGameObject::RBTurnRight(float MouseX)
{
	if (m_RigidBody)
		m_RigidBody->TurnRight(MouseX);
}

void CGameObject::RBMoveUp()
{
	if (m_RigidBody)
		m_RigidBody->MoveUp();
}

void CGameObject::FollowTerrain(void)
{
	Plane p;
	float offset;

	// Clamp it to ground first
	CObjFrame frame = *m_OBB.Frame;
	CVector3f pos;

	if (m_RigidBody)
		offset = m_RigidBody->GetTerrainOffset();
	else
		offset = 0.0f;

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
	front.y += ClampToPlane(&front, &p, offset);
	
	CVector3f backleft(hw, -hh, -hl);
	frame.RotateToWorldPoint(&backleft, &backleft);
	g_pCurrLevel->GetPlaneAtPoint(backleft.x, backleft.z, &p);
	backleft.y += ClampToPlane(&backleft, &p, offset);

	CVector3f backright(-hw, -hh, -hl);
	frame.RotateToWorldPoint(&backright, &backright);
	g_pCurrLevel->GetPlaneAtPoint(backright.x, backright.z, &p);
	backright.y += ClampToPlane(&backright, &p, offset);

	GetOrientationFromPoints(&q, &front, &backleft, &backright);

	frame.SetOrientation(&q);
	SetFrame(&frame, 0);
}

/*
void CGameObject::FollowTerrain(float offset)
{
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
}*/


