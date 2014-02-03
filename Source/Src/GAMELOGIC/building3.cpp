/// \file Building3.cpp
/// A stationary Building3
//////////////////////////////////////////////////////////////////////////
#include "gamelogic/Building3.h"
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

CBuilding3::CBuilding3()
{

}

//////////////////////////////////////////////////////////////////////////
/// Destructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CBuilding3::~CBuilding3()
{

}

//////////////////////////////////////////////////////////////////////////
/// Setup a Building3
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding3::Setup(void)
{
   m_Active = true;
   m_HitPoints = 3500;
	m_StaticObj = true;
   moveright = false;
}

//////////////////////////////////////////////////////////////////////////
/// Update
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding3::Update(void)
{
   if(m_CurBuilding3State == FALLING)
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
         if(m_Building3Falling)
            m_Building3Falling->m_UpdateFlag = ONLY_UPDATE;  

         m_CurBuilding3State = EXPLODE;

         const Sphere* pObjectSphere = GetSphere();
         float Building3Radius = pObjectSphere->Radius; 
   
         m_Building3Explode = g_ParticleManager.ObtainSystem();

         CObjFrame TempFrame;
         TempFrame = *GetFrame(0);
         TempFrame.MoveUp(Building3Radius * 0.5f);
         CVector3f Building3Top = *TempFrame.GetPosition();

         if(m_Building3Explode)
            g_ParticleManager.CreateSystem(m_Building3Explode, Building3Top, NULL, g_ParticleManager.BUILDING_EXPLODE, LOD_BOTH);
      }
   }

   else if(m_CurBuilding3State == EXPLODE)
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

void CBuilding3::Collide(const CGameObject *obj)
{

}

//////////////////////////////////////////////////////////////////////////
/// Die
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding3::Die(void)
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

void CBuilding3::Reset(void)
{
   
}

//////////////////////////////////////////////////////////////////////////
/// Take damage
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding3::TakeDamage(float damage, CGameObject * guywhohitme)
{
   m_HitPoints -= (int)damage;
   if(m_HitPoints <= 0 && m_CurBuilding3State == STANDING)
      Collapse(3.0f, -20.0f);
}

//////////////////////////////////////////////////////////////////////////
/// Collapse
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBuilding3::Collapse(float timeinseconds, float fallingspeed)
{
   m_TimeStartedCollapse = g_time;

   const Sphere* pObjectSphere = GetSphere();
   float Building3Radius = pObjectSphere->Radius; 

   m_TimeEndCollapse = m_TimeStartedCollapse + abs((Building3Radius / fallingspeed)* 1.3f);

   m_FallingSpeed = fallingspeed;

   m_CurBuilding3State = FALLING;

   m_Building3Falling = g_ParticleManager.ObtainSystem();

   if(OnlyOnce)
   {
      if(GameMain.m_CurrentBuildingCom == GameMain.COM_BUILDINGNONE)
         GameMain.m_CurrentBuildingCom = GameMain.COM_BUILDINGDEAD;
      OnlyOnce = false;
   }

   CObjFrame TempFrame;
   TempFrame = *GetFrame(0);
   TempFrame.MoveUp(-Building3Radius * 0.6f);
   CVector3f Building3Base = *TempFrame.GetPosition();

   if(m_Building3Falling)
   {
      m_Building3Falling->m_IsBuilding = true;
      g_ParticleManager.CreateSystem(m_Building3Falling, Building3Base, NULL, g_ParticleManager.BUILDING_FALLING, LOD_BOTH, 200.0f, 500.0f);
   }
   else
    bool debug = true;
}

