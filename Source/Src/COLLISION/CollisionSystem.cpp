#include "collision\collisionsystem.h"
#include "graphics\terrain.h"
#include "math3d\intersection.h"
#include "physics\physics.h"

#include "utils\mmgr.h"

extern CTerrain *g_pCurrLevel;

CCollisionSystem::CCollisionSystem(void)
{
	m_Pruner.Init(50);
}

void CCollisionSystem::CheckTerrainCollision(CGameObject *obj)
{
	float d;
	const Sphere *sphere = obj->GetSphere();
	Plane p;

	if (g_pCurrLevel->GetPlaneAtPoint(sphere->Center.x, sphere->Center.z, &p))
	{
		d = PointToPlaneDistance(&sphere->Center, &p);

		if (obj->GetType() == OBJ_FIGHTER)
			d *= 1.5f;

		if (d <= sphere->Radius)
			obj->Collide(0);
	}
}

void CCollisionSystem::CheckCollisions(CGameObject *list)
{
	OBB *boxA, *boxB;
	CRigidBody *bodyA, *bodyB;
	CGameObject *obj = list;

	while (obj)
	{
		if (obj->IsActive())
		{
			bodyA = obj->GetRigidBody();
			if (!bodyA)
				CheckTerrainCollision(obj);
			else
			{	
				bodyA->DetectTerrainCollision();
				bodyA->HandleTerrainCollision();
			}

			if (obj->IsActive())
				m_Pruner.AddObj(obj);
		}
		obj = obj->m_Next;
	}

	RDC_Pair *pairs = m_Pruner.CheckCollisions();

	while (pairs)
	{
		if (pairs->objA->IsActive() && pairs->objB->IsActive())
		{
			boxA = pairs->objA->GetOBB();
			boxB = pairs->objB->GetOBB();

			if (OBBToOBB(boxA, boxB))
			{
				pairs->objA->Collide(pairs->objB);
				pairs->objB->Collide(pairs->objA);

				bodyA = pairs->objA->GetRigidBody();
				bodyB = pairs->objB->GetRigidBody();
				if (bodyA && bodyB)
				{
					Plane p;
					CVector3f v1, v2, vr;
					const Sphere *s1 = pairs->objA->GetSphere();
					const Sphere *s2 = pairs->objB->GetSphere();

					SphereToSphereContactPlane(&p, s1, s2);
					bodyA->GetVelocity(&v1);
					bodyB->GetVelocity(&v2);
					Vec3fSubtract(&vr, &v1, &v2);

					if (Vec3fDotProduct(&vr, &p.Normal) < 0.0f)
					{
						CollisionData collision;

						collision.Body1 = bodyA;
						collision.Body2 = bodyB;

						collision.CollisionNormal = p.Normal;
						collision.CollisionPoint = p.Point;
						collision.RelativeVelocity = vr;

						float J = CalcImpulse(&collision);

						// Clamp it a bit
						if (J > 3000.0f)
							J = 3000.0f;
						else if (J < -3000.0f)
							J = -3000.0f;

						bodyA->ApplyImpulse(J, &collision);
						bodyB->ApplyImpulse(-J, &collision);
					}
				}
				else 
				{
					// Collide with static objs
					if (bodyA && pairs->objB->IsStaticObj())
					{
						Plane p;
						CVector3f vr;
						Sphere s1 = *pairs->objA->GetSphere();
						Sphere s2 = *pairs->objB->GetSphere();
						s2.Center.y = s1.Center.y;

						SphereToSphereContactPlane(&p, &s1, &s2);
						bodyA->GetVelocity(&vr);

						if (Vec3fDotProduct(&vr, &p.Normal) <= 0.0f)
						{
							CollisionData collision;

							collision.Body1 = bodyA;
							collision.Body2 = 0;

							collision.CollisionNormal = p.Normal;
							collision.CollisionPoint = p.Point;
							collision.RelativeVelocity = vr;

							float J = CalcImpulse(&collision);

							bodyA->ApplyImpulse(J, &collision);
						}
					}
					else if (bodyB && pairs->objA->IsStaticObj())
					{
						Plane p;
						CVector3f vr;
						Sphere s1 = *pairs->objA->GetSphere();
						Sphere s2 = *pairs->objB->GetSphere();
						s1.Center.y = s2.Center.y;

						SphereToSphereContactPlane(&p, &s2, &s1);
						bodyB->GetVelocity(&vr);

						if (Vec3fDotProduct(&vr, &p.Normal) <= 0.0f)
						{
							CollisionData collision;

							collision.Body1 = bodyB;
							collision.Body2 = 0;

							collision.CollisionNormal = p.Normal;
							collision.CollisionPoint = p.Point;
							collision.RelativeVelocity = vr;

							float J = CalcImpulse(&collision);

							bodyB->ApplyImpulse(J, &collision);
						}
					}
				}
			}
		}
		pairs = pairs->next;
	}

	m_Pruner.ReleaseAllPairs();
}

bool CCollisionSystem::CheckForAnyCollision(CGameObject *list)
{
	CGameObject *obj = list;

	while (obj)
	{
		m_Pruner.AddObj(obj);
		obj = obj->m_Next;
	}

	return m_Pruner.CheckForAnyCollision();
}
