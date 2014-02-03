/// \file building.cpp
/// A stationary building
//////////////////////////////////////////////////////////////////////////
#include "gamelogic/building.h"
#include "graphics/particlemanager.h"
#include "core/gamemain.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CParticleManager g_ParticleManager;
extern CGameMain GameMain;

/////////////////////////////////
/*/ Global Instances          /*/ 
/////////////////////////////////

extern float g_time;
extern float g_FrameTime;

//////////////////////////////////////////////////////////////////////////
/// Constructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CBuilding::CBuilding()
{

}

//////////////////////////////////////////////////////////////////////////
/// Destructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CBuilding::~CBuilding()
{

}

//////////////////////////////////////////////////////////////////////////
/// Setup a building
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding::Setup(void)
{
   m_Active = true;
   m_HitPoints = 1500;
	m_StaticObj = true;
   OnlyOnce = true;
   moveright = false;
}

//////////////////////////////////////////////////////////////////////////
/// Update
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding::Update(void)
{
   if(m_CurBuildingState == FALLING)
   {
      MoveUp(m_FallingSpeed * g_FrameTime, 0);
      
      if(moveright)
      {
         MoveRight(10.0f * g_FrameTime, 0);
         moveright = false;
      }
      else
      {
         MoveRight(-(10.0f * g_FrameTime), 0);
         moveright = true;
      }

      if((m_TimeEndCollapse - g_time) <= 2.0f)
      {
         if(!(FSOUND_IsPlaying(m_pBuildingFallingSFX->m_Channel)))
         {
            // Make main weapon sound
            m_pBuildingFallingSFX->Play3DAudio(60, 10000.0f, 300);
            CVector3f position;
	         GetPosition(&position, 0);
            CVector3f velocity(0.0f, 0.0f, 0.0f);
            m_pBuildingFallingSFX->UpdateSound(position, velocity);
         }
      }

      if((m_TimeEndCollapse - g_time) <= 1.0f)
      {     
         if(m_BuildingFalling)
            m_BuildingFalling->m_UpdateFlag = ONLY_UPDATE;  

         m_CurBuildingState = EXPLODE;

         const Sphere* pObjectSphere = GetSphere();
         float BuildingRadius = pObjectSphere->Radius; 
   
         m_BuildingExplode = g_ParticleManager.ObtainSystem();

         CObjFrame TempFrame;
         TempFrame = *GetFrame(0);
         TempFrame.MoveUp(BuildingRadius * 0.5f);
         CVector3f BuildingTop = *TempFrame.GetPosition();

         if(m_BuildingExplode)
            g_ParticleManager.CreateSystem(m_BuildingExplode, BuildingTop, NULL, g_ParticleManager.BUILDING_EXPLODE, LOD_BOTH);
      }
   }

   else if(m_CurBuildingState == EXPLODE)
   {
      static bool moveright = false;

      MoveUp(m_FallingSpeed * g_FrameTime, 0);

      if(moveright)
      {
         MoveRight(10.0f * g_FrameTime, 0);
         moveright = false;
      }
      else
      {
         MoveRight(-(10.0f * g_FrameTime), 0);
         moveright = true;
      }

      m_RenderObj.m_Material.m_Diffuse.a -= 1.0f * g_FrameTime;
      m_RenderObj.m_Material.m_Ambient.a -= 1.0f * g_FrameTime;

      if(g_time >= m_TimeEndCollapse)
         Die();
   }

   CGameObject::Update();
}

//////////////////////////////////////////////////////////////////////////
/// On Collide
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding::Collide(const CGameObject *obj)
{

}

//////////////////////////////////////////////////////////////////////////
/// Die
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding::Die(void)
{
   m_Active = false;
   RemoveFromQuadtree();
   GameMain.m_BuildingsDead++;
}

//////////////////////////////////////////////////////////////////////////
/// Reset
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding::Reset(void)
{
   
}

//////////////////////////////////////////////////////////////////////////
/// Take damage
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding::TakeDamage(float damage, CGameObject * guywhohitme)
{
   m_HitPoints -= (int)damage;
   if(m_HitPoints <= 0 && m_CurBuildingState == STANDING)
      Collapse(3.0f, -20.0f);
}

//////////////////////////////////////////////////////////////////////////
/// Collapse
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding::Collapse(float timeinseconds, float fallingspeed)
{
   m_TimeStartedCollapse = g_time;

   const Sphere* pObjectSphere = GetSphere();
   float BuildingRadius = pObjectSphere->Radius; 

   m_TimeEndCollapse = m_TimeStartedCollapse + abs((BuildingRadius / fallingspeed)* 1.3f);

   m_FallingSpeed = fallingspeed;

   m_CurBuildingState = FALLING;

   m_BuildingFalling = g_ParticleManager.ObtainSystem();

   CObjFrame TempFrame;
   TempFrame = *GetFrame(0);
   TempFrame.MoveUp(-BuildingRadius * 0.6f);
   CVector3f BuildingBase = *TempFrame.GetPosition();

   if(OnlyOnce)
   {
      if(GameMain.m_CurrentBuildingCom == GameMain.COM_BUILDINGNONE)
         GameMain.m_CurrentBuildingCom = GameMain.COM_BUILDINGDEAD;
      OnlyOnce = false;
   }

   if(m_BuildingFalling)
   {
      m_BuildingFalling->m_IsBuilding = true;
      g_ParticleManager.CreateSystem(m_BuildingFalling, BuildingBase, NULL, g_ParticleManager.BUILDING_FALLING, LOD_BOTH, 200.0f, 500.0f);
   }
   else
    bool debug = true;
  
}

