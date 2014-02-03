/// \file laser.cpp
/// The laser projectile
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\laser.h"
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
/*/ External Global Variables /*/ 
/////////////////////////////////

extern float g_FrameTime;

//////////////////////////////////////////////////////////////////////////
/// Constructor for a Plasma Shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CLaser::CLaser()
{

}

//////////////////////////////////////////////////////////////////////////
/// Destructor for a plasma shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CLaser::~CLaser()
{
   
}

//////////////////////////////////////////////////////////////////////////
/// Setup the a plasma shot's attributes
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CLaser::SetUp(int shotOwner, int damage, float damageradius, CGameObject * projectileowner)
{
   // Save the type of object that shot this weapon
   SetOwner(shotOwner);

   // Set this object to active
   m_Active = true;
   
   // Damage of the projectile
   SetDamage(damage);

   // Pointer to the game object that owns this shot
   m_ProjectileOwner = projectileowner;

   m_DamageRadius = damageradius;
   m_LaserDie = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LASERHIT], 6); 

   m_Velocity = 500.0f;

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

void CLaser::Update(void)
{
   // Move the shot forward
   MoveForward(m_Velocity * g_FrameTime, 0);

   CGameObject::Update();
}

//////////////////////////////////////////////////////////////////////////
/// Collision response for this plasma object
/// 
/// Input:     CGameObject *obj - The object it collided with
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CLaser::Collide(const CGameObject *obj)
{ 
  // Object is the terrain 
   if(!obj)
   {
      // create terrain deformation
      CVector2f Center;
      Center.x = m_RenderObj.GetSphere()->Center.x;
      Center.y = m_RenderObj.GetSphere()->Center.z;
      g_pCurrLevel->DeformCircle(&Center, m_DamageRadius * 0.5f, m_Damage * -.05f);
      
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

void CLaser::Reset(void)
{

}

//////////////////////////////////////////////////////////////////////////
/// Kill the plasmashot and everything associated with it
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CLaser::Die(void)
{
   m_ExplosionEffect = g_ParticleManager.ObtainSystem();

   // If obtained a particle effect then we use it
   if(m_ExplosionEffect)
   {
      // Create an effect for the shot
      g_ParticleManager.CreateSystem(m_ExplosionEffect, *(GetFrame(0)->GetPosition()),
      NULL, g_ParticleManager.LASERSHOT_HIT, LOD_BOTH);
   }  

   // Play the sound of a laser hitting the ground
   m_LaserDie->Play3DAudio(5.0f, 10000.0f, 70);   
   CVector3f position;
   GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_LaserDie->UpdateSound(position, velocity);

   // Cause damage to units around
   AreaOfEffectDamage(m_DamageRadius, m_Damage);

   m_Active = false;
   RemoveFromQuadtree();
}
