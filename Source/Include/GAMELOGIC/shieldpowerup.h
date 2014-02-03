/// \file shieldpowerup.h
/// The shield powerup
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/powerup.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CShieldPowerUp : public CPowerUp
{
	private:
      int m_AmountRestored;
      		
	public:
      CShieldPowerUp(CShieldPowerUp& ShieldPowerUp) : CPowerUp(ShieldPowerUp)
      {
         m_Type = OBJ_SHDPWRUP;
         m_pShieldPickup = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_SHLDPWRUP], 10);
      }

		CShieldPowerUp();
		~CShieldPowerUp();

      CAudio * m_pShieldPickup;

      void Setup();
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);

      inline int GetAmountRestored(void)
      {   return m_AmountRestored;  }
};