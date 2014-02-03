/// \file plasmaupgrade.h
/// The upgrade to the plasma shot
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/powerup.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CMissileUpgrade : public CPowerUp
{
	private:
      		
	public:
      CMissileUpgrade(CMissileUpgrade& MissileUpgrade) : CPowerUp(MissileUpgrade)
      {
         m_Type = OBJ_MSLPWRUP;
         m_pUpgrade = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_WPNUPGRADE], 10);
      }

		CMissileUpgrade();
		~CMissileUpgrade();

      CAudio * m_pUpgrade;

      void Setup(void);
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
};
