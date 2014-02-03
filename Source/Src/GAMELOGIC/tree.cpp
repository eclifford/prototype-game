/// \file tree.cpp
/// A stationary tree
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/tree.h"
#include "graphics/particlemanager.h"

#include "utils\mmgr.h"

extern CParticleManager g_ParticleManager;
extern float g_FrameTime;

CTree::CTree()
{
   
}

CTree::~CTree()
{

}

void CTree::Setup(void)
{
   m_CurTreeState = TREE_FINE;
   m_HitPoints = 40.0f;
   m_FireDamagePerSec = 5.0f;
   m_Active = true;
	m_StaticObj = true;
}

void CTree::Update(void)
{
   if(m_CurTreeState == TREE_FIRE)
   {
      m_HitPoints -= m_FireDamagePerSec * g_FrameTime;
      if(m_HitPoints <= 0.0f)
      {
         m_CurTreeState = TREE_DIE;

         m_TreeExplode = g_ParticleManager.ObtainSystem();
         if(m_TreeExplode)
            g_ParticleManager.CreateSystem(m_TreeExplode, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.TREE_DIE, LOD_FAR, 5.0f, 200.0f);
      }
   }

   else if(m_CurTreeState == TREE_DIE)
   {
      m_RenderObj.m_Material.m_Diffuse.a -= 1.5f * g_FrameTime;
      m_RenderObj.m_Material.m_Ambient.a -= 1.5f * g_FrameTime;

      if(m_RenderObj.m_Material.m_Diffuse.a <= 0.0f && m_RenderObj.m_Material.m_Ambient.a <= 0.0f)
      {
         Die();
      }

		// Rotate
		CVector3f pos, npos;
		CVector3f offset(0.0f, m_OBB.y, 0.0f);

		m_RenderObj.GetFrame(0)->RotateToWorldVector(&offset, &offset);
		m_RenderObj.GetFrame(0)->GetPosition(&pos);
		Vec3fSubtract(&pos, &pos, &offset);
		Vec3fNegate(&npos, &pos);

		m_RenderObj.GetFrame(0)->TranslateWorld(&npos);
		m_RenderObj.GetFrame(0)->RotateWorldAxis(0.05f, &m_RotationAxis);
		m_RenderObj.GetFrame(0)->TranslateWorld(&pos);
   }

   CGameObject::Update();
}

void CTree::FindRotationAxis(const CGameObject *obj)
{
	CVector3f pos1, pos2, vec, Y(0.0f, 1.0f, 0.0f);

	m_RenderObj.GetFrame(0)->GetPosition(&pos1);
	CGameObject *p = const_cast<CGameObject *>(obj);
	p->GetPosition(&pos2, 0);
	Vec3fSubtract(&vec, &pos1, &pos2);
	Vec3fCrossProduct(&m_RotationAxis, &Y, &vec);
	Vec3fNormalize(&m_RotationAxis, &m_RotationAxis);
}

void CTree::Collide(const CGameObject *obj)
{
   // Object is the terrain 
   if(!obj)
   {
      // DO NOTHING
   }
   
   // Object is a game object
   else
   {
      if(obj->m_RadarType == RADAR_ENEMY || obj->m_RadarType == RADAR_PLAYER || obj->m_RadarType == RADAR_FRIENDLY && obj->m_RadarType != OBJ_SPHERE && m_CurTreeState != TREE_DIE)
      {
			FindRotationAxis(obj);
         m_CurTreeState = TREE_DIE;
         m_pTreeFall->Play3DAudio(10.0f, 10000.0f, 100);
         CVector3f position;
	      GetPosition(&position, 0);
         CVector3f velocity;
         velocity.x = 0.0f;
         velocity.y = 0.0f;
         velocity.z = 0.0f;
         m_pTreeFall->UpdateSound(position, velocity);
      }
   } 
}

void CTree::Die(void)
{
   m_Active = false;
   RemoveFromQuadtree();

   if(treefire)
      treefire->SetSystemAlive(false);

   m_pTreeOnFire->StopAudio();
}

void CTree::Reset(void)
{
   
}

void CTree::TakeDamage(float damage, CGameObject * guywhohitme)
{
   m_HitPoints -= damage;

   if(m_CurTreeState == TREE_FINE)
   {
      // Get a particle effect for the shot if one available
      treefire = g_ParticleManager.ObtainSystem();

      m_pTreeOnFire->Play3DAudio(20.0f, 10000.0f, 150);
      CVector3f position;
	   GetPosition(&position, 0);
      CVector3f velocity;
      velocity.x = 0.0f;
      velocity.y = 0.0f;
      velocity.z = 0.0f;
      m_pTreeOnFire->UpdateSound(position, velocity);

      // If obtained a particle effect then we use it
      if(treefire != NULL)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(treefire, *(GetFrame(0)->GetPosition()),
         GetFrame(0)->GetPosition(), g_ParticleManager.TREEFIRE, LOD_NONE, 5.0f, 200.0f);
      }  
      m_CurTreeState = TREE_FIRE;
   }

   if(m_HitPoints <= 0)
      Die();
}

