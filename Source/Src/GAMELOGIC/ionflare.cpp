/// \file IonFlare.cpp
/// The plasma shot projectile
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\IonFlare.h"
#include "gamelogic\objectfactory.h"
#include "graphics\particlemanager.h"
#include "core\gamemain.h"
#include "graphics\quadtree.h"
#include "core\oglwindow.h"
#include "sound\audiomanager.h"
#include "gamelogic\player.h"
#include "gamelogic\mothership.h"
#include "graphics\terrain.h"

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
extern CMothership		g_MotherShip;

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

CIonFlare::CIonFlare()
{

}

//////////////////////////////////////////////////////////////////////////
/// Destructor for a plasma shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CIonFlare::~CIonFlare()
{
   
}

//////////////////////////////////////////////////////////////////////////
/// Setup the a plasma shot's attributes
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CIonFlare::SetUp(int shotowner, CGameObject * projectileowner, float damage, float damageradius, float velocity)
{
   // Save the type of object that shot this weapon
   SetOwner(shotowner);
   
   // Damage of the projectile
   SetDamage((int)damage);

   m_PlasmaExplode = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLAYER_IONCANNON], 10);

   m_Active = true;
   m_Velocity = velocity;
   m_DamageRadius = damageradius;

   m_CreationTime = g_time;
   m_LifeTime = 3.0f;
}

//////////////////////////////////////////////////////////////////////////
/// Update the plasma shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CIonFlare::Update(void)
{
   // Move the shot forward
   MoveForward(g_FrameTime * m_Velocity, 0);

   if((m_CreationTime + m_LifeTime) < g_time)
        Die();

   CGameObject::Update();
}

//////////////////////////////////////////////////////////////////////////
/// Collision response for this plasma object
/// 
/// Input:     CGameObject *obj - The object it collided with
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CIonFlare::Collide(const CGameObject *obj)
{ 
   // Object is the terrain 
   if(!obj)
   {
      // Kill the shot
      Die();
   }
   
   // Object is a game object
   else
   {
	   if(obj->GetType() != OBJ_PLAYER)
		   Die();
      /*
      if(m_ShotOwner == ENEMY_SHOT && (obj->GetType() == OBJ_PLAYER || obj->GetType() == OBJ_CONVOY || obj->GetType() == OBJ_FTURRET))
          Die();
      if(m_ShotOwner == PLAYER_SHOT && obj->GetType() != OBJ_PLAYER && obj->m_RadarType != RADAR_PROJECTILE)
      {
         Die();
         if(obj->m_RadarType == RADAR_ENEMY)
            g_Player.m_HitsWithPlasma++;
      }*/
   } 	
}

//////////////////////////////////////////////////////////////////////////
/// Reset the object so that it can be used again
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CIonFlare::Reset(void)
{

}

//////////////////////////////////////////////////////////////////////////
/// Kill the IonFlare and everything associated with it
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CIonFlare::Die(void)
{
   CVector3f pos;

	m_RenderObj.GetFrame(0)->GetPosition(&pos);
	g_MotherShip.StartIonRun(&pos);

   //m_ExplosionEffect = g_ParticleManager.ObtainSystem();

   // Get a particle effect for the shot if one available
   m_ExplosionEffect = g_ParticleManager.ObtainSystem();

   // If obtained a particle effect then we use it
   if(m_ExplosionEffect != NULL)
   {
      g_ParticleManager.ClearSystem(m_ExplosionEffect);
      
      // Create an effect for the shot
      g_ParticleManager.CreateSystem(m_ExplosionEffect, *(GetFrame(0)->GetPosition()),
      GetFrame(0)->GetPosition(), g_ParticleManager.IONFLARE_GROUND, LOD_NONE, 20.0f, 500.0f);
   }

   // If this shot has an effect destroy it
   if(m_ParticleEffect)
      m_ParticleEffect->SetSystemAlive(false);

	/*
   CObjFrame TempMothershippos;
   TempMothershippos = *GetFrame(0);
   TempMothershippos.MoveUp(250.0f);

   CIonCannon * ion = g_ObjectFactory.CreateIonCannon();
   ion->SetUp(PLAYER_SHOT, 1000, this);
   ion->SetFrame(&TempMothershippos, 0);

   // Get a particle effect for the shot if one available
   ion->m_ParticleEffect = g_ParticleManager.ObtainSystem();

   // If obtained a particle effect then we use it
   if(ion->m_ParticleEffect != NULL)
   {
      // Create an effect for the shot
      g_ParticleManager.CreateSystem(ion->m_ParticleEffect, *(ion->GetFrame(0)->GetPosition()),
      ion->GetFrame(0)->GetPosition(), g_ParticleManager.ION_BEAM, LOD_BOTH);
   }  
*/     
   m_Active = false;
   RemoveFromQuadtree();
}



