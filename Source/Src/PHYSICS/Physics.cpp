//////////////////////////////////////////////////
// File: Physics.cpp
// 
// Author: Kin Wai Choi
//
// Purpose: Rigid body implementation  
//////////////////////////////////////////////////
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include "physics\physics.h"
#include "math3d\intersection.h"
#include "gamelogic\gameobject.h"
#include "graphics\terrain.h"
#include <float.h>

#include "utils\mmgr.h"

extern CTerrain *g_pCurrLevel;

// Constructor
CRigidBody::CRigidBody(void)
{
	m_LoadCount = 0;
	m_Loads = NULL;
}

// Destructor
CRigidBody::~CRigidBody(void)
{
	if (m_Loads)
	{
		delete [] m_Loads;
		m_Loads = NULL;
	}
}

// Initialization
void CRigidBody::Initialize(CGameObject *owner, float Mass, float Width, float Height, float Length)
{
	this->owner = owner;
	this->frame = const_cast<CObjFrame *>(owner->GetFrame(0));
	m_LoadCount = RB_NUMTHRUSTERS;
	m_Loads = new AppliedForce[m_LoadCount];

	// Mass
	m = Mass;

	// Inertia tensor
	float ww = Width * Width;
	float hh = Height * Height;
	float ll = Length * Length;

	memset(&IBody, 0, sizeof(CMatrix33));
	IBody.m00 = m / 12.0f * (hh + ll); // Ixx
	IBody.m11 = m / 12.0f * (ww + ll); // Iyy
	IBody.m22 = m / 12.0f * (hh + ww); // Izz

	// Inverse of inertia tensor
	memset(&IInverseBody, 0, sizeof(CMatrix33));
	IInverseBody.m00 = 1.0f / IBody.m00;
	IInverseBody.m11 = 1.0f / IBody.m11;
	IInverseBody.m22 = 1.0f / IBody.m22;

	// Zero everything
	Vec3fZero(&P);
	Vec3fZero(&L);
	Vec3fZero(&v);
	Vec3fZero(&w);
	Vec3fZero(&m_TurnMoment);
	Vec3fZero(&m_ExternalMoment);

	SetupHardPoints(Width, Height, Length);
	SetupThrusters(Width, Height, Length);
	m_MomentScaleFactor = 500.0f;
	m_TerrainOffset = 1.0f;
	m_AngularDamping2 = 1.75f;
	m_FlipTime = 0.0f;

	m_MaxTurnSpeed = PI / 3.0f;
	SetParameters(70.0f, 2.0f);
}

// Activate forward thruster
void CRigidBody::MoveForward(void)
{
	m_Loads[RB_FORWARD].Active = true;
}

// Activate backward thruster
void CRigidBody::MoveBackward(void)
{
	m_Loads[RB_BACKWARD].Active = true;
}

// Activate right thruster
void CRigidBody::MoveLeft(void)
{
	m_Loads[RB_RIGHT].Active = true;
}

// Activate left thruster
void CRigidBody::MoveRight(void)
{
	m_Loads[RB_LEFT].Active = true;
}

// Activate jump jet thruster
void CRigidBody::MoveUp(void)
{
	m_Loads[RB_UP].Active = true;
}

// Turning
void CRigidBody::TurnRight(float angle)
{
	float omega = 10.0f * angle;
	if (omega > m_MaxTurnSpeed)
		omega = m_MaxTurnSpeed;
	else if (omega < -m_MaxTurnSpeed)
		omega = -m_MaxTurnSpeed;

	CVector3f alpha(0.0f, -10.0f * omega, 0.0f), moment;
	Mat33MultiplyVector(&moment, &IBody, &alpha);
	Vec3fAdd(&m_TurnMoment, &m_TurnMoment, &moment);
}

// Stop turning
void CRigidBody::StopTurning(void)
{
	CVector3f l;
	frame->RotateToLocalVector(&l, &L);
	l.y = 0.0f;
	frame->RotateToWorldVector(&L, &l);
}

// Add friction
void CRigidBody::ApplyFriction(void)
{
	CVector3f dir, f;

	Vec3fNormalize(&dir, &v);
	Vec3fScale(&f, &dir, -m * 5.0f);
	f.y = 0.0f;
	Vec3fAdd(&F, &F, &f);
}

