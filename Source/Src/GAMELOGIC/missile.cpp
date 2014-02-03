/// \file missile.cpp
/// The missile projectile
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\missile.h"
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

CMissile::CMissile()
{ 
   m_SmokeTrail = NULL;
   m_FireTrail = NULL;
   m_ExplosionEffect = NULL;
}

//////////////////////////////////////////////////////////////////////////
/// Destructor for a plasma shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CMissile::~CMissile()
{
   
}

//////////////////////////////////////////////////////////////////////////
/// Setup the a plasma shot's attributes
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMissile::SetUp(int shotOwner, int damage, int myindex, float damageradius, CGameObject * projectileowner)
{
   // Save the type of object that shot this weapon
   SetOwner(shotOwner);

   // Set the object to active
   m_Active = true;

   if(shotOwner == PLAYER_SHOT)
   {
      m_MissileExplode = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_TINY], 9);
      m_MissileInAir = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_MISSILEINAIR], 7);
   }
   else if(shotOwner == ENEMY_SHOT)
   {
      m_MissileExplode = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_TINY], 7);
      m_MissileInAir = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_MISSILEINAIR], 6);
   }

   m_MissileInAir->Play3DAudio(10.0f, 10000.0f, 40);

   // Damage of the projectile
   SetDamage(damage);
   m_Velocity = 40.0f;
   m_DamageRadius = damageradius;

   // Pointer to the game object that owns this shot
   m_ProjectileOwner = projectileowner;

   m_CreationTime = g_time;
   m_LifeTime = 5.0f;

	m_RenderObj.GetFrame(0)->GetOrientation(&m_Q1);

	// Rotate according to index
	switch (myindex)
	{
	case 0:
		m_RenderObj.RotateLocalZ(PI / 2.0f, 0);
		break;
	case 1:
		m_RenderObj.RotateLocalZ(-PI / 2.0f, 0);
		break;
	case 2:
		m_RenderObj.RotateLocalZ(PI / 8.0f * 3.0f, 0);
		break;
	case 3:
		m_RenderObj.RotateLocalZ(-PI / 8.0f * 3.0f, 0);
		break;
	case 4:
		m_RenderObj.RotateLocalZ(PI / 4.0f, 0);
		break;
	case 5:
		m_RenderObj.RotateLocalZ(-PI / 4.0f, 0);
		break;
	case 6:
		m_RenderObj.RotateLocalZ(PI / 8.0f, 0);
		break;
	case 7:
		m_RenderObj.RotateLocalZ(-PI / 8.0f, 0);
		break;
	case 8:
		break;
	case 9:
		break;
	}

	CObjFrame TempFrame = *m_RenderObj.GetFrame(0);
	m_RenderObj.RotateLocalX(-PI / 3.0f, 0);
	m_RenderObj.GetFrame(0)->GetOrientation(&m_Q0);

	TempFrame.RotateLocalX(PI / 6.0f);
	TempFrame.GetOrientation(&m_Q2);

	m_Delay = (rand() % 8 + 1) / 10.0f;

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

void CMissile::Update(void)
{
	m_Age = g_time - m_CreationTime;

   if(m_Age > m_LifeTime)
   {
      Die();
      return;
   }

	CQuaternion q;
	float t;
	if (m_Age <= 0.5f)
	{
		t = m_Age;
		QuatSlerp(&q, &m_Q0, &m_Q1, t * 2.0f);
		m_RenderObj.GetFrame(0)->SetOrientation(&q);
	}
	else if (m_Age > 0.5f && m_Age <= 0.5f + m_Delay)
	{
		// Do nothing
	}
	else if (m_Age <= 1.0f + m_Delay)
	{
		m_Velocity = 60.0f;
		t = m_Age - 0.5f - m_Delay;
		QuatSlerp(&q, &m_Q1, &m_Q2, t * 2.0f);
		m_RenderObj.GetFrame(0)->SetOrientation(&q);
	}
	else if (m_Age <= 1.2f + m_Delay)
	{
		t = m_Age - 1.0f - m_Delay;
		QuatSlerp(&q, &m_Q2, &m_Q1, t * 5.0f);
		m_RenderObj.GetFrame(0)->SetOrientation(&q);
	}
	else if (m_Velocity < 120.0f)
		m_Velocity = 120.0f;

   // Move the shot forward
   MoveForward(m_Velocity * g_FrameTime, 0);

   // Update the ambient sound associated with this object
   GetPosition(&m_CurPosition, 0);
   CVector3f Velocity;
   Velocity.x = (m_CurPosition.x - m_LastPosition.x) / g_FrameTime;
   Velocity.y = (m_CurPosition.y - m_LastPosition.y) / g_FrameTime;
   Velocity.z = (m_CurPosition.z - m_LastPosition.z) / g_FrameTime;
   m_MissileInAir->UpdateSound(m_CurPosition, Velocity);
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

void CMissile::Collide(const CGameObject *obj)
{ 
   // Object is the terrain 
   if(!obj)
   {
      // create terrain deformation
      CVector2f Center;
      Center.x = m_RenderObj.GetSphere()->Center.x;
      Center.y = m_RenderObj.GetSphere()->Center.z;
      g_pCurrLevel->DeformCircle(&Center, m_DamageRadius * 0.5f, m_Damage * -.025f);

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
            g_Player.m_HitsWithMissiles++;
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

void CMissile::Reset(void)
{

}

//////////////////////////////////////////////////////////////////////////
/// Kill the plasmashot and everything associated with it
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMissile::Die(void)
{
   // Obtain a free system
   m_ExplosionEffect = g_ParticleManager.ObtainSystem();

   // If obtained a particle effect then we use it
   if(m_ExplosionEffect)
   {
      // Create an effect for the shot
      g_ParticleManager.CreateSystem(m_ExplosionEffect, *(GetFrame(0)->GetPosition()),
      NULL, g_ParticleManager.MISSILE_EXP, LOD_BOTH);
   }  

   // Play the sound of the explosion
   m_MissileExplode->Play3DAudio(25.0f, 10000.0f, 128);   
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_MissileExplode->UpdateSound(position, velocity);

   // Cause damage to units around
   AreaOfEffectDamage(m_DamageRadius, m_Damage);

   // Clean up the particle systems associated with this missile
   if(m_FireTrail)
      m_FireTrail->SetSystemAlive(false);
   if(m_SmokeTrail)
      m_SmokeTrail->SetSystemAlive(false);

   m_MissileInAir->StopAudio();
   
   // Remove from the obj list and quad tree
   m_Active = false;
   RemoveFromQuadtree();
}
