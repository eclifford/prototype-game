/// \file tree3.h
/// A stationary tree3
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/stationaryobjects.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CTree3: public CStationaryObject
{
	private:
		CVector3f m_RotationAxis;
		
		void FindRotationAxis(const CGameObject *obj);
		
	public:
      CTree3(CTree3& tree3) : CStationaryObject(tree3)
      {
         m_Type = OBJ_TREE3;
         treefire = NULL;
			Vec3fZero(&m_RotationAxis);
         m_RenderObj.m_Material.m_Diffuse.a = 1.0f;
         m_RenderObj.m_Material.m_Ambient.a = 1.0f;
			m_StaticObj = true;
         m_pTreeFall = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_TREEFALL], 3);
         m_pTreeOnFire = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_FIRE], 2);
      }

		CTree3();
		~CTree3();

      float m_HitPoints;
      float m_FireDamagePerSec;

      CAudio * m_pTreeFall;
      CAudio * m_pTreeOnFire;

      enum TreeState{TREE_FINE = 0, TREE_FIRE, TREE_DIE, TREE_FALLOVER};
      int m_CurTreeState;

      CParticleSystem *treefire;
      CParticleSystem *m_TreeExplode;

      void Setup(void);
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
      void TakeDamage(float damage, CGameObject * guywhohitme);
};