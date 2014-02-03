/// \file stationaryobjects.h
/// The base class for stationary objects
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/gameobject.h"

class CStationaryObject : public CGameObject
{
	private:
		
	public:
      CStationaryObject(CStationaryObject& StationaryObject) : CGameObject(StationaryObject)
      {

      }

		CStationaryObject();
		~CStationaryObject();

      int m_HitPoints;
      int m_MaxHitPoints;

      void FollowTerrain(void);
      virtual void Update(void) = 0;
		virtual void Collide(const CGameObject *obj) = 0;
		virtual void Die(void) = 0;
		virtual void Reset(void) = 0;
      virtual void TakeDamage(float damage, CGameObject * guywhohitme) { }
};