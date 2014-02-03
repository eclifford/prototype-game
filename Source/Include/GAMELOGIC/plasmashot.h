/// \file plasmashot.h
/// The plasma shot 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/projectile.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CParticleManager g_ParticleManager;

class CPlasmaShot : public CProjectile
{
	private:
			
	public:
      CPlasmaShot(CPlasmaShot& PlasmaShot) : CProjectile(PlasmaShot)
      {
         m_Type = OBJ_PLASMA;  
         m_ParticleEffect = NULL;
         m_ExplosionEffect = NULL;
      }

      // Sound FX
      CAudio * m_PlasmaExplode;
      CAudio * m_PlasmaInAir;

      CParticleSystem *m_ParticleEffect;
      CParticleSystem *m_ExplosionEffect;

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

		CPlasmaShot();
		~CPlasmaShot();

      void SetUp(int shotowner, CGameObject * projectileowner, float damage, float damageradius = 10.0f, float velocity = 200.0f);
      void Update();
		void Collide(const CGameObject *obj);
		void Die(void);
      void Reset(void);
};