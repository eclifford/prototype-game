/// \file IonFlare.h
/// The ion flare 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/projectile.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CParticleManager g_ParticleManager;

class CIonFlare : public CProjectile
{
	private:
			
	public:
      CIonFlare(CIonFlare& IonFlare) : CProjectile(IonFlare)
      {
         m_Type = OBJ_IONFLARE;  
         m_ParticleEffect = NULL;
         m_ExplosionEffect = NULL;
      }

      // Sound FX
      CAudio * m_PlasmaExplode;

      CParticleSystem *m_ParticleEffect;
      CParticleSystem *m_ExplosionEffect;

		CIonFlare();
		~CIonFlare();

      void SetUp(int shotowner, CGameObject * projectileowner, float damage, float damageradius = 10.0f, float velocity = 200.0f);
      void Update();
		void Collide(const CGameObject *obj);
		void Die(void);
      void Reset(void);
};