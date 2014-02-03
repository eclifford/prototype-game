/// \file tree.h
/// A stationary tree
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/stationaryobjects.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CTree: public CStationaryObject
{
	private:
		CVector3f m_RotationAxis;
		
		void FindRotationAxis(const CGameObject *obj);
	public:
      CTree(CTree& Tree) : CStationaryObject(Tree)
      {
         m_Type = OBJ_TREE;
         treefire = NULL;
			Vec3fZero(&m_RotationAxis);
         m_RenderObj.m_Material.m_Diffuse.a = 1.0f;
         m_RenderObj.m_Material.m_Ambient.a = 1.0f;
			m_StaticObj = true;

         m_pTreeFall = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_TREEFALL], 3);
         m_pTreeOnFire = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_FIRE], 2);
      }

		CTree();
		~CTree();
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