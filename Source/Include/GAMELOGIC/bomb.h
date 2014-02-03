/// \file bomb.h
/// A Bomb projectile
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/projectile.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CParticleManager g_ParticleManager;

class CBomb : public CProjectile
{
	private:
		float				m_Age;
		float				m_Delay;
      float m_Velocity;
			
	public:
      CBomb(CBomb& Bomb) : CProjectile(Bomb)
      {
         m_Type = OBJ_BOMB;    
         m_ExplosionEffect = NULL;
         m_BombDrop = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_DROPBOMB], 7);
         m_BombExplode = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_MED], 7);
      }

      enum BombStates{DROPPED = 0, FALLING};
      int m_CurBombState;
      float m_AmountRotatedThisFrame;
      float m_AmountRotatedTotal;

      CParticleSystem *m_ExplosionEffect;

      CAudio * m_BombExplode;
      CAudio * m_BombDrop;

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

		CBomb();
		~CBomb();

      void SetUp(int shotOwner, int damage, float damageradius, CGameObject * projectileowner);
      void Update();
		void Collide(const CGameObject *obj);
		void Die(void);
      void Reset(void);
};






