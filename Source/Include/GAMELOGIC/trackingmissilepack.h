/// \file trackingmissilepack.h
/// A pack of tracking missiles
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/powerup.h"
#include "sound/audio.h"

class CTrackingMissilePack : public CPowerUp
{
	private:
      int m_NumMissiles;
      float m_FallingVelocity;
      		
	public:
      CTrackingMissilePack(CTrackingMissilePack& TrackingMissilePack) : CPowerUp(TrackingMissilePack)
      {
         m_Type = OBJ_TRKMSLPACK;
      }

		CTrackingMissilePack();
		~CTrackingMissilePack();

      CAudio * m_pPackPickup;

      void Setup();
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);

      inline int GetNumMissiles(void)
      {   return m_NumMissiles;  }
};