// External moment for flipping
void CRigidBody::ApplyExternalMoment(const CVector3f *dir, float mag)
{
	CVector3f moment;

	// Assume dir is normalized
	Vec3fScale(&moment, dir, mag);
	Mat33MultiplyVector(&moment, &I, &moment);
	Vec3fAdd(&m_ExternalMoment, &m_ExternalMoment, &moment);
}

// External momentum 
void CRigidBody::ApplyExternalMomentum(const CVector3f *v)
{
	CVector3f dP;

	Vec3fScale(&dP, v, m);
	Vec3fAdd(&P, &P, &dP);
}

// Set thruster strength
void CRigidBody::SetThruster(int thruster, float force)
{
	if (thruster >= 0 && thruster < RB_NUMTHRUSTERS)
		m_Loads[thruster].Magnitude = force;
}

// Setup scale
void CRigidBody::SetTurnMomentScale(float scale)
{
	m_MomentScaleFactor = scale;
}

// Setup thrusters and calculate damping based on parameters
void CRigidBody::SetParameters(float maxforwardspd, float accelerationtime)
{
	float a = maxforwardspd / accelerationtime;
	m_Loads[RB_FORWARD].Magnitude = m * a;
	m_Loads[RB_BACKWARD].Magnitude = m_Loads[RB_FORWARD].Magnitude * 0.5f;
	m_Loads[RB_LEFT].Magnitude = m_Loads[RB_RIGHT].Magnitude = m_Loads[RB_BACKWARD].Magnitude;

	m_LinearDamping2 = m_Loads[RB_FORWARD].Magnitude / (maxforwardspd * maxforwardspd);

	if (maxforwardspd >= 100.0f)
			m_AngularDamping2 = 4.0f;
	else
			m_AngularDamping2 = 2.2f;
}

// Setup hard points in local space
void CRigidBody::SetupHardPoints(float Width, float Height, float Length)
{
	// Just use the corners of the BB for now
	m_HardPointCount = 8;

	// Set up all points
	float w = Width / 2.0f;
	float h = Height / 2.0f;
	float l = Length / 2.0f;

	// Front corners
	m_HardPoints[0].x = -w;
	m_HardPoints[0].y = -h;
	m_HardPoints[0].z = l;

	m_HardPoints[1].x = w;
	m_HardPoints[1].y = -h;
	m_HardPoints[1].z = l;

	m_HardPoints[2].x = w;
	m_HardPoints[2].y = h;
	m_HardPoints[2].z = l;

	m_HardPoints[3].x = -w;
	m_HardPoints[3].y = h;
	m_HardPoints[3].z = l;

	// Back corners
	m_HardPoints[4].x = -w;
	m_HardPoints[4].y = -h;
	m_HardPoints[4].z = -l;

	m_HardPoints[5].x = w;
	m_HardPoints[5].y = -h;
	m_HardPoints[5].z = -l;

	m_HardPoints[6].x = w;
	m_HardPoints[6].y = h;
	m_HardPoints[6].z = -l;

	m_HardPoints[7].x = -w;
	m_HardPoints[7].y = h;
	m_HardPoints[7].z = -l;
}

// Setup default thrusters
void CRigidBody::SetupThrusters(float Width, float Height, float Length)
{
	m_Loads[RB_FORWARD].Magnitude = 666.0f;
	m_Loads[RB_FORWARD].Direction = CVector3f(0.0f, 0.0f, 1.0f);
	m_Loads[RB_FORWARD].PointOfAction = CVector3f(0.0f, 0.0f, -Length / 2.0f);
	m_Loads[RB_FORWARD].Active = false;

	m_Loads[RB_BACKWARD].Magnitude = 666.0f;
	m_Loads[RB_BACKWARD].Direction = CVector3f(0.0f, 0.0f, -1.0f);
	m_Loads[RB_BACKWARD].PointOfAction = CVector3f(0.0f, 0.0f, -Length / 2.0f);
	m_Loads[RB_BACKWARD].Active = false;

	m_Loads[RB_LEFT].Magnitude = 666.0f;
	m_Loads[RB_LEFT].Direction = CVector3f(-1.0f, 0.0f, 0.0f);
	m_Loads[RB_LEFT].PointOfAction = CVector3f(Width / 2.0f, 0.0f, 0.0f);
	m_Loads[RB_LEFT].Active = false;

	m_Loads[RB_RIGHT].Magnitude = 666.0f;
	m_Loads[RB_RIGHT].Direction = CVector3f(1.0f, 0.0f, 0.0f);
	m_Loads[RB_RIGHT].PointOfAction = CVector3f(-Width / 2.0f, 0.0f, 0.0f);
	m_Loads[RB_RIGHT].Active = false;

	m_Loads[RB_UP].Magnitude = 666.0f;
	m_Loads[RB_UP].Direction = CVector3f(0.0f, 1.0f, 0.0f);
	m_Loads[RB_UP].PointOfAction = CVector3f(0.0f, 0.0f, 0.0f);
	m_Loads[RB_UP].Active = false;
}

