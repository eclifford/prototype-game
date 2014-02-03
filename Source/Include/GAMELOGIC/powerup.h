/// \file powerup.h
/// The base class for all powerups
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/gameobject.h"

class CPowerUp : public CGameObject
{
	private:
		
	public:
      CPowerUp(CPowerUp& PowerUp) : CGameObject(PowerUp)
      {

      }

		CPowerUp();
		~CPowerUp();

      float m_DeathAge;
      float m_CurAge;

      virtual void Update(void) = 0;
		virtual void Collide(const CGameObject *obj) = 0;
		virtual void Die(void) = 0;
		virtual void Reset(void) = 0;
      void FollowTerrain(void);
};