/// \file building.h
/// A stationary building
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/stationaryobjects.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CBuilding: public CStationaryObject
{
	private:
		
	public:
      CBuilding(CBuilding& Building) : CStationaryObject(Building)
      {
         m_Type = OBJ_BUILDING;
         m_RadarType = RADAR_BUILDING;
         m_CurBuildingState = STANDING;
			m_StaticObj = true;

         m_pBuildingFallingSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_BUILDINGCOLLAPSE1], 8);     
      }

		CBuilding();
		~CBuilding();

      enum BuildingStates{STANDING = 0, FALLING, EXPLODE};
      int m_CurBuildingState;

      CParticleSystem * m_BuildingFalling;
      CParticleSystem * m_BuildingSmoke;
      CParticleSystem *m_BuildingExplode;

      CAudio * m_pBuildingFallingSFX;
      CAudio * m_pBuildingFallingLoopSFX;

      float m_TimeStartedCollapse;
      float m_TimeEndCollapse;
      float m_TimeCurCollapse;
      float m_FallingSpeed;

      bool OnlyOnce;
      bool moveright;

      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
      void TakeDamage(float damage, CGameObject * guywhohitme);
      void Setup(void);
      void Collapse(float timeinseconds, float fallingspeed);
};