/// \file ioncannon.h
/// The blast of the ion cannon
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/projectile.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CParticleManager g_ParticleManager;

class CIonCannon : public CProjectile
{
	private:
			
	public:
      CIonCannon(CIonCannon& IonCannon) : CProjectile(IonCannon)
      {
         m_Type = OBJ_IONCANNON;
         m_ParticleEffect = NULL;
         m_ExplosionEffect = NULL;
      }

      CParticleSystem *m_ParticleEffect;
      CParticleSystem *m_ExplosionEffect;

      CAudio * m_IonBlast;
      CAudio * m_IonWave;

      float m_Velocity;
      float m_Gravity;

		CIonCannon();
		~CIonCannon();

      void SetUp(int type, int damage, CGameObject * projectileowner);
      void Update();
		void Collide(const CGameObject *obj);
		void Die(void);
      void Reset(void);
};