// Apply damping to translation
void CRigidBody::ApplyDampingForce(CVector3f *force, CVector3f *vel)
{
	CVector3f f;

	Vec3fScale(&f, vel, -Vec3fMagnitude(vel) * m_LinearDamping2);
	Vec3fAdd(force, force, &f);
}

// Apply damping to rotation
void CRigidBody::ApplyDampingMoment(CVector3f *tau, CVector3f *omega)
{
	CVector3f Moment;

	Vec3fScale(&Moment, omega, -Vec3fMagnitude(omega) * m_AngularDamping2);
	Mat33MultiplyVector(&Moment, &I, &Moment);
	Vec3fAdd(tau, tau, &Moment);
}

// Check if we need to flip
void CRigidBody::CheckFlip(float dt)
{
	CVector3f up, Y(0.0f, 1.0f, 0.0f);
	frame->GetUp(&up);

	if (Vec3fDotProduct(&Y, &up) >= 0.5f)
		m_FlipTime = 0.0f;
	else
	{
		m_FlipTime += dt;
		if (m_FlipTime >= 2.0f)
		{
			// If so, check flip direction
			CVector3f right, forward, omega, dL, dP;
			float dot1, dot2;

			frame->GetRight(&right);
			frame->GetForward(&forward);
		
			dot1 = Vec3fDotProduct(&right, &Y);
			dot2 = Vec3fDotProduct(&forward, &Y);
			if (dot1 > 0.0f)
			{
				frame->GetForward(&omega);
				Vec3fScale(&omega, &omega, -PI * 4.0f);
			}
			else if (dot1 < 0.0f)
			{
				frame->GetForward(&omega);
				Vec3fScale(&omega, &omega, PI * 4.0f);
			}
			else if (dot2 > 0.0f)
			{
				frame->GetRight(&omega);
				Vec3fScale(&omega, &omega, PI * 4.0f);
			}
			else if (dot2 < 0.0f)
			{
				frame->GetRight(&omega);
				Vec3fScale(&omega, &omega, -PI * 4.0f);
			}

			Vec3fScale(&dP, &Y, 0.3f * m);
			Vec3fAdd(&P, &P, &dP);

			Mat33MultiplyVector(&dL, &I, &omega);
			Vec3fAdd(&L, &L, &dL);
			m_FlipTime = 0.0f;
		}
	}
}

// Calculate net force and torque
void CRigidBody::CalcLoads(void)
{
	Vec3fZero(&F);
	Vec3fZero(&T);

	CVector3f Moment, f, temp;
	CVector3f G(0.0f, GRAVITY * m, 0.0f);

	// Terrain contact forces
	HandleTerrainContact();

	// Sideway thrusters
	if (m_ContactCount > 0 || m_CollisionCount > 0)
	{
		for (int i = 0; i < m_LoadCount - 1; i++)
		{
			if (m_Loads[i].Active)
			{
				Vec3fScale(&f, &m_Loads[i].Direction, m_Loads[i].Magnitude);
				frame->RotateToWorldVector(&f, &f);
				frame->RotateToWorldVector(&temp, &m_Loads[i].PointOfAction);
				Vec3fCrossProduct(&Moment, &temp, &f);
				Vec3fAdd(&T, &T, &Moment);
				f.y = 0.0f;
				Vec3fAdd(&F, &F, &f);
				m_Loads[i].Active = false;
			}
		}
	}

	// Upward thruster
	if (m_Loads[RB_UP].Active)
	{
		Vec3fScale(&f, &m_Loads[RB_UP].Direction, m_Loads[RB_UP].Magnitude);
		frame->RotateToWorldVector(&f, &f);
		Vec3fAdd(&F, &F, &f);
		m_Loads[RB_UP].Active = false;
	}

	// Centripetal force
	Vec3fCrossProduct(&f, &w, &v);
	Vec3fScale(&f, &f, m);
	f.y = 0.0f;
	Vec3fAdd(&F, &F, &f);

	// Gravity
	Vec3fAdd(&F, &F, &G);

	// If there is contact, should not move down
	if (m_ContactCount > 0)
	{	
		if (F.y < 0.0f)
			F.y = 0.0f;
	}

	// If there is contact or collision, apply friction
	if (m_ContactCount > 0 || m_CollisionCount > 0)
	{
		ApplyFriction();
	}

	// Add turning
	frame->RotateToWorldVector(&m_TurnMoment, &m_TurnMoment);
	Vec3fAdd(&T, &T, &m_TurnMoment);
	Vec3fZero(&m_TurnMoment);

	// Extern stuff
	Vec3fAdd(&T, &T, &m_ExternalMoment);
	Vec3fZero(&m_ExternalMoment);
}

