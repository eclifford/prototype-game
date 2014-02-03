/// \file plasmaupgrade.cpp
/// The plasma upgrade
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/plasmaupgrade.h"
#include "core/gamemain.h"
#include "gamelogic/player.h"

#include "utils\mmgr.h"

extern float g_FrameTime;
extern float g_time;
extern CPlayer g_Player;
extern CAudioManager g_AudioManager;

CPlasmaUpgrade::CPlasmaUpgrade()
{

}

CPlasmaUpgrade::~CPlasmaUpgrade()
{

}

void CPlasmaUpgrade::Setup(void)
{
   m_Active = true;
   m_CurAge = g_time;
   m_DeathAge = 0.0f;
}

void CPlasmaUpgrade::Update()
{
   m_RenderObj.RotateLocalY(5.0f * g_FrameTime, 0);

   m_CurAge += g_FrameTime;

   if(m_CurAge >= m_DeathAge && m_DeathAge != 0.0f)
   {
      m_RenderObj.m_Material.m_Diffuse.a -= 1.5f * g_FrameTime;
      m_RenderObj.m_Material.m_Ambient.a -= 1.5f * g_FrameTime;
   }

   if(m_RenderObj.m_Material.m_Diffuse.a <= 0.0f && m_RenderObj.m_Material.m_Ambient.a <= 0.0f)
   {
      Die();
   }

   CGameObject::Update();
}

void CPlasmaUpgrade::Collide(const CGameObject *obj)
{
   // Object is the terrain 
   if(!obj)
   {
      // DO NOTHING
   }
   
   // Object is a game object
   else
   {
      switch(obj->GetType())
      {
         case OBJ_PLAYER:
         {
            // Kill the powerup
            m_pUpgrade->Play3DAudio(10.0f, 10000.0f, 120);
            CVector3f position;
	         GetPosition(&position, 0);
            CVector3f velocity;
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            velocity.z = 0.0f;
            m_pUpgrade->UpdateSound(position, velocity);
            Die();         
         }
         break;
      } 
   } 
}

void CPlasmaUpgrade::Die(void)
{
   m_Active = false;
   RemoveFromQuadtree();
   g_Player.m_NumPowerUpsCollected++;
}

void CPlasmaUpgrade::Reset(void)
{
   
}