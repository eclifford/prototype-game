/// \file Building3.h
/// A stationary Building3
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/stationaryobjects.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CBuilding3: public CStationaryObject
{
	private:
		
	public:
      CBuilding3(CBuilding3& Building3) : CStationaryObject(Building3)
      {
         m_Type = OBJ_BUILDING3;
         m_RadarType = RADAR_BUILDING;
         m_CurBuilding3State = STANDING;
			m_StaticObj = true;

         m_pBuildingFallingSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_BUILDINGCOLLAPSE1], 8);   
      }

		CBuilding3();
		~CBuilding3();

      bool OnlyOnce;
      bool moveright;

      enum Building3States{STANDING = 0, FALLING, EXPLODE};
      int m_CurBuilding3State;

      CParticleSystem * m_Building3Falling;
      CParticleSystem * m_Building3Smoke;
      CParticleSystem *m_Building3Explode;

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