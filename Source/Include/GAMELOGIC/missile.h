/// \file missile.h
/// A Missile projectile
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/projectile.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CParticleManager g_ParticleManager;

class CMissile : public CProjectile
{
	private:
		CQuaternion		m_Q0, m_Q1, m_Q2;
		float				m_Age;
		float				m_Delay;
			
	public:
      CMissile(CMissile& Missile) : CProjectile(Missile)
      {
         m_Type = OBJ_MISSILE;    
         m_FireTrail = NULL;
         m_SmokeTrail = NULL;
         m_ExplosionEffect = NULL;
      }

      CParticleSystem *m_FireTrail;
      CParticleSystem *m_SmokeTrail;
      CParticleSystem *m_ExplosionEffect;

      CAudio * m_MissileExplode;
      CAudio * m_MissileInAir;

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

		CMissile();
		~CMissile();

      void SetUp(int shotOwner, int damage, int myindex, float damageradius, CGameObject * projectileowner);
      void Update();
		void Collide(const CGameObject *obj);
		void Die(void);
      void Reset(void);
};