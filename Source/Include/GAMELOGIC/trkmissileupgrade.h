/// \file plasmaupgrade.h
/// The upgrade to the plasma shot
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/powerup.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CTrkMissileUpgrade : public CPowerUp
{
	private:
      		
	public:
      CTrkMissileUpgrade(CTrkMissileUpgrade& TrkMissileUpgrade) : CPowerUp(TrkMissileUpgrade)
      {
         m_Type = OBJ_TRKMSLPWRUP;
         m_pUpgrade = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_WPNUPGRADE], 10);
      }

		CTrkMissileUpgrade();
		~CTrkMissileUpgrade();

      CAudio * m_pUpgrade;

      void Setup(void);
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
};