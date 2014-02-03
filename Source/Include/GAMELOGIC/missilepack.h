/// \file missilepack.h
/// Missile pack powerup
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/powerup.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CMissilePack : public CPowerUp
{
	private:
      int m_NumMissiles;
      float m_FallingVelocity;
      		
	public:
      CMissilePack(CMissilePack& MissilePack) : CPowerUp(MissilePack)
      {
         m_Type = OBJ_MSLPACK;
         m_pPackPickup = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_AMMOPWRUP], 10);
      }

		CMissilePack();
		~CMissilePack();

      CAudio * m_pPackPickup;

      void Setup();
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);

      inline int GetNumMissiles(void)
      {   return m_NumMissiles;  }
};