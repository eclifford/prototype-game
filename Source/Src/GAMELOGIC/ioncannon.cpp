/// \file ioncannon.cpp
/// The ion cannon blast
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\ioncannon.h"
#include "gamelogic\objectfactory.h"
#include "graphics\particlemanager.h"
#include "core\gamemain.h"
#include "graphics\quadtree.h"
#include "gamelogic\mothership.h"
#include "graphics\light.h"
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
extern CMothership		g_MotherShip;
extern CPositionalLight g_IonLight;

/////////////////////////////////
/*/ Global Variables          /*/ 
/////////////////////////////////

extern float g_FrameTime;

//////////////////////////////////////////////////////////////////////////
/// Constructor for a Plasma Shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CIonCannon::CIonCannon()
{ 

}

//////////////////////////////////////////////////////////////////////////
/// Destructor for a plasma shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CIonCannon::~CIonCannon()
{
   
}

//////////////////////////////////////////////////////////////////////////
/// Setup the a plasma shot's attributes
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CIonCannon::SetUp(int shotOwner, int damage, CGameObject * projectileowner)
{
   // Save the type of object that shot this weapon
   SetOwner(ION_CANNON);
   
   // Damage of the projectile
   SetDamage(damage);

   m_IonWave = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_IONWAVE], 10);

   // Pointer to the game object that owns this shot
   m_ProjectileOwner = projectileowner;

   m_Active = true;
}

//////////////////////////////////////////////////////////////////////////
/// Update the plasma shot
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CIonCannon::Update(void)
{
   // Move the shot forward
   MoveUp(-(200.0f * g_FrameTime), 0);

   CGameObject::Update();

   Point3f Position;
   m_RenderObj.GetPosition(&Position, 0);
   g_IonLight.SetPosition(&Position);
}

//////////////////////////////////////////////////////////////////////////
/// Collision response for this plasma object
/// 
/// Input:     CGameObject *obj - The object it collided with
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CIonCannon::Collide(const CGameObject *obj)
{ 
   // Object is the terrain 
   if(!obj)
   {
      // create terrain deformation
      CVector2f Center;
      Center.x = m_RenderObj.GetSphere()->Center.x;
      Center.y = m_RenderObj.GetSphere()->Center.z;
      g_pCurrLevel->DeformCircle(&Center, 100.0f, -20.0f);

      Color3F Color = { .6274f, .4274f, .1412f};
      g_pCurrLevel->ColorCircle(&Center, 100.0f, &Color);

      // Kill the particle
      Die();
   }
   
   // Object is a game object
   else
   {

   } 
}

//////////////////////////////////////////////////////////////////////////
/// Reset the object so that it can be used again
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CIonCannon::Reset(void)
{

}

//////////////////////////////////////////////////////////////////////////
/// Kill the plasmashot and everything associated with it
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CIonCannon::Die(void)
{
   m_ExplosionEffect = g_ParticleManager.ObtainSystem();

   // If obtained a particle effect then we use it
   if(m_ExplosionEffect)
   {
      // Create an effect for the shot
      g_ParticleManager.CreateSystem(m_ExplosionEffect, *(GetFrame(0)->GetPosition()),
      NULL, g_ParticleManager.ION_BLAST, LOD_BOTH);
   }  

   if(m_ParticleEffect)
      m_ParticleEffect->SetSystemDyingAge(m_ParticleEffect->GetSystemAge() + 3.0f);

      // Make explosion sound
   m_IonWave->Play3DAudio(100, 10000.0f, 300);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_IonWave->UpdateSound(position, velocity);

   AreaOfEffectDamage(200.0f, 500.0f);
	g_MotherShip.IonDone();
   
   m_Active = false;
   RemoveFromQuadtree();
}
