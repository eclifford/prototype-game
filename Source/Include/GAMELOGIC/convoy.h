/// \file convoy.h
/// The convoy unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/friendly.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

struct Waypoint;

extern CAudioManager g_AudioManager;

class CConvoy: public CFriendly
{
	private:
		
	public:
      CConvoy(CConvoy& Convoy);

		CConvoy();
		~CConvoy();

      enum ConvoyState{CONVOY_FINE = 0, CONVOY_SMOKE, CONVOY_FIRE};
      enum ConvoyMoveState{CONVOY_MOVING, CONVOY_STOPPED};
      int m_CurConvoyState;
      int m_CurConvoyMoveState;

      float m_TurnSpeed;
      CVector3f Position;
      CObjFrame ConvoyTopMid;

      CParticleSystem * m_pSmoke;
      CParticleSystem * m_pFire;
      CParticleSystem * m_pExplode;

      CAudio * m_pExplosionSFX;
      CAudio * m_pEngineSFX;

      // Used for sound 
      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

      // Waypoints
      Waypoint* m_Waypoints;
      int m_CurrentWaypointIndex;
      int m_LastWaypoint;

      float m_AimTimer;
      float m_AimWait;
      bool tempbool1;
      bool tempbool2;
      bool tempbool3;

      void Setup();
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
      void TakeDamage(float damage, CGameObject * guywhohitme);
      void SetWaypoints(Waypoint* WaypointArray, int Indices);
      int  TargetRelativePosition();
      void AimTowardTarget();
      bool IsAtCurrentWaypoint();
      void NextWaypoint();
      void MoveTowardWaypoint();

};
