/// \file Building2.h
/// A stationary Building2
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/stationaryobjects.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CBuilding2: public CStationaryObject
{
	private:
		
	public:
      CBuilding2(CBuilding2& Building2) : CStationaryObject(Building2)
      {
         m_Type = OBJ_BUILDING2;
         m_RadarType = RADAR_BUILDING;
         m_CurBuilding2State = STANDING;
	      m_StaticObj = true;

         m_pBuildingFallingSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_BUILDINGCOLLAPSE1], 8);   
      }

		CBuilding2();
		~CBuilding2();

      bool OnlyOnce;
      bool moveright;

      enum Building2States{STANDING = 0, FALLING, EXPLODE};
      int m_CurBuilding2State;

      CParticleSystem * m_Building2Falling;
      CParticleSystem * m_Building2Smoke;
      CParticleSystem *m_Building2Explode;

      CAudio * m_pBuildingFallingSFX;

      float m_TimeStartedCollapse;
      float m_TimeEndCollapse;
      float m_TimeCurCollapse;
      float m_FallingSpeed;

      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
      void TakeDamage(float damage, CGameObject * guywhohitme);
      void Setup(void);
      void Collapse(float timeinseconds, float fallingspeed);
};