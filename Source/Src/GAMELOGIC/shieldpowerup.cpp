/// \file shieldpowerup.cpp
/// The shield powerup
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/shieldpowerup.h"
#include "core/gamemain.h"
#include "gamelogic/player.h"

#include "utils\mmgr.h"

extern float g_FrameTime;
extern float g_time;
extern CPlayer g_Player;
extern CAudioManager g_AudioManager;

CShieldPowerUp::CShieldPowerUp()
{

}

CShieldPowerUp::~CShieldPowerUp()
{

}

void CShieldPowerUp::Setup()
{
   m_Active = true;
   m_AmountRestored = 200;
   m_CurAge = g_time;
   m_DeathAge = 0.0f;
   m_RenderObj.m_Material.m_Diffuse.a = 1.0f;
   m_RenderObj.m_Material.m_Ambient.a = 1.0f;
}

void CShieldPowerUp::Update()
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

void CShieldPowerUp::Collide(const CGameObject *obj)
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
            m_pShieldPickup->Play3DAudio(10.0f, 10000.0f, 120);
            CVector3f position;
	         GetPosition(&position, 0);
            CVector3f velocity;
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            velocity.z = 0.0f;
            m_pShieldPickup->UpdateSound(position, velocity);
            Die();         
         }
         break;
      } 
   } 
}

void CShieldPowerUp::Die(void)
{
   m_Active = false;
   RemoveFromQuadtree();
   g_Player.m_NumPowerUpsCollected++;
}

void CShieldPowerUp::Reset(void)
{
   
}