// Rotate inertia to world space
void CRigidBody::RotateInertia(void)
{
	CMatrix33 Rt;

	QuatToRotationMatrix(&R, &frame->m_Orientation);
	Mat33Transpose(&Rt, &R);

	Mat33Multiply(&IInverse, &R, &IInverseBody);
	Mat33Multiply(&IInverse, &IInverse, &Rt);

	Mat33Multiply(&I, &R, &IBody);
	Mat33Multiply(&I, &I, &Rt);
}

// Move simulation forward
void CRigidBody::StepIntegrate(float dt, bool checkflip)
{
	CVector3f dx, dP, dL, f, vel, tau, omega;
	CQuaternion q, dq;
	CObjFrame newFrame = *frame;
	CVector3f kP1, kP2, kP3, kP4;
	CVector3f kL1, kL2, kL3, kL4;

	// Limit time step
	if (dt > 0.04f)
		dt = 0.04f;

	// Rotate hard points into world space
	RotateHardPoints();

	// I^(-1) = R * Ibody^(-1) * Rt
	RotateInertia();

	// Calculate net force and torque
	CalcLoads();

	float threshold;

	// Check if we need to flip
	if (checkflip)
		CheckFlip(dt);

	// RK4 integration
	// K1
	f = F;
	ApplyDampingForce(&f, &v);
	Vec3fScale(&kP1, &f, dt);

	// K2
	f = F;
	Vec3fScale(&vel, &kP1, 0.5f);
	Vec3fAdd(&vel, &vel, &P);
	Vec3fScale(&vel, &vel, 1.0f / m);
	ApplyDampingForce(&f, &vel);
	Vec3fScale(&kP2, &f, dt);

	// K3
	f = F;
	Vec3fScale(&vel, &kP2, 0.5f);
	Vec3fAdd(&vel, &vel, &P);
	Vec3fScale(&vel, &vel, 1.0f / m);
	ApplyDampingForce(&f, &vel);
	Vec3fScale(&kP3, &f, dt);

	// K2
	f = F;
	Vec3fAdd(&vel, &kP3, &P);
	Vec3fScale(&vel, &vel, 1.0f / m);
	ApplyDampingForce(&f, &vel);
	Vec3fScale(&kP4, &f, dt);

	// Accumulation
	Vec3fScale(&kP2, &kP2, 2.0f);
	Vec3fScale(&kP3, &kP3, 2.0f);

	Vec3fAdd(&dP, &kP1, &kP2);
	Vec3fAdd(&dP, &dP, &kP3);
	Vec3fAdd(&dP, &dP, &kP4);
	Vec3fScale(&dP, &dP, 1.0f / 6.0f);
	Vec3fAdd(&P, &P, &dP);

	// K1
	tau = T;
	ApplyDampingMoment(&tau, &w);
	Vec3fScale(&kL1, &tau, dt);

	// K2
	tau = T;
	Vec3fScale(&omega, &kL1, 0.5f);
	Vec3fAdd(&omega, &omega, &L);
	Mat33MultiplyVector(&omega, &IInverse, &omega);
	ApplyDampingMoment(&tau, &omega);
	Vec3fScale(&kL2, &tau, dt);

	// K3
	tau = T;
	Vec3fScale(&omega, &kL2, 0.5f);
	Vec3fAdd(&omega, &omega, &L);
	Mat33MultiplyVector(&omega, &IInverse, &omega);
	ApplyDampingMoment(&tau, &omega);
	Vec3fScale(&kL3, &tau, dt);

	// K4
	tau = T;
	Vec3fAdd(&omega, &kL3, &L);
	Mat33MultiplyVector(&omega, &IInverse, &omega);
	ApplyDampingMoment(&tau, &omega);
	Vec3fScale(&kL4, &tau, dt);

	// Accumulation
	Vec3fScale(&kL2, &kL2, 2.0f);
	Vec3fScale(&kL3, &kL3, 2.0f);

	Vec3fAdd(&dL, &kL1, &kL2);
	Vec3fAdd(&dL, &dL, &kL3);
	Vec3fAdd(&dL, &dL, &kL4);
	Vec3fScale(&dL, &dL, 1.0f / 6.0f);
	Vec3fAdd(&L, &L, &dL);

	// Fudge, stop turning comletely if momentum is too small
	CVector3f minw(PI / 180.0f, 0.0f, 0.0f), minL;
	Mat33MultiplyVector(&minL, &I, &minw);
	threshold = Vec3fMagnitudeSquare(&minL);
	if (Vec3fMagnitudeSquare(&L) < threshold)
		Vec3fZero(&L);

	// v = P / m;
	Vec3fScale(&v, &P, 1.0f / m);

	// w = I^(-1) * L
	Mat33MultiplyVector(&w, &IInverse, &L);

	// dx = v * dt
	Vec3fScale(&dx, &v, dt);
	newFrame.TranslateWorld(&dx);

	// dq = 0.5 * q * w * dt
	newFrame.GetOrientation(&q);
	CQuaternion qw(0.0f, &w);
	QuatCrossProduct(&dq, &qw, &q);
	QuatScale(&dq, &dq, 0.5f * dt);
	QuatAdd(&q, &q, &dq);
	QuatNormalize(&q, &q);
	newFrame.SetOrientation(&q);

	// Clamp to ground if sinking, bad, but better than blowing up
	Plane p;
	CVector3f pos;
	newFrame.GetPosition(&pos);
	float t;

	if (g_pCurrLevel->GetPlaneAtPoint(pos.x, pos.z, &p))
		t = ClampToPlane(&pos, &p, m_TerrainOffset + owner->GetOBB()->y - COLLISIONTOLERANCE);
	else
		t = -1.0f;

	if (t > 0.0f)
		newFrame.MoveUp(t);

	owner->SetFrame(&newFrame, 0);

	// Don't run off the map
	CheckMapBoundary();
}

