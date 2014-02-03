#pragma once

#include "math3d\math3d.h"
#include "math3d\objframe.h"

class CGameObject;

// Physical constants
#define GRAVITY -9.8f
#define VELOCITYTOLERANCE 0.3f
#define COEFFICIENTOFRESTITUTION 0.9f
#define COEFFICIENTOFRESTITUTIONGROUND 0.1f

enum {RB_FORWARD = 0, RB_BACKWARD, RB_LEFT, RB_RIGHT, RB_UP, RB_NUMTHRUSTERS};

class CRigidBody;

struct AppliedForce
{
	bool Active;
	float Magnitude;
	CVector3f Direction;			// Force direction, in local space
	CVector3f PointOfAction;	// Point to which force is applied, in local space
};

// Everything should be in world space
struct CollisionData
{
	CRigidBody  *Body1;
	CRigidBody  *Body2;
	CVector3f	CollisionNormal;
	CVector3f	CollisionTangent;
	CVector3f	CollisionPoint;
	CVector3f	RelativeVelocity;
	CVector3f	RelativeAcceleration;
};

class CRigidBody
{
private:

	CGameObject *owner;

	// Constant quantities
	float			m;
	CMatrix33	IBody;
	CMatrix33	IInverseBody;

	// State quantities
	CObjFrame	*frame;	// x and q
	CVector3f	P;			// Linear momentum
	CVector3f	L;			// Angular momentum

	// Derived quantities
	CMatrix33	IInverse;
	CMatrix33	I;
	CMatrix33	R;
	CVector3f	v;
	CVector3f	w;

	// Computed quantities
	CVector3f	F;
	CVector3f	T;

	// Others
	int				m_LoadCount;	// Number of fixed loads
	AppliedForce	*m_Loads;		// Array of loads, specified in local space
	CVector3f		m_TurnMoment;
	CVector3f		m_ExternalMoment;
	float				m_MomentScaleFactor;
	float				m_TerrainOffset;
	float				m_LinearDamping2;
	float				m_AngularDamping2;
	float				m_MaxTurnSpeed;
	
	float				m_FlipTime;

	int				m_HardPointCount;	// Count of collision hard points
	CVector3f		m_HardPoints[8];		// Points to check collisions, in local space
	CVector3f		m_HardPointsWorld[8]; // Hard points in world space
	int				m_ContactCount;
	int				m_CollisionCount;
	CollisionData	m_Collisions[8];		// Used for terrain contact and collision

	void CalcLoads(void);
	void HandleTerrainContact(void);
	void SetupHardPoints(float Width, float Height, float Length);
	void SetupThrusters(float Width, float Height, float Length);
	float CalcDistFromTerrain(const CVector3f *pt);
	void RotateHardPoints(void);
	void RotateInertia(void);
	void ApplyDampingForce(CVector3f *force, CVector3f *vel);
	void ApplyDampingMoment(CVector3f *tau, CVector3f *omega);
	void CheckFlip(float dt);
	void CheckMapBoundary(void);
public:
	CRigidBody(void);
	~CRigidBody(void);

	void Initialize(CGameObject *owner, float Mass, float Width, float Height, float Length);
	void StepIntegrate(float dt, bool checkflip = true);
	void HandleTerrainCollision(void);
	void DetectTerrainCollision(void);
	void ApplyImpulse(float J, const CollisionData *collision);
	void StopTurning(void);
	void ApplyFriction(void);
	void ApplyExternalMoment(const CVector3f *dir, float mag);
	void ApplyExternalMomentum(const CVector3f *v);

	void MoveForward(void);
	void MoveBackward(void);
	void TurnRight(float angle);
	void MoveLeft(void);
	void MoveRight(void);
	void MoveUp(void);

	void SetThruster(int thruster, float magnitude);
	void SetTurnMomentScale(float scale);
	void SetTerrainOffset(float offset)
	{
		m_TerrainOffset = offset;
	}

	void SetLinearDamping2(float damp)
	{
		m_LinearDamping2 = damp;
	}
	void SetAngularDamping2(float damp)
	{
		m_AngularDamping2 = damp;
	}

	void SetParameters(float maxforwardspd, float accelerationtime);
	void SetMaxTurnSpeed(float omega)
	{
		m_MaxTurnSpeed = omega;
	}

	void GetVelocity(CVector3f *v);
	float GetTerrainOffset(void) const
	{
		return m_TerrainOffset;
	}
	friend float CalcImpulse(const CollisionData *collision);
};

float CalcImpulse(const CollisionData *collision);

