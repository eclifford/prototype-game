/// \file energypowerup.h
/// The energy powerup
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/powerup.h"

class CEnergyPowerUp : public CPowerUp
{
	private:
      int m_AmountRestored;
      		
	public:
      CEnergyPowerUp(CEnergyPowerUp& ShieldPowerUp) : CPowerUp(ShieldPowerUp)
      {

      }

		CEnergyPowerUp();
		~CEnergyPowerUp();

      void Setup(int energyRestored);
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);

      inline int GetAmountRestored(void)
      {   return m_AmountRestored;  }
};