// Rotate hard points into world space
void CRigidBody::RotateHardPoints(void)
{
	for (int i = 0; i < m_HardPointCount; i++)
		frame->RotateToWorldPoint(&m_HardPointsWorld[i], &m_HardPoints[i]);
}

// Compute distance fromt terrain
float CRigidBody::CalcDistFromTerrain(const Point3f *pt)
{
	Plane p;
	if (g_pCurrLevel->GetPlaneAtPoint(pt->x, pt->z, &p))
		return -ClampToPlane(pt, &p, 0.0f);
	else
		// Some big number so the test will fail
		return 9999.0f;
}

// Bounce back if running off map
void CRigidBody::CheckMapBoundary(void)
{
	AABB box;
	CVector3f n, pos;
	float J;
	CollisionData collision;

	g_pCurrLevel->GetAABB(&box);
	frame->GetPosition(&pos);

	// Check X
	Vec3fZero(&n);
	if (pos.x < box.Min.x + 50.0f)
	{
		n.x = 1.0f;
		n.y = n.z = 0.0f;
	}
	else if (pos.x > box.Max.x - 50.0f)
	{
		n.x = -1.0f;
		n.y = n.z = 0.0f;
	}

	if (n.x != 0.0f && Vec3fDotProduct(&v, &n) < 0.0f)
	{
		collision.Body1 = this;
		collision.Body2 = 0;

		collision.CollisionNormal = n;
		collision.CollisionPoint = pos;
		collision.RelativeVelocity = v;

		J = CalcImpulse(&collision) * 5.0f;

		// Clamp it a bit
		if (J > 2000.0f)
			J = 2000.0f;
		else if (J < -2000.0f)
			J = -2000.0f;
		ApplyImpulse(J, &collision);
	}

	// Check Z
	Vec3fZero(&n);
	if (pos.z < box.Min.z + 50.0f)
	{
		n.z = 1.0f;
		n.y = n.x = 0.0f;
	}
	else if (pos.z > box.Max.z - 50.0f)
	{
		n.z = -1.0f;
		n.y = n.x = 0.0f;
	}

	if (n.z != 0.0f && Vec3fDotProduct(&v, &n) < 0.0f)
	{
		collision.Body1 = this;
		collision.Body2 = 0;

		collision.CollisionNormal = n;
		collision.CollisionPoint = pos;
		collision.RelativeVelocity = v;

		J = CalcImpulse(&collision) * 5.0f;

		// Clamp it a bit
		if (J > 2000.0f)
			J = 2000.0f;
		else if (J < -2000.0f)
			J = -2000.0f;
		ApplyImpulse(J, &collision);
	}
}

