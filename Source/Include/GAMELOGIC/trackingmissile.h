/// \file trackingmissile.h
/// A Tracking Missile projectile
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/projectile.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CParticleManager g_ParticleManager;

class CTrackingMissile : public CProjectile
{
	private:
		CGameObject		*m_Target;
		CQuaternion		m_Q0, m_Q1, m_Q2;
		bool				m_Tracking;
		float				m_Age;
		float				m_LastUpdate;
		float				m_UpdatePeriod;
		float				m_SlerpScale;
		bool				m_FlyLeft;
			
	public:
      CTrackingMissile(CTrackingMissile& TrackingMissile) : CProjectile(TrackingMissile)
      {
         m_Type = OBJ_TRKMISSILE;   
         m_FireTrail = NULL;
         m_SmokeTrail = NULL;
         m_ExplosionEffect = NULL;
			m_FlyLeft = true;
			m_Target = 0;
      }

      CParticleSystem *m_FireTrail;
      CParticleSystem *m_SmokeTrail;
      CParticleSystem *m_ExplosionEffect;

      CAudio * m_MissileExplode;
      CAudio * m_MissileInAir;

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

		CTrackingMissile();
		~CTrackingMissile();

      void SetUp(int shotOwner, int damage, bool t, float damageradius, float accuracy, CGameObject * projectileowner);
		void SetFlyLeft(bool left)
		{
			m_FlyLeft = left;
		}
		void SetTarget(CGameObject *obj)
		{
			m_Target = obj;
		}
      void Update();
		void Collide(const CGameObject *obj);
		void Die(void);
      void Reset(void);
};