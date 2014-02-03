/// \file friendly.h
/// The base class for all friendly NPC units
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/gameobject.h"

class CFriendly : public CGameObject
{
	private:
		
	public:
      CFriendly(CFriendly& Friendly) : CGameObject(Friendly)
      {
         m_RadarType = RADAR_FRIENDLY;
      }

		CFriendly();
		~CFriendly();

      float m_HitPoints;
      float m_MaxHitPoints;

      virtual void Update(void) = 0;
		virtual void Collide(const CGameObject *obj) = 0;
		virtual void Die(void) = 0;
		virtual void Reset(void) = 0;
      virtual void TakeDamage(float damage, CGameObject * guywhohitme) { }
};