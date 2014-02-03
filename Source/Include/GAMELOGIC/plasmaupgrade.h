/// \file plasmaupgrade.h
/// The upgrade to the plasma shot
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/powerup.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CPlasmaUpgrade : public CPowerUp
{
	private:
      		
	public:
      CPlasmaUpgrade(CPlasmaUpgrade& PlasmaUpgrade) : CPowerUp(PlasmaUpgrade)
      {
         m_Type = OBJ_PLASMAPWRUP;
         m_pUpgrade = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_WPNUPGRADE], 10);
      }

		CPlasmaUpgrade();
		~CPlasmaUpgrade();

      CAudio * m_pUpgrade;

      void Setup(void);
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
};