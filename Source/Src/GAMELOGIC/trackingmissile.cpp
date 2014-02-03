/// \file trackingmissile.cpp
/// The missile projectile
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/trackingmissile.h"
#include "gamelogic/objectfactory.h"
#include "gamelogic/player.h"
#include "graphics/particlemanager.h"
#include "core/gamemain.h"
#include "graphics/quadtree.h"
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
extern CPlayer				g_Player;

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

CTrackingMissile::CTrackingMissile()
{ 
	m_Target = 0;
	m_FlyLeft = true;
}

//////////////////////////////////////////////////////////////////////////
/// Destructor for a plasma shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CTrackingMissile::~CTrackingMissile()
{
   
}

//////////////////////////////////////////////////////////////////////////
/// Setup the a plasma shot's attributes
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CTrackingMissile::SetUp(int shotOwner, int damage, bool left, float damageradius, float accuracy, CGameObject * projectileowner)
{
   // Save the type of object that shot this weapon
   SetOwner(shotOwner);

	m_Tracking = false;
	m_UpdatePeriod = accuracy;
	m_SlerpScale = 1.0f / m_UpdatePeriod;
   m_DamageRadius = damageradius;
   
   // Damage of the projectile
   SetDamage(damage);
   m_Velocity = 40.0f;

   // Pointer to the game object that owns this shot
   m_ProjectileOwner = projectileowner;

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

   m_MissileInAir->Play3DAudio(10.0f, 10000.0f, 75);
   
   m_Active = true;

   m_CreationTime = g_time;
   m_LifeTime = 10.0f;

	CObjFrame temp = *m_RenderObj.GetFrame(0);
	temp.GetOrientation(&m_Q0);
	m_Q2 = m_Q0;
	temp.RotateLocalX(-PI / 6.0f);
	temp.GetOrientation(&m_Q1);

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

void CTrackingMissile::Update(void)
{
	CQuaternion q;
	float t;

	m_Age = g_time - m_CreationTime;

   if(m_Age > m_LifeTime)
   {
      Die();
      return;
   }

	if (!m_Tracking)
	{
		if (m_Age <= 0.4f)
		{
			if (m_FlyLeft)
				m_RenderObj.MoveRight(-20.0f * g_FrameTime, 0);
			else
				m_RenderObj.MoveRight(20.0f * g_FrameTime, 0);
	
			t = m_Age;
			QuatSlerp(&q, &m_Q0, &m_Q1, t * 2.5f);
			m_RenderObj.GetFrame(0)->SetOrientation(&q);
		}
		else if (m_Age <= 0.8f && !m_Target)
		{
			t = m_Age - 0.4f;
			QuatSlerp(&q, &m_Q1, &m_Q2, t * 2.5f);
			m_RenderObj.GetFrame(0)->SetOrientation(&q);
		}
		else
		{
			m_Tracking = true;
			m_RenderObj.GetFrame(0)->GetOrientation(&m_Q0);
			m_Q1 = m_Q0;
			m_Velocity = 120.0f;
			m_LastUpdate = g_time - m_UpdatePeriod;
		}
	}
	else
	{
		if (m_Target)
		{
			if (m_Target->IsActive())
			{
				t = g_time - m_LastUpdate;

				if (t >= m_UpdatePeriod)
				{
					CVector3f oldfwd, newfwd, tpos, pos, lead(0.0f, 0.0f, 0.0f);

					m_Q0 = m_Q1;

					// Find new orientation, lead target a little
					m_RenderObj.GetFrame(0)->GetForward(&oldfwd);
					m_Target->GetPosition(&tpos, 0);
					m_Target->GetVelocity(&lead);
					Vec3fScale(&lead, &lead, m_UpdatePeriod);
					Vec3fAdd(&tpos, &tpos, &lead);
					GetPosition(&pos, 0);
					Vec3fSubtract(&newfwd, &tpos, &pos);
					Vec3fNormalize(&newfwd, &newfwd);

					QuatRotationArc(&q, &oldfwd, &newfwd);
					QuatCrossProduct(&m_Q1, &q, &m_Q1);

					m_LastUpdate += m_UpdatePeriod;
					t = g_time - m_LastUpdate;
				}

				QuatSlerp(&q, &m_Q0, &m_Q1, t * m_SlerpScale);
				m_RenderObj.GetFrame(0)->SetOrientation(&q);
			}
			else
				m_Target = 0;
		}
	}

   // Move the shot forward
   MoveForward(m_Velocity * g_FrameTime, 0);

   // Update the ambient sound
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

void CTrackingMissile::Collide(const CGameObject *obj)
{ 
 // Object is the terrain 
   if(!obj)
   {
      // create terrain deformation
      CVector2f Center;
      Center.x = m_RenderObj.GetSphere()->Center.x;
      Center.y = m_RenderObj.GetSphere()->Center.z;
      g_pCurrLevel->DeformCircle(&Center, m_DamageRadius, m_Damage * -.025f);

      Color3F Color = { .6274f, .4274f, .1412f};
      g_pCurrLevel->ColorCircle(&Center, m_DamageRadius, &Color);

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
            g_Player.m_HitsWithTrkMissiles++;
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

void CTrackingMissile::Reset(void)
{

}

//////////////////////////////////////////////////////////////////////////
/// Kill the plasmashot and everything associated with it
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CTrackingMissile::Die(void)
{
   m_ExplosionEffect = g_ParticleManager.ObtainSystem();

   // If obtained a particle effect then we use it
   if(m_ExplosionEffect)
   {
      // Create an effect for the shot
      g_ParticleManager.CreateSystem(m_ExplosionEffect, *(GetFrame(0)->GetPosition()),
      NULL, g_ParticleManager.TRKMISSILE_EXP, LOD_BOTH);
   }  

   m_MissileExplode->Play3DAudio(25.0f, 10000.0f, 128);   
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity;
   velocity.x = 0.0f;
   velocity.y = 0.0f;
   velocity.z = 0.0f;
   m_MissileExplode->UpdateSound(position, velocity);

   m_MissileInAir->StopAudio();

   // Cause damage to units around
   AreaOfEffectDamage(m_DamageRadius, m_Damage);
 
   // Clean up the particle systems associated with this missile
   if(m_FireTrail)
      m_FireTrail->SetSystemAlive(false);
   if(m_SmokeTrail)
      m_SmokeTrail->SetSystemAlive(false);
   
   m_Active = false;
   RemoveFromQuadtree();
}