// Handle terrain contact forces
void CRigidBody::HandleTerrainContact(void)
{
	float Distance;
	CVector3f pos, pt, rv, ra, a_n, a_t, a_w;
	Plane p;
	float Vrn, Arn;

	frame->GetPosition(&pos);

	m_ContactCount = 0;

	// Go through each hard point, check distance to terrain
	for (int i = 0; i < m_HardPointCount; i++)
	{
		Distance = CalcDistFromTerrain((Point3f *)&m_HardPointsWorld[i]);
		if (Distance < COLLISIONTOLERANCE + m_TerrainOffset)
		{
			// Check relative velocity
			Vec3fSubtract(&pt, &m_HardPointsWorld[i], &pos);
			Vec3fCrossProduct(&rv, &w, &pt);
			Vec3fAdd(&rv, &rv, &v);

			g_pCurrLevel->GetPlaneAtPoint(m_HardPointsWorld[i].x, m_HardPointsWorld[i].z, &p);
			Vrn = Vec3fDotProduct(&rv, &p.Normal);
			if (fabs(Vrn) < VELOCITYTOLERANCE)
			{
				CVector3f a, a_t;
				Vec3fScale(&a, &F, 1.0f / m);

				Vec3fCrossProduct(&a_t, &w, &pt);
				Vec3fCrossProduct(&a_t, &w, &a_t);

				Vec3fAdd(&ra, &a, &a_t);
				Arn = Vec3fDotProduct(&ra, &p.Normal);

				if (Arn <= 0.0f)
				{
					// Have contact, store info for processing
					m_Collisions[m_ContactCount].CollisionNormal = p.Normal;
					m_Collisions[m_ContactCount].CollisionPoint = m_HardPointsWorld[i];
					m_Collisions[m_ContactCount].RelativeAcceleration = ra;
					m_Collisions[m_ContactCount].RelativeVelocity = rv;

					Vec3fScale(&m_Collisions[m_ContactCount].CollisionTangent, &p.Normal, Vec3fDotProduct(&rv, &p.Normal));
					Vec3fSubtract(&m_Collisions[m_ContactCount].CollisionTangent, &m_Collisions[m_ContactCount].CollisionTangent, &rv);
					Vec3fNormalize(&m_Collisions[m_ContactCount].CollisionTangent, &m_Collisions[m_ContactCount].CollisionTangent);
					m_ContactCount++;
				}
			}
		}
	}

	float cf;
	CVector3f ContactForce, Moment, Torque, f;

	Vec3fZero(&Torque);

	// For all contacts, add contact forces
	for (int k = 0; k < m_ContactCount; k++)
	{
		cf = -m / m_ContactCount * Vec3fDotProduct(&m_Collisions[k].RelativeAcceleration, &m_Collisions[k].CollisionNormal);
		Vec3fScale(&ContactForce, &m_Collisions[k].CollisionNormal, cf);
		Vec3fAdd(&F, &F, &ContactForce);

		Vec3fSubtract(&pt, &m_Collisions[k].CollisionPoint, &pos);
		Vec3fCrossProduct(&Moment, &pt, &ContactForce);
		Vec3fAdd(&Torque, &Torque, &Moment);
	}

	Vec3fAdd(&T, &T, &Torque);
}

