/// \file plasmashot.cpp
/// The plasma shot projectile
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/plasmashot.h"
#include "gamelogic/objectfactory.h"
#include "graphics/particlemanager.h"
#include "core/gamemain.h"
#include "graphics/quadtree.h"
#include "core/oglwindow.h"
#include "sound/audiomanager.h"
#include "gamelogic/player.h"
#include "graphics/terrain.h"

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

CPlasmaShot::CPlasmaShot()
{

}

//////////////////////////////////////////////////////////////////////////
/// Destructor for a plasma shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CPlasmaShot::~CPlasmaShot()
{
   
}

//////////////////////////////////////////////////////////////////////////
/// Setup the a plasma shot's attributes
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlasmaShot::SetUp(int shotowner, CGameObject * projectileowner, float damage, float damageradius, float velocity)
{
   // Save the type of object that shot this weapon
   SetOwner(shotowner);
   
   // Damage of the projectile
   SetDamage((int)damage);

   m_ProjectileOwner = projectileowner;

   if(shotowner == PLAYER_SHOT || FRIENDLY_SHOT)
   {
      m_PlasmaExplode = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_TINY], 9);
      m_PlasmaInAir = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLASMAINAIR], 1);
   }
   else if(shotowner == ENEMY_SHOT)
   {
      m_PlasmaExplode = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_TINY], 7);
      m_PlasmaInAir = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLASMAINAIR], 6);    
   }

   m_PlasmaInAir->Play3DAudio(10.0f, 10000.0f, 100);

   m_Active = true;
   m_Velocity = velocity;
   m_DamageRadius = damageradius;

   m_CreationTime = g_time;
   m_LifeTime = 3.0f;

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

void CPlasmaShot::Update(void)
{
   // Move the shot forward
   MoveForward(g_FrameTime * m_Velocity, 0);

   if((m_CreationTime + m_LifeTime) < g_time)
        Die();

   // Update the ambient sound associated with this object
   GetPosition(&m_CurPosition, 0);
   CVector3f Velocity;
   Velocity.x = (m_CurPosition.x - m_LastPosition.x) / g_FrameTime;
   Velocity.y = (m_CurPosition.y - m_LastPosition.y) / g_FrameTime;
   Velocity.z = (m_CurPosition.z - m_LastPosition.z) / g_FrameTime;
   m_PlasmaInAir->UpdateSound(m_CurPosition, Velocity);
   m_LastPosition = m_CurPosition;

   CGameObject::Update();
}

//////////////////////////////////////////////////////////////////////////
/// Collision response for this plasma object
/// 
/// Input:     CGameObject *obj - The object it collided with
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlasmaShot::Collide(const CGameObject *obj)
{ 
   // Object is the terrain 
   if(!obj)
   {
      // create terrain deformation
      CVector2f Center;
      Center.x = m_RenderObj.GetSphere()->Center.x;
      Center.y = m_RenderObj.GetSphere()->Center.z;
      g_pCurrLevel->DeformCircle(&Center, m_DamageRadius * 0.5f, m_Damage * -.05f);

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
      if(m_ShotOwner == PLAYER_SHOT && obj->GetType() != OBJ_PLAYER && obj->m_RadarType != RADAR_PROJECTILE)
      {
         Die();
         if(obj->m_RadarType == RADAR_ENEMY)
            g_Player.m_HitsWithPlasma++;
      }
      if(m_ShotOwner == FRIENDLY_SHOT && obj->GetType() != OBJ_FTURRET && obj->m_RadarType != RADAR_PROJECTILE)
      {
         Die();
      }
   } 
}

//////////////////////////////////////////////////////////////////////////
/// Reset the object so that it can be used again
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlasmaShot::Reset(void)
{

}

//////////////////////////////////////////////////////////////////////////
/// Kill the plasmashot and everything associated with it
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlasmaShot::Die(void)
{
   m_ExplosionEffect = g_ParticleManager.ObtainSystem();

   // If obtained a particle effect then we use it
   if(m_ExplosionEffect)
   {
      if(m_ShotOwner == PLAYER_SHOT || m_ShotOwner == FRIENDLY_SHOT)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(m_ExplosionEffect, *(GetFrame(0)->GetPosition()),
         NULL, g_ParticleManager.PLASMAEXP, LOD_BOTH);
         if(m_ParticleEffect)
         {
            m_ExplosionEffect->m_StartColor = m_ParticleEffect->m_StartColor;
            float ratio = g_Player.m_CurMainWeaponDamage / g_Player.m_MaxMainWeaponDamage;
            m_ExplosionEffect->SetLife(m_ExplosionEffect->GetLife() * ratio);

            if(m_ExplosionEffect->GetLife() <= 0.3f)
               m_ExplosionEffect->SetLife(0.3f);
         }
      }
      else if(m_ShotOwner == ENEMY_SHOT)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(m_ExplosionEffect, *(GetFrame(0)->GetPosition()),
         NULL, g_ParticleManager.ENEMY_PLASMAEXP, LOD_BOTH);
      }
      else
        bool debug = true;
   }  

   // Cause damage to units around
   AreaOfEffectDamage(m_DamageRadius, m_Damage);

   m_PlasmaInAir->StopAudio();

   m_PlasmaExplode->Play3DAudio(40.0f, 10000.0f, 100 + (unsigned int)m_Damage);   
   CVector3f position;
   GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_PlasmaExplode->UpdateSound(position, velocity);

   // If this shot has an effect destroy it
   if(m_ParticleEffect)
      m_ParticleEffect->SetSystemAlive(false);
     
   m_Active = false;
   RemoveFromQuadtree();
}



