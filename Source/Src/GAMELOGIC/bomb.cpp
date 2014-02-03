/// \file bomb.cpp
/// The bomb projectile
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\bomb.h"
#include "gamelogic\objectfactory.h"
#include "graphics\particlemanager.h"
#include "core\gamemain.h"
#include "graphics\quadtree.h"
#include "graphics\terrain.h"
#include "gamelogic\player.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CObjectFactory   g_ObjectFactory;
extern CParticleManager g_ParticleManager;
extern CGameMain        GameMain;
extern CQuadtree        g_Quadtree;
extern CTerrain*        g_pCurrLevel;
extern CAudioManager    g_AudioManager;
extern CPlayer          g_Player;

/////////////////////////////////
/*/ External Globals          /*/ 
/////////////////////////////////

extern float g_FrameTime;

//////////////////////////////////////////////////////////////////////////
/// Constructor for a Plasma Shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CBomb::CBomb()
{ 
   m_ExplosionEffect = NULL;
}

//////////////////////////////////////////////////////////////////////////
/// Destructor for a plasma shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CBomb::~CBomb()
{
   
}

//////////////////////////////////////////////////////////////////////////
/// Setup the a plasma shot's attributes
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBomb::SetUp(int shotOwner, int damage, float damageradius, CGameObject * projectileowner)
{
   // Save the type of object that shot this weapon
   SetOwner(shotOwner);

   // Amount rotated thus far in degrees
   m_AmountRotatedThisFrame = 0.0f;
   m_AmountRotatedTotal = 0.0f;

   // Set the object to active
   m_Active = true;

   m_BombExplode = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_TINY], 7);

   // Damage of the projectile
   SetDamage(damage);
   m_Velocity = 0.0f;
   m_DamageRadius = damageradius;

   // Pointer to the game object that owns this shot
   m_ProjectileOwner = projectileowner;

   // Make main weapon sound
   m_BombDrop->Play3DAudio(100, 10000.0f, 250);
   CVector3f position;
   GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_BombDrop->UpdateSound(position, velocity);

   // Get the position of the object
   GetPosition(&m_CurPosition, 0);
   m_LastPosition = m_CurPosition; 
}

//////////////////////////////////////////////////////////////////////////
/// Update the plasma shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBomb::Update(void)
{  
   m_AmountRotatedThisFrame = PI * 0.25f * g_FrameTime;
   m_AmountRotatedTotal += m_AmountRotatedThisFrame;
   if(m_AmountRotatedTotal <= PI * 0.5f)
      RotateLocalX(m_AmountRotatedThisFrame, 0);
     
   // Fall downwards
   m_Velocity -= (30.0f * g_FrameTime);
   CObjFrame tempframe;
   CVector3f vector(0.0f, m_Velocity * g_FrameTime, 0.0f);
   tempframe = *GetFrame(0);
   tempframe.TranslateWorld(&vector);
   SetFrame(&tempframe, 0);


   /*
   // Update the engine sound
   GetPosition(&m_CurPosition, 0);
   CVector3f Velocity;
   Velocity.x = (m_CurPosition.x - m_LastPosition.x) / g_FrameTime;
   Velocity.y = (m_CurPosition.y - m_LastPosition.y) / g_FrameTime;
   Velocity.z = (m_CurPosition.z - m_LastPosition.z) / g_FrameTime;
   m_DropBomb->UpdateSound(m_CurPosition, Velocity);
   m_LastPosition = m_CurPosition;
   */

   CGameObject::Update();
}

//////////////////////////////////////////////////////////////////////////
/// Collision response for this plasma object
/// 
/// Input:     CGameObject *obj - The object it collided with
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBomb::Collide(const CGameObject *obj)
{ 
   // Object is the terrain 
   if(!obj)
   {
      // create terrain deformation
      CVector2f Center;
      Center.x = m_RenderObj.GetSphere()->Center.x;
      Center.y = m_RenderObj.GetSphere()->Center.z;
      g_pCurrLevel->DeformCircle(&Center, m_DamageRadius * 0.5f, m_Damage * -.075f);

      Color3F Color = { .6274f, .4274f, .1412f};
      g_pCurrLevel->ColorCircle(&Center, m_DamageRadius * 0.5f, &Color);
      
      CParticleSystem *dirt = g_ParticleManager.ObtainSystem();

      if(dirt)
         g_ParticleManager.CreateSystem(dirt, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.DIRT, LOD_BOTH, 20.0f, 200.0f);
      
      // Kill the shot
      Die();
   }
   
   // Object is a game object
   else
   {
      if(m_ShotOwner == ENEMY_SHOT && obj->m_RadarType != RADAR_ENEMY && obj->m_RadarType != RADAR_PROJECTILE)
          Die();
   } 
}

//////////////////////////////////////////////////////////////////////////
/// Reset the object so that it can be used again
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBomb::Reset(void)
{

}

//////////////////////////////////////////////////////////////////////////
/// Kill the plasmashot and everything associated with it
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBomb::Die(void)
{
   // Obtain a free system
   m_ExplosionEffect = g_ParticleManager.ObtainSystem();

   m_BombDrop->StopAudio();
   
   // If obtained a particle effect then we use it
   if(m_ExplosionEffect)
   {
      // Create an effect for the shot
      g_ParticleManager.CreateSystem(m_ExplosionEffect, *(GetFrame(0)->GetPosition()),
      NULL, g_ParticleManager.MISSILE_EXP, LOD_BOTH);
   }  

   // Play the sound of the explosion
   m_BombExplode->Play3DAudio(125.0f, 10000.0f, 250);   
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_BombExplode->UpdateSound(position, velocity);

   // Cause damage to units around
   AreaOfEffectDamage(m_DamageRadius, m_Damage);
   
   // Remove from the obj list and quad tree
   m_Active = false;
   RemoveFromQuadtree();
}