// Detect terrain collision
void CRigidBody::DetectTerrainCollision(void)
{
	float Distance;
	CVector3f pos, pt, rv, temp;
	Plane p;
	float Vrn;

	frame->GetPosition(&pos);
	m_CollisionCount = 0;

	// Go through each hard point, check distance to terrain
	for (int i = 0; i < m_HardPointCount; i++)
	{
		Distance = CalcDistFromTerrain((Point3f *)&m_HardPointsWorld[i]);
		if (Distance < COLLISIONTOLERANCE + m_TerrainOffset)
		{
			// Check relative velocity
			Vec3fSubtract(&pt, &m_HardPointsWorld[i], &pos);
			Vec3fCrossProduct(&rv, &w, &pt);
			Vec3fAdd(&rv, &rv, &v);

			g_pCurrLevel->GetPlaneAtPoint(m_HardPointsWorld[i].x, m_HardPointsWorld[i].z, &p);
			Vrn = Vec3fDotProduct(&rv, &p.Normal);

			if (Vrn < -VELOCITYTOLERANCE)
			{
				if (Vrn < -100.0f)
				{
					Vec3fReflect(&P, &P, &p.Normal);
					Vec3fScale(&P, &P, 0.75f);
					return;
				}
				else
				{
					m_Collisions[m_CollisionCount].Body1 = this;
					m_Collisions[m_CollisionCount].Body2 = NULL;
					m_Collisions[m_CollisionCount].CollisionNormal = p.Normal;
					m_Collisions[m_CollisionCount].CollisionPoint = m_HardPointsWorld[i];
					m_Collisions[m_CollisionCount].RelativeVelocity = rv;

					Vec3fScale(&m_Collisions[m_CollisionCount].CollisionTangent, &p.Normal, Vec3fDotProduct(&rv, &p.Normal));
					Vec3fSubtract(&m_Collisions[m_CollisionCount].CollisionTangent, &m_Collisions[m_CollisionCount].CollisionTangent, &rv);
					Vec3fNormalize(&m_Collisions[m_CollisionCount].CollisionTangent, &m_Collisions[m_CollisionCount].CollisionTangent);
					m_CollisionCount++;
				}
			}
		}
	}
}

// Apply impulse for all collisions
void CRigidBody::HandleTerrainCollision(void)
{
	for (int i = 0; i < m_CollisionCount; i++)
	{
		float J = CalcImpulse(&m_Collisions[i]);
		ApplyImpulse(J, &m_Collisions[i]);
	}
}

// Apply an impulse for a collision
void CRigidBody::ApplyImpulse(float J, const CollisionData *collision)
{
	CVector3f dP, dL, pt, f;

	frame->GetPosition(&pt);
	Vec3fSubtract(&pt, &collision->CollisionPoint, &pt);

	Vec3fScale(&dP, &collision->CollisionNormal, J);

	Vec3fAdd(&P, &P, &dP);

	Vec3fCrossProduct(&dL, &pt, &dP);
	Vec3fAdd(&L, &L, &dL);
}

// Calculate impulse for a collision
float CalcImpulse(const CollisionData *collision)
{
	CVector3f Term1, pt1, pos1, temp;
	float Nominator, Denominator, cr;

	collision->Body1->frame->GetPosition(&pos1);
	Vec3fSubtract(&pt1, &collision->CollisionPoint, &pos1);
	Vec3fCrossProduct(&Term1, &pt1, &collision->CollisionNormal);
	Mat33MultiplyVector(&Term1, &collision->Body1->IInverse, &Term1);
	Vec3fCrossProduct(&Term1, &Term1, &pt1);
	Denominator = 1.0f / collision->Body1->m + Vec3fDotProduct(&Term1, &collision->CollisionNormal);

	// If Body 2 is 0, it is hitting a stationary object or the terrain
	if (collision->Body2)
	{
		CVector3f Term2, pt2, pos2;
		cr = COEFFICIENTOFRESTITUTION;
		collision->Body2->frame->GetPosition(&pos2);
		Vec3fSubtract(&pt2, &collision->CollisionPoint, &pos2);
		Vec3fCrossProduct(&Term2, &pt2, &collision->CollisionNormal);
		Mat33MultiplyVector(&Term2, &collision->Body2->IInverse, &Term2);
		Vec3fCrossProduct(&Term2, &Term2, &pt2);
		Denominator += 1.0f / collision->Body2->m + Vec3fDotProduct(&Term2, &collision->CollisionNormal);
	}
	else
	{
		cr = COEFFICIENTOFRESTITUTIONGROUND;
	}

	Nominator = -(1.0f + cr) * Vec3fDotProduct(&collision->RelativeVelocity, &collision->CollisionNormal);

	float J = Nominator / Denominator;

	return J;
}

// Get velocity
void CRigidBody::GetVelocity(CVector3f *vel)
{
	*vel = v;
}


