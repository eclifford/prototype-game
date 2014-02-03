/// \file player.cpp
/// The player
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\player.h"
#include "graphics\geometrymanager.h"
#include "graphics\quadtree.h"
#include "gamelogic\objectfactory.h"
#include "graphics\particlemanager.h"
#include "core\gamemain.h" 
#include "gamelogic\ioncannon.h"
#include "sound\audiomanager.h"
#include "input\inputmanager.h"
#include "camera\camera.h"
#include "math3d\intersection.h"
#include "graphics\color4f.h"
#include "graphics\hudtext.h"
#include "physics\physics.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CQuadtree        g_Quadtree;
extern CObjectFactory   g_ObjectFactory;
extern CParticleManager g_ParticleManager;
extern CTextureManager  g_TextureManager;
extern CGameMain        GameMain;
extern COpenGLWindow		*pGLWIN;	
extern CAudioManager    g_AudioManager;
extern CPlayerCamera		g_PlayerCamera;
extern CBaseCamera		*g_Camera;
extern CGeometryManager g_GeometryManager;
extern CHudTextManager g_HudTextManager;

/////////////////////////////////
/*/ Globals                   /*/ 
/////////////////////////////////

extern float g_time;
extern float g_FrameTime;

void CPlayer::Update2(void)
{
	m_pMissilePods->Update();
	CGameObject::Update();
}

 CPlayer::CPlayer() 
{
    m_Type = OBJ_PLAYER; 
    m_RadarType = RADAR_PLAYER;
    m_pTarget = 0;
    m_ShowShield = false;
    m_RigidBody = 0;    
}

CPlayer::~CPlayer()
{
    bool debug = true;
}

//////////////////////////////////////////////////////////////////////////
/// Setup the players attributes
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::SetupPlayer()
{
     m_Type = OBJ_PLAYER; 
    m_RigidBody = new CRigidBody; 
    m_RadarType = RADAR_PLAYER;
    m_pTarget = 0;
    m_ShowShield = false;
    
   fire = 0;
   smoke = 0;
   m_pTarget = 0;
   m_ShowShield = false;
   m_TurretAngle = 0.0f;
   m_turretpos = 0.0f;

   // Activate the player 
  	m_Active = true;	
   m_InAir = false;
   m_InBoost = false;

   // Player States //////////////////////////////////////////////////////
      m_CurSecWeapon = SEC_MISSILE;        // Current selected secondary weapon
      m_CurPlayerState = FINE;             // Player damage state
      m_CurMainWeaponUpgrade = MW_Level1;  // State of upgrades to main weapon
      m_CurMissileUpgrade = MS_Level1;     // State of upgrades to cluster missiles
      m_CurTrkMissileUpgrade = TMS_Level1; // State of upgrades to trk missiles
      m_CurTurretState = STANDBY;          // State of turret (Recoiling or Standby)  
	  m_CurElevation = 0.0f;
      m_MaxTurretElevation = 0.5f;
      m_MinTurretElevation = -0.2f;

   // Shield / Hull stats ////////////////////////////////////////////////
      m_MaxShield = m_Shield = 200;  // Max shield 
      m_MaxHull = m_Hull = 200;      // Max Hull
      m_ShieldRegenPerSec = 5;       // Shield Regen

   // Energy stats ////////////////////////////////////////////////
      m_MaxEnergy = m_Energy = 100;  // Max Energy 
      m_EnergyRegenPerSec = 5;      // Energy Regen

   // Player Movement ////////////////////////////////////////////////////
      m_RigidBody->Initialize(this, 600.0f, m_OBB.x * 2.0f, m_OBB.y * 2.0f, m_OBB.z * 2.0f);
      m_RigidBody->SetTerrainOffset(1.0f);            
      m_MaxVelocity  = 70.0f;       // Max velocity in m/s
      m_MaxBoostVelocity = 200.0f;
      m_BoostAcceleration = 1.0f;
      m_CurBoostVelocity = 0.0f;
      m_AccelerationTime = 1.0f;    // Time to reach max velocity
      m_TurnSpeedScale = 500.0f;    // Scale = default
      m_RigidBody->SetParameters(m_MaxVelocity, m_AccelerationTime);
      m_RigidBody->SetThruster(RB_UP, 12000);
      m_RigidBody->SetTurnMomentScale(m_TurnSpeedScale);
      m_BoostEnergyPerSec = 20.0f;
  
   // Main weapon stats //////////////////////////////////////////////////
      m_MainWeaponLastFired = 0.0f;      // Last time main weapon was fired
      m_MainWeaponCooldown = 0.20f;       // Cooldown of main weapon 
      m_MinMainWeaponDamage = 15.0f;     // Starting damage (no charge)
      m_MaxMainWeaponDamage = 120.0f;    // Weapon damage at full charge
      m_CurMainWeaponDamage = m_MinMainWeaponDamage;  
      m_MinMainWeaponRadius = 5.0f;
      m_MaxMainWeaponRadius = 50.0f;
      m_CurMainWeaponRadius = m_MinMainWeaponRadius;
      m_PlasmaShotSpeed = 200.0f;        // Velocity in meters per sec of plasma shot
      m_MinEnergyUsed = 1.0f;
      m_MaxEnergyUsed = 80.0f;
      m_CurEnergyUsed = 0.0f;
   
   // Charged shot steps /////////////////////////////////////////////////
      m_MaxChargeTime = 3.0f;     // Time delay in seconds till max charge
      m_ChargeDamageStep = 0.0f;  // Damaged added this frame (based on time)
      m_ChargeColorStep = 0.25f;  // Color added per second
      m_SpreadStep = 0.5f;       // Charge field spread per second
      m_ChargeEnergyStep = 0.0;
   
   // Missile stats //////////////////////////////////////////////////////
      m_MissileLastFired = 0.0f;  // Last time a missile was fired
      m_MissileCoolDown = 2.0f;   // Time in seconds of missile cooldown
      m_MissileDamage = 50.0f;    // Damage of a cluster missile
      m_ClusterSize = 6;          // Number of starting cluster missile bays
      m_NumMissiles = 40;        // Amount of missiles
      m_MissileSpeed = 200.0f;    // Velocity in m/s of player fired missile
      m_MissileRadius = 30.0f;

   // Tracking Missile stats /////////////////////////////////////////////
      m_TrkMissileLastFired = 0.0f;  // Last time a tracking missile was fired
      m_TrkMissileCoolDown = 0.25f;  // Time in seconds of trk missile cooldown
      m_TrkMissileDamage = 50.0f;    // Trk Missile Damage
      m_TrkMissileRadius = 10.0f;
      m_CurrentMissileBay = 0;       // Current bay firing 
      m_NumTrackingMissiles = 40;   // Number of tracking missiles
      m_TrkMissileAccuracy = 0.4f;

   // Special Abilities //////////////////////////////////////////////////
      m_IonLastFired = 0.0f;           // Ion Cannon last fired
      m_IonCoolDown = 130.0f;           // Time in seconds of ion cannon cooldown
      m_JumpJetPowerInSeconds = 4.0f;  // Max time in the air
      m_CurrentJumpJetPower = 4.0f;    // Set to max at begin
      m_JumpJetEnergyPerSec = 15.0f;

   // Player Sound FX ////////////////////////////////////////////////////
      m_pMissilesFired = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_MISSILEFIRED], 9);
      m_pMainWeaponBlast = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLAYER_TURRET], 10);
      m_pPlayerExplode = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_HUGE], 10);
      m_pShieldAbsorb = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLAYER_SHIELDABSORB], 10);
      m_pHullHit = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLAYER_HULLHIT], 10);
      m_pEngine = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLAYER_ENGINE], 10);
      m_pJumpJet = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLAYER_JUMPJET], 10);
      m_pWeaponChange = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_WPNCHANGE], 10);
      m_pCollide = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_OBJCOLLIDE], 10);
      m_pLockOn = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LOCKON], 10);
      m_pIonFlare = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_IONFLARE], 10);
      m_pChargeRampUp = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_CHARGE], 10);
      m_pChargeFull = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_CHARGE2], 10);
      m_pEngine->Play3DAudio(20, 10000.0f, 0);

   // Stats Setup ////////////////////////////////////////////////////////
      m_HitsWithPlasma = 0;
      m_HitsWithMissiles = 0;
      m_HitsWithTrkMissiles = 0;
      m_ShotsFiredPlasma = 0;
      m_ShotsFiredMissiles = 0;
      m_ShotsFiredTrkMissiles = 0;
      m_EnemiesDestroyed = 0;
      m_PlayerTotalDamage = 0;
      m_NumPowerUpsCollected = 0;

      m_EnemyMedTanksDestroyed = 0;
      m_EnemyHvyTanksDestroyed = 0;
      m_EnemyMslTanksDestroyed = 0;
      m_EnemyScoutsDestroyed = 0;
      m_EnemyFightersDestroyed = 0;
      m_EnemySpheresDestroyed = 0;
      m_EnemyMeleeTanksDestroyed = 0;

   // Particle FX ////////////////////////////////////////////////////////

      // Setup thruster one
      thruster1 = g_ParticleManager.ObtainSystem();
      if(thruster1)
         g_ParticleManager.CreateSystem(thruster1, m_Burner1, &m_Burner1, g_ParticleManager.BURNER, LOD_NEAR, 20.0f, 500.0f);
      
      // Setup thruster two
      thruster2 = g_ParticleManager.ObtainSystem();
      if(thruster2)
         g_ParticleManager.CreateSystem(thruster2, m_Burner2, &m_Burner2, g_ParticleManager.BURNER, LOD_NEAR, 20.0f, 500.0f);

   // Get the turrets position
   m_RenderObj.GetPosition(&m_TurretPosition, 1);

   // Time the player was created 
   m_TimeMissionStarted = g_time;


   // load the shield model
   Sphere BoundSphere;
   AABB Box;
   unsigned int GeometryIndex;
   unsigned int NumParts;
   CVector3f* pTranslations;
   int TextureIndex;

   g_GeometryManager.LoadPTGFile("resources\\models\\shieldtank.ptg", &BoundSphere,
                                 &Box, &GeometryIndex, &NumParts, &pTranslations,
                                 &TextureIndex); 

   m_pShieldModel = new CQuadtreeRenderObj;
   m_pShieldModel->Initialize(&BoundSphere, GeometryIndex, NumParts, pTranslations, TextureIndex);   
   delete [] pTranslations;
   m_pShieldMissilePods = new CQuadtreeRenderObj;

   g_GeometryManager.LoadPTGFile("resources\\models\\shieldmissilepods.ptg", &BoundSphere,
                                 &Box, &GeometryIndex, &NumParts, &pTranslations,
                                 &TextureIndex);

   m_pShieldMissilePods->Initialize(&BoundSphere, GeometryIndex, NumParts, pTranslations, TextureIndex);   
   delete [] pTranslations;
   
   g_GeometryManager.LoadPTGFile("resources\\models\\missilepods.ptg", &BoundSphere,
                                 &Box, &GeometryIndex, &NumParts, &pTranslations,
                                 &TextureIndex); 

   m_pMissilePods = new CQuadtreeRenderObj;
   m_pMissilePods->Initialize(&BoundSphere, GeometryIndex, NumParts, pTranslations, TextureIndex);
   
   delete [] pTranslations;

   m_ShowShield = false;

   //m_pShieldModel->GetPosition(&m_TurretShieldPosition, 1);

//   Vec3fSubtract(&m_TurretShieldTranslation, &m_TurretShieldTranslation, &m_TurretPosition);

   // make the shield transpaernt
   
   m_pShieldModel->m_Material.m_Diffuse.r = 0.0f;
   m_pShieldModel->m_Material.m_Diffuse.g = 0.0f;
   m_pShieldModel->m_Material.m_Diffuse.b = 1.0f;
   m_pShieldModel->m_Material.m_Diffuse.a = 0.3f;

   //m_pShieldModel->m_Material.m_Ambient.r = 0.0f;
   //m_pShieldModel->m_Material.m_Ambient.g = 0.0f;
   m_pShieldModel->m_Material.m_Ambient.b = 1.0f;
   m_pShieldModel->m_Material.m_Ambient.a = 0.3f;

   m_pShieldModel->m_Material.m_Specular.r = 0.0f;
   m_pShieldModel->m_Material.m_Specular.g = 0.0f;
   m_pShieldModel->m_Material.m_Specular.b = 1.0f;
   m_pShieldModel->m_Material.m_Specular.a = 1.0f;

   m_RenderObj.m_Material.m_Shininess = 128;

   m_pShieldMissilePods->m_Material = m_pShieldModel->m_Material;

   // store the positions
   m_pShieldModel->GetPosition(&m_TurretShieldPosition, 1);

   m_pShieldMissilePods->GetPosition(&m_ShieldPodsPosition, 0);

   m_pMissilePods->GetPosition(&m_MissilePodsPosition, 0); 
   
   // Get the position of the object
   GetPosition(&m_CurPosition, 0);
   m_LastPosition = m_CurPosition; 
}

//////////////////////////////////////////////////////////////////////////
/// Fire the main weapon
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::FireMainWeapon(bool charged)
{ 
   // Check to see if the weapon is ready to be fired
   if(g_time >= (m_MainWeaponCooldown + m_MainWeaponLastFired) && m_CurTurretState == STANDBY)
   {  
      m_pMainWeaponBlast->Play3DAudio(30, 10000.0f, unsigned int(50 + (m_CurMainWeaponDamage * 3)));
      CVector3f position;
	   GetPosition(&position, 0);
      CVector3f velocity(0.0f, 0.0f, 0.0f);
      m_pMainWeaponBlast->UpdateSound(position, velocity);

      m_MainWeaponLastFired = g_time;
      m_CurTurretState = RECOIL;

      m_ShotsFiredPlasma++;

      CPlasmaShot * shot = g_ObjectFactory.CreatePlasmaShot();
      shot->SetUp(PLAYER_SHOT, this, m_CurMainWeaponDamage, m_CurMainWeaponRadius, m_PlasmaShotSpeed);
      
      // make enemies avoid player shot
      GameMain.AddToCollisionAvoidanceList(shot);

      // Find the position of the shot
      CObjFrame frame;
      FindShotPosition(&frame);
      shot->SetFrame(&frame, 0);
      
      CParticleSystem * effect = g_ParticleManager.ObtainSystem();

      // If obtained a particle effect then we use it
      if(effect)
      {	 
		 // Create an effect for the shot
		 g_ParticleManager.CreateSystem(effect, *(shot->GetFrame(0)->GetPosition()),
		 NULL, g_ParticleManager.FLASH, LOD_BOTH, 20.0f, 500.0f);
		 if(charged && charge)
			effect->m_StartColor = charge->m_StartColor;
		 else
		 {
			 switch(m_CurMainWeaponUpgrade)
			 {
				case MW_Level1:
					effect->m_StartColor.x = 0.0f;
					effect->m_StartColor.y = 0.0f;
					effect->m_StartColor.z = 1.0f;
					break;
		   		case MW_Level2:
					effect->m_StartColor.x = 1.0f;
					effect->m_StartColor.y = 0.0f;
					effect->m_StartColor.z = 0.0f;
					break;
				case MW_Level3:
					effect->m_StartColor.x = 0.0f;
					effect->m_StartColor.y = 1.0f;
					effect->m_StartColor.z = 0.0f;
					break;
			 }
		 }
      }   
 
      // Get a particle effect for the shot if one available
      shot->m_ParticleEffect = g_ParticleManager.ObtainSystem();

      // If obtained a particle effect then we use it
      if(shot->m_ParticleEffect != NULL)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(shot->m_ParticleEffect, *(shot->GetFrame(0)->GetPosition()),
         shot->GetFrame(0)->GetPosition(), g_ParticleManager.PLASMA, LOD_BOTH, 20.0f, 500.0f);
        
         if(charged && charge)
            shot->m_ParticleEffect->m_StartColor = charge->m_StartColor;
		 else
		 {
			 switch(m_CurMainWeaponUpgrade)
			 {
				case MW_Level1:
					shot->m_ParticleEffect->m_StartColor.x = 0.0f;
					shot->m_ParticleEffect->m_StartColor.y = 0.0f;
					shot->m_ParticleEffect->m_StartColor.z = 1.0f;
					break;
		   		case MW_Level2:
					shot->m_ParticleEffect->m_StartColor.x = 1.0f;
					shot->m_ParticleEffect->m_StartColor.y = 0.0f;
					shot->m_ParticleEffect->m_StartColor.z = 0.0f;
					break;
				case MW_Level3:
					shot->m_ParticleEffect->m_StartColor.x = 0.0f;
					shot->m_ParticleEffect->m_StartColor.y = 1.0f;
					shot->m_ParticleEffect->m_StartColor.z = 0.0f;
					break;
			 }
		 }

      }  
   }
}

//////////////////////////////////////////////////////////////////////////
/// Fire the Ion Cannon
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::FireIonCannon()
{
   if(g_time >= (m_IonCoolDown + m_IonLastFired))
   {
      m_IonLastFired = g_time;  
      m_MainWeaponLastFired = g_time;
      m_CurTurretState = RECOIL;
      
      m_pIonFlare->Play3DAudio(30, 10000.0f, 120);
      CVector3f position;
	   GetPosition(&position, 0);
      CVector3f velocity(0.0f, 0.0f, 0.0f);
      m_pIonFlare->UpdateSound(position, velocity);

      CIonFlare * shot = g_ObjectFactory.CreateIonFlare();
      shot->SetUp(PLAYER_SHOT, this, 0, 0, 150.0f);

      // Find the position of the shot
      CObjFrame frame;
      FindShotPosition(&frame);
      shot->SetFrame(&frame, 0);
      
      CParticleSystem * effect = g_ParticleManager.ObtainSystem();

      // If obtained a particle effect then we use it
      if(effect)
      {	 
		    // Create an effect for the shot
		    g_ParticleManager.CreateSystem(effect, *(shot->GetFrame(0)->GetPosition()),
		    NULL, g_ParticleManager.FLASH, LOD_BOTH, 20.0f, 500.0f);
      }   
 
      // Get a particle effect for the shot if one available
      shot->m_ParticleEffect = g_ParticleManager.ObtainSystem();

      // If obtained a particle effect then we use it
      if(shot->m_ParticleEffect != NULL)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(shot->m_ParticleEffect, *(shot->GetFrame(0)->GetPosition()),
         shot->GetFrame(0)->GetPosition(), g_ParticleManager.IONFLARE_SHOT, LOD_BOTH, 20.0f, 500.0f);
      }
   }
}  

//////////////////////////////////////////////////////////////////////////
/// Fire the Missiles
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::FireMissiles(int NumMissiles)
{ 
   // Check to see if the weapon is ready to be fired
   if(g_time >= (m_MissileCoolDown + m_MissileLastFired))
   { 
      m_NumMissiles -= NumMissiles;

      if(m_NumMissiles < 0)
      {
         NumMissiles += m_NumMissiles;
         m_NumMissiles = 0;
      }
      
      m_MissileLastFired = g_time;
      
      for(int i = 0; i < NumMissiles; i++)
      {
         CMissile * shot = g_ObjectFactory.CreateMissile();
         m_ShotsFiredMissiles++;

         m_pMissilesFired->Play3DAudio(20, 10000.0f, 40);
         CVector3f position;
	      GetPosition(&position, 0);
         CVector3f velocity;
         velocity.x = 0.0f;
         velocity.y = 0.0f;
         velocity.z = 0.0f;
         m_pMissilesFired->UpdateSound(position, velocity);

         CObjFrame frame = MissilePodFrame;

         frame.MoveForward(-6.5f);

         switch(i)
         {        
            case 0:
            {
               frame.MoveRight(3.0f);
               frame.MoveUp(-0.75f);
            }          
            break;

            case 1:
            {
               frame.MoveRight(-3.0f);
               frame.MoveUp(-0.75f);
            }
            break;

            case 2:
            {
               frame.MoveRight(2.5f);
               frame.MoveUp(-0.5f);
            }          
            break;

            case 3:
            {
               frame.MoveRight(-2.5f);
               frame.MoveUp(-0.5f);
            }
            break;

            case 4:
            {
               frame.MoveRight(2.0f);
               frame.MoveUp(-0.75f);
            }          
            break;

            case 5:
            {
               frame.MoveRight(-2.0f);
               frame.MoveUp(-0.75f);
            }
            break;

            case 6:
            {
               frame.MoveRight(2.5f);
               frame.MoveUp(-1.0f);
            }          
            break;

            case 7:
            {
               frame.MoveRight(-2.5f);
               frame.MoveUp(-1.0f);
            }
            break;

            case 8:
            {
               frame.MoveRight(2.5f);
               frame.MoveUp(-0.75f);
            }          
            break;

            case 9:
            {
               frame.MoveRight(-2.5f);
               frame.MoveUp(-0.75f);
            }
            break;
         }
     
         //FindMissilePosition(&frame, offsetx, offsety);
         shot->SetFrame(&frame, 0);
         shot->SetUp(PLAYER_SHOT, (int)m_MissileDamage, i, m_MissileRadius, this);
      
         shot->m_FireTrail = g_ParticleManager.ObtainSystem();

         // If obtained a particle effect then we use it
         if(shot->m_FireTrail!= NULL)
         {
            // Create an effect for the shot
            g_ParticleManager.CreateSystem(shot->m_FireTrail, *(shot->GetFrame(0)->GetPosition()),
            shot->GetFrame(0)->GetPosition(), g_ParticleManager.MISSILE_TRAIL, LOD_FAR);
         }  

         shot->m_SmokeTrail = g_ParticleManager.ObtainSystem();

         // If obtained a particle effect then we use it
         if(shot->m_SmokeTrail != NULL)
         {
            // Create an effect for the shot
            g_ParticleManager.CreateSystem(shot->m_SmokeTrail, *(shot->GetFrame(0)->GetPosition()),
            shot->GetFrame(0)->GetPosition(), g_ParticleManager.MISSILE_TRAIL_SMOKE, LOD_FAR);
         }  
      }
   }
}

//////////////////////////////////////////////////////////////////////////
/// Fire the tracking Missiles
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::FireTrackingMissiles(int numissiles)
{ 
   // Check to see if the weapon is ready to be fired
   if(g_time >= (m_TrkMissileCoolDown + m_TrkMissileLastFired) && m_NumTrackingMissiles > 0)
   { 
      // Cycle through each missile pod
      m_CurrentMissileBay++;
      m_ShotsFiredTrkMissiles++;
      if(m_CurrentMissileBay >= m_ClusterSize)
         m_CurrentMissileBay = 0;

      // Reduce missile count
      m_NumTrackingMissiles -= 1;

      // Save time last fired
      m_TrkMissileLastFired = g_time;

      // Play sound of missile firing
      m_pMissilesFired->Play3DAudio(20, 10000.0f, 120);

      // Update that sound
      CVector3f position;
	   GetPosition(&position, 0);
      CVector3f velocity(0.0f, 0.0f, 0.0f);
      m_pMissilesFired->UpdateSound(position, velocity);

      // Fire the missile at the right place
      float offsetx, offsety;    
      int CurMissile = m_CurrentMissileBay;
      CTrackingMissile * shot = g_ObjectFactory.CreateTrackingMissile();
      
      // Find the position of the shot
      CObjFrame frame;

      switch(CurMissile)
      {
         case 0:
         {
            offsetx = -3.0f;
            offsety = 0.0f; 
				shot->SetFlyLeft(false);
         }          
         break;

         case 1:
         {
            offsetx = 3.0f;
            offsety = 0.0f;
				shot->SetFlyLeft(true);
         }
         break;

         case 2:
         {
            offsetx = -2.0f;
            offsety = 1.0f; 
				shot->SetFlyLeft(false);
         }          
         break;

         case 3:
         {
            offsetx = 2.0f;
            offsety = 1.0f;
				shot->SetFlyLeft(true);
         }
         break;

         case 4:
         {
            offsetx = -1.0f;
            offsety = 0.0f; 
				shot->SetFlyLeft(false);
         }          
         break;

         case 5:
         {
            offsetx = 1.0f;
            offsety = 0.0f;
				shot->SetFlyLeft(true);
         }
         break;

         case 6:
         {
            offsetx = -2.0f;
            offsety = -1.0f; 
				shot->SetFlyLeft(false);
         }          
         break;

         case 7:
         {
            offsetx = 2.0f;
            offsety = -1.0f;
				shot->SetFlyLeft(true);
         }
         break;

         case 8:
         {
            offsetx = -1.5f;
            offsety = 0.0f; 
				shot->SetFlyLeft(false);
         }          
         break;

         case 9:
         {
            offsetx = 1.5f;
            offsety = 0.0f;
				shot->SetFlyLeft(true);
         }
         break;
      }
  
      FindMissilePosition(&frame, offsetx, offsety);
      shot->SetFrame(&frame, 0);

      shot->SetUp(PLAYER_SHOT, (int)m_MissileDamage, false, m_TrkMissileRadius, m_TrkMissileAccuracy, this);
      shot->SetTarget(m_pTarget);
   
      shot->m_FireTrail = g_ParticleManager.ObtainSystem();

      // If obtained a particle effect then we use it
      if(shot->m_FireTrail != NULL)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(shot->m_FireTrail, *(shot->GetFrame(0)->GetPosition()),
         shot->GetFrame(0)->GetPosition(), g_ParticleManager.TRKMISSILE_TRAIL, LOD_FAR);
      }  

      shot->m_SmokeTrail = g_ParticleManager.ObtainSystem();

      // If obtained a particle effect then we use it
      if(shot->m_SmokeTrail != NULL)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(shot->m_SmokeTrail, *(shot->GetFrame(0)->GetPosition()),
         shot->GetFrame(0)->GetPosition(), g_ParticleManager.TRKMISSILE_TRAIL_SMOKE, LOD_FAR);
      }  
   }
}

//////////////////////////////////////////////////////////////////////////
/// Find the point where a shot will come out
/// 
/// Input:     CObjFrame * frame - frame to copy to shot
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::FindShotPosition(CObjFrame * frame)
{
   CQuaternion qTank, qTurret, qShot;
   CVector3f tankPos, turretPos, turretAt, shotPos;

   m_RenderObj.GetFrame(0)->GetOrientation(&qTank);
   m_RenderObj.GetFrame(1)->GetOrientation(&qTurret);
   QuatCrossProduct(&qShot, &qTank, &qTurret);

   m_RenderObj.GetPosition(&tankPos, 0);
   m_RenderObj.GetPosition(&turretPos, 1);
   m_RenderObj.GetFrame(0)->RotateToWorldVector(&turretPos, &turretPos);

   m_RenderObj.GetFrame(1)->GetForward(&turretAt);
   Vec3fScale(&turretAt, &turretAt, 5.5f);
   m_RenderObj.GetFrame(0)->RotateToWorldVector(&turretAt, &turretAt);

   Vec3fAdd(&shotPos, &tankPos, &turretPos);
   Vec3fAdd(&shotPos, &shotPos, &turretAt);
   
   frame->SetOrientation(&qShot);
   frame->SetPosition(&shotPos); 
}

//////////////////////////////////////////////////////////////////////////
/// Find the point where a shot will come out
/// 
/// Input:     CObjFrame * frame - frame to copy to shot
//             float offsetx 
//             float offsety
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::FindMissilePosition(CObjFrame * frame, float offsetx, float offsety)
{
   CQuaternion qTank, qTurret, qShot;
   CVector3f tankPos, turretPos, turretAt, shotPos;

   CVector3f right, up;
   m_RenderObj.GetFrame(0)->GetRight(&right);
   m_RenderObj.GetFrame(0)->GetUp(&up);

   m_RenderObj.GetFrame(0)->GetOrientation(&qTank);
   m_RenderObj.GetFrame(1)->GetOrientation(&qTurret);
   QuatCrossProduct(&qShot, &qTank, &qTurret);

   m_RenderObj.GetPosition(&tankPos, 0);
   m_RenderObj.GetPosition(&turretPos, 1);
   m_RenderObj.GetFrame(0)->RotateToWorldVector(&turretPos, &turretPos);

   m_RenderObj.GetFrame(1)->GetForward(&turretAt);
   Vec3fScale(&turretAt, &turretAt, -3.5f);
   m_RenderObj.GetFrame(0)->RotateToWorldVector(&turretAt, &turretAt);

   Vec3fScale(&right, &right, offsetx);
   Vec3fScale(&up, &up, offsety);
   Vec3fAdd(&shotPos, &tankPos, &turretPos);
   Vec3fAdd(&shotPos, &shotPos, &turretAt);
   Vec3fAdd(&shotPos, &shotPos, &right);
   Vec3fAdd(&shotPos, &shotPos, &up);
   
   frame->SetOrientation(&qShot);
   frame->SetPosition(&shotPos);  
}

void CPlayer::UpdateShield(void)
{
   static float ShieldDisplayTime = 1.0f;
     
   if (!m_ShowShield)
      return;

   // if the shield has been displayed for the time then stop displaying it
   if (g_time > m_StartShieldTime + ShieldDisplayTime)
   {
      //m_StartShieldTime = g_time;
      m_ShowShield = false;
      m_pShieldModel->RemoveFromQuadtree();
      m_pShieldMissilePods->RemoveFromQuadtree();

      //m_pShieldModel->m_Material.m_Diffuse.a = 1.0f;
      //m_pShieldModel->m_Material.m_Diffuse.a = 1.0f;
   }
   // otherwise update the shield
   else
   {
      // update the shield position

      // update the shield tank base
      m_pShieldModel->SetFrame(m_RenderObj.GetFrame(0), 0);

      m_pShieldModel->SetFrame(m_RenderObj.GetFrame(1), 1);

      m_pShieldModel->Update();
      
      m_pShieldMissilePods->SetFrame(m_pMissilePods->GetFrame(0), 0);

      m_pShieldMissilePods->Update();
      // update the shield tank turret
      /*
      m_pShieldModel->GetFrame(1)->Reset();
      m_pShieldModel->SetPosition(&m_TurretShieldPosition, 1);
      m_pShieldModel->RotateLocalX(m_TurretAngle, 1);
      m_pShieldModel->MoveForward(m_turretpos * 1.2f, 1);

      // update the missile pods
      CObjFrame ShieldPodFrame;
      FindShotPosition(&ShieldPodFrame);

      m_pShieldMissilePods->SetFrame(m_RenderObj.GetFrame(0), 0);
      m_pShieldMissilePods->MoveUp(m_ShieldPodsPosition.y, 0);
      m_pShieldMissilePods->MoveForward(m_ShieldPodsPosition.z, 0);

      CQuaternion MissilePodQuat;
      ShieldPodFrame.GetOrientation(&MissilePodQuat);
      m_pShieldMissilePods->SetOrientation(&MissilePodQuat, 0); 
      */

      // update shield fade
      m_pShieldModel->m_Material.m_Diffuse.a = (ShieldDisplayTime +
                                               (m_StartShieldTime - g_time)) * .3f;

      m_pShieldModel->m_Material.m_Ambient.a = (ShieldDisplayTime +
                                               (m_StartShieldTime - g_time)) * .3f;
                                               
      // update shield fade
      m_pShieldMissilePods->m_Material.m_Diffuse.a = (ShieldDisplayTime +
                                               (m_StartShieldTime - g_time)) * .3f;

      m_pShieldMissilePods->m_Material.m_Ambient.a = (ShieldDisplayTime +
                                               (m_StartShieldTime - g_time)) * .3f;   
   }   
}

//////////////////////////////////////////////////////////////////////////
/// Update the location and status of the turret
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::UpdateTurretState(void)
{
   static float velocity = 8.0f;    // Speed of recoil
   static float maxrecoil = -.8f;  // Max/min distance of recoil
   static bool forward = false;     // Direction moving

   switch(m_CurTurretState)
   {
      case STANDBY:
      {

      }
      break;

      case RECOIL:
      {
         if (forward)
            m_turretpos += (velocity * g_FrameTime);
         else
            m_turretpos -= (velocity * g_FrameTime);
         
         if (m_turretpos <= maxrecoil)
         {
            forward = true;
            m_turretpos = maxrecoil;
         }
         
         else if (m_turretpos > 0.0f)
         {
            m_CurTurretState = STANDBY;
            forward = false;
            m_RenderObj.SetPosition(&m_TurretPosition, 1);
            
            //m_pShieldModel->SetPosition(&m_TurretShieldPosition, 1);
            
            m_turretpos = 0.0f;
            break;
         }

         //m_pShieldModel->SetPosition(&m_TurretShieldPosition, 1);
         //m_pShieldModel->MoveForward(m_turretpos * 1.2f, 1);

         m_RenderObj.SetPosition(&m_TurretPosition, 1);
         m_RenderObj.MoveForward(m_turretpos, 1);
      }
      break;
   }
}

//////////////////////////////////////////////////////////////////////////
/// Get the input of the player
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::GetPlayerInput(void)
{
   static bool keydown = false;  // Temp used for testing keys held down
   static bool charged = false;  // Main weapon charging?
   
   // Get the movement of the mouse this frame
   CVector3f MouseMove(0.0f, 0.0f, 0.0f);
   CInputManager::GetInstance()->PollMouseMove(MouseMove.x, MouseMove.y, MouseMove.z);

   // Cycle current secondary weapon forward
   if (MouseMove.z < 0)
   {
      m_CurSecWeapon = m_CurSecWeapon++;
      if(!(FSOUND_IsPlaying(m_pWeaponChange->m_Channel)))
         m_pWeaponChange->Play2DAudio(50);
      
      if(m_CurSecWeapon >= NUM_SEC_WEAPONS)
      {
         m_CurSecWeapon = NUM_SEC_WEAPONS - 1;
      }
   }
   
   // Cycle current secondary weapon backward
   else if (MouseMove.z > 0)
   {
      m_CurSecWeapon = m_CurSecWeapon--;
      if(!(FSOUND_IsPlaying(m_pWeaponChange->m_Channel)))
         m_pWeaponChange->Play2DAudio(50);
      
      if(m_CurSecWeapon < 0)
      {
         m_CurSecWeapon = 0;
      }
   }

   // Current secondary weapon is missiles
   if(CInputManager::GetInstance()->GetKey(DIK_1))
   {
      m_CurSecWeapon = 0;
      if(!(FSOUND_IsPlaying(m_pWeaponChange->m_Channel)))
         m_pWeaponChange->Play2DAudio(50);
   }

   // Current secondary weapon is trkmissiles
   if(CInputManager::GetInstance()->GetKey(DIK_2))
   {
      m_CurSecWeapon = 1;
      if(!(FSOUND_IsPlaying(m_pWeaponChange->m_Channel)))
         m_pWeaponChange->Play2DAudio(50);
   }

   // Current secondary weapon is ion cannon
   if(CInputManager::GetInstance()->GetKey(DIK_3))
   {
      m_CurSecWeapon = 2;
      if(!(FSOUND_IsPlaying(m_pWeaponChange->m_Channel)))
         m_pWeaponChange->Play2DAudio(50);;
   }

   // Prepare to fire main weapon
   if(CInputManager::GetInstance()->PollMouseClick() &&m_CurTurretState == STANDBY)
   {
      CObjFrame frame;
      FindShotPosition(&frame);
      static float timefirstpressed = 0.0f;
      
      m_ChargeDamageStep = (m_MaxMainWeaponDamage - m_MinMainWeaponDamage) / m_MaxChargeTime;
      m_ChargeEnergyStep = (m_MaxEnergyUsed - m_MinEnergyUsed) / m_MaxChargeTime;
      m_ChargeRadiusStep = (m_MaxMainWeaponRadius - m_MinMainWeaponRadius) / m_MaxChargeTime;
      
      // See if this is the first frame input has been detected
      if(keydown == false)
      {
         keydown = true;
         timefirstpressed = g_time;
      }
      
      // Main Weapon is inactive 
      if(charged == false && ((timefirstpressed + 0.25f) < g_time))
      {
         timefirstpressed = 0.0f;

         m_pChargeRampUp->Play3DAudio(20.0f, 10000.0f, 120);
         CVector3f position;
	      GetPosition(&position, 0);
         CVector3f velocity;
         velocity.x = 0.0f;
         velocity.y = 0.0f;
         velocity.z = 0.0f;
         m_pChargeRampUp->UpdateSound(position, velocity);
         
         charge = g_ParticleManager.ObtainSystem();

         if(charge)
         {
            g_ParticleManager.ClearSystem(charge);
			   switch(m_CurMainWeaponUpgrade)
			   {
				   case MW_Level1:
					   g_ParticleManager.CreateSystem(charge, *frame.GetPosition(), NULL, g_ParticleManager.CHARGELVL1, LOD_BOTH, 20.0f, 500.0f); 
					   break;
				   case MW_Level2:
					   g_ParticleManager.CreateSystem(charge, *frame.GetPosition(), NULL, g_ParticleManager.CHARGELVL2, LOD_BOTH, 20.0f, 500.0f); 
					   break;
				   case MW_Level3:
					   g_ParticleManager.CreateSystem(charge, *frame.GetPosition(), NULL, g_ParticleManager.CHARGELVL3, LOD_BOTH, 20.0f, 500.0f); 
					   break;
				   default:
				       bool debug = true;
				       break;
			   }
            charge->SetLife(0.0f);
         }
         m_CurMainWeaponDamage = m_MinMainWeaponDamage; 
         m_CurEnergyUsed = m_MinEnergyUsed;
         m_CurMainWeaponRadius = m_MinMainWeaponRadius;
         charged = true;
      }

      // Main weapon is currently charging
      if(charged)
      {  
         CVector3f updatepos = *frame.GetPosition();
         if(charge)
         {
            charge->SetLocation(updatepos.x, updatepos.y, updatepos.z);
            charge->SetParticlesPerSec(charge->GetParticlesPerSec() + 1);

			   if(m_CurMainWeaponUpgrade == MW_Level1)
			   {
				   charge->m_StartColor.y += (m_ChargeColorStep * g_FrameTime);
				   if(charge->m_StartColor.y > 0.5f)
					   charge->m_StartColor.y = 0.5f;
			   }
			   else if(m_CurMainWeaponUpgrade == MW_Level2)
			   {
				   charge->m_StartColor.y += (m_ChargeColorStep * g_FrameTime);
				   if(charge->m_StartColor.y > 0.5f)
					   charge->m_StartColor.y = 0.5f;
			   }
		      else if(m_CurMainWeaponUpgrade == MW_Level3)
			   {
				   charge->m_StartColor.x += (m_ChargeColorStep * g_FrameTime);
				   if(charge->m_StartColor.x > 0.5f)
					   charge->m_StartColor.x = 0.5f;
			   }

         }

         m_CurMainWeaponDamage += m_ChargeDamageStep * g_FrameTime;
         m_CurMainWeaponRadius += m_ChargeRadiusStep * g_FrameTime;
      
         if(m_CurMainWeaponDamage >= m_MaxMainWeaponDamage)
            m_CurMainWeaponDamage = m_MaxMainWeaponDamage;
         if(m_CurMainWeaponRadius >= m_MaxMainWeaponRadius)
            m_CurMainWeaponRadius = m_MaxMainWeaponRadius;

         if(charge)
         {
            int maxparticles = 300;

            static float spreadinc = 0.0f;
            spreadinc += m_SpreadStep * g_FrameTime;
         
            if(charge->GetParticlesPerSec() >= (unsigned int)maxparticles)
               charge->SetParticlesPerSec(maxparticles);

            charge->SetSpread(1.0f, charge->m_SpreadMin.x += -spreadinc, charge->m_SpreadMax.x += spreadinc, charge->m_SpreadMin.y += -spreadinc, charge->m_SpreadMax.y += spreadinc,
               charge->m_SpreadMin.z += -spreadinc,  charge->m_SpreadMax.z += spreadinc);

            // Cap the spread values
            float maxspread = 0.50f;
            if(charge->m_SpreadMin.x <= -maxspread)
               charge->m_SpreadMin.x = -maxspread;
            if(charge->m_SpreadMin.y <= -maxspread)
               charge->m_SpreadMin.y = -maxspread;
            if(charge->m_SpreadMin.z <= -maxspread)
               charge->m_SpreadMin.z = -maxspread;
            if(charge->m_SpreadMax.x >= maxspread)
               charge->m_SpreadMax.x = maxspread;
            if(charge->m_SpreadMax.y >= maxspread)
               charge->m_SpreadMax.y = maxspread;
            if(charge->m_SpreadMax.z >= maxspread)
               charge->m_SpreadMax.z = maxspread;
         }

         if(!(FSOUND_IsPlaying(m_pChargeFull->m_Channel)) && !(FSOUND_IsPlaying(m_pChargeRampUp->m_Channel)))
            m_pChargeFull->Play3DAudio(20.0f, 10000.0f, 120);
         CVector3f position;
	      GetPosition(&position, 0);
         CVector3f velocity;
         velocity.x = 0.0f;
         velocity.y = 0.0f;
         velocity.z = 0.0f;
         m_pChargeFull->UpdateSound(position, velocity);
      }

   }
   
   // Left Mouse has been released after a charge = fire main weapon
   else if(!CInputManager::GetInstance()->PollMouseClick() && keydown == true && charged == true) 
   {
      keydown = false;
      charged = false;
      if(charge)
         charge->m_UpdateFlag = ONLY_UPDATE;
      FireMainWeapon(true);
      m_pChargeFull->StopAudio();
   }

   // Left mouse has been released after no charge = fire main weapon
   else if(!CInputManager::GetInstance()->PollMouseClick() && keydown == true) 
   {
      keydown = false;
      m_CurMainWeaponDamage = m_MinMainWeaponDamage;
      m_CurEnergyUsed = m_MinEnergyUsed;
      m_CurMainWeaponRadius = m_MinMainWeaponRadius;
      FireMainWeapon(false);
   }

   // Fire current secondary weapon
   if(CInputManager::GetInstance()->PollMouseRClick())
   {
      switch(m_CurSecWeapon)
      {
         case SEC_MISSILE:
            FireMissiles(m_ClusterSize);
            break;
         case SEC_TRK_MISSILE:
            FireTrackingMissiles(m_ClusterSize);
            break;
         case SEC_IONCANNON:
            FireIonCannon();
            break;
      }
   }

   float tempelevation = MouseMove.y * 0.0012f;
	g_PlayerCamera.TurnUp(-tempelevation);
	g_PlayerCamera.TurnRight(MouseMove.x * 0.0012f); 

   if (CInputManager::GetInstance()->GetKey(DIK_W))
   {
		RBMoveForward();
   }

   if (CInputManager::GetInstance()->GetKey(DIK_S))
   {
		RBMoveBackward();
   }

   if (CInputManager::GetInstance()->GetKey(DIK_A))
   {
		RBMoveLeft();
   }

   if (CInputManager::GetInstance()->GetKey(DIK_D))
   {
		RBMoveRight();
   }

   if (CInputManager::GetInstance()->GetKey(DIK_SPACE) && m_Energy > 0.0f)
   {
      m_Energy -= g_FrameTime * m_JumpJetEnergyPerSec;

      if(m_Energy < 0.0f)
         m_Energy = 0.0f;
      
		RBMoveUp();

      if(m_InAir == false)
      {
         m_pJumpJet->Play3DAudio(40.0f, 10000.0f, 180);
         m_InAir = true;
      }

   }
   else if(!CInputManager::GetInstance()->GetKey(DIK_SPACE))
   {
      m_pJumpJet->StopAudio();
      m_InAir = false;
   }

   if(m_Energy <= 0.0f && m_InAir == true)
       m_pJumpJet->StopAudio();


   if (CInputManager::GetInstance()->GetKey(DIK_LSHIFT) && m_Energy > 0.0f)
   {
      m_Energy -= g_FrameTime * m_BoostEnergyPerSec;

      if(m_Energy < 0.0f)
         m_Energy = 0.0f;

      if(m_InBoost == false)
      {
         //m_pJumpJet->Play3DAudio(40.0f, 10000.0f, 140);
         m_InBoost = true;
         m_RigidBody->SetParameters(m_MaxBoostVelocity, m_BoostAcceleration);
      }

   }
   else if(CInputManager::GetInstance()->GetKey(DIK_LSHIFT) && m_Energy <= 0.0f && m_InBoost == true)
   {
      //m_pJumpJet->StopAudio();
      m_RigidBody->SetParameters(m_MaxVelocity, m_AccelerationTime);
   }
   else if(!CInputManager::GetInstance()->GetKey(DIK_LSHIFT))
   {
     // m_pJumpJet->StopAudio();
      m_InBoost = false;
      m_CurBoostVelocity -= m_BoostAcceleration * g_FrameTime;
      if(m_CurBoostVelocity <= m_MaxVelocity)
         m_CurBoostVelocity = m_MaxVelocity;
      m_RigidBody->SetParameters(m_MaxVelocity, m_AccelerationTime);
   }
}

// Attempts to update orientation of tank and turret to match camera aim
// Player camera only
void CPlayer::MatchCameraOrientation(void)
{
	CVector3f AimPoint, CamPos, proj;

	// Obtain aim point
	g_PlayerCamera.GetLookAt(&AimPoint);
	g_PlayerCamera.GetPosition(&CamPos);
	Vec3fScale(&AimPoint, &AimPoint, g_PlayerCamera.GetDirScale());
	Vec3fAdd(&AimPoint, &AimPoint, &CamPos);

	// Rotate aim point into local space
	m_RenderObj.GetFrame(0)->RotateToLocalPoint(&AimPoint, &AimPoint);

	// See if we need to rotate local Y
	proj = AimPoint;

	// Find angle to rotate
	Vec3fNormalize(&proj, &proj);
	float SineAlpha = proj.y;
	proj.y = 0.0f;
	Vec3fNormalize(&proj, &proj);
	float theta = (float)acos(proj.z);

	if (theta > 0.0f)
	{
		// Find direction to rotate
		CVector3f Z(0.0f, 0.0f, 1.0f), cross;
		Vec3fCrossProduct(&cross, &Z, &proj);
		if (cross.y > 0.0f)
			RotateLocalY(theta, 0);
		else if (cross.y < 0.0f)
			RotateLocalY(-theta, 0);
	}

	// Rotate turret
	float alpha = (float)asin(SineAlpha);
	if (alpha > m_MaxTurretElevation)
		alpha = m_MaxTurretElevation;
	else if (alpha < m_MinTurretElevation)
		alpha = m_MinTurretElevation;

	float angle = alpha - m_CurElevation;
	m_CurElevation = alpha;
	m_RenderObj.GetFrame(1)->RotateLocalX(-angle);

   m_TurretAngle -= angle;
   //m_pShieldModel->RotateLocalX(-angle, 1);
}

//////////////////////////////////////////////////////////////////////////
/// Update the player
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::Update(void)
{
   // Update the physics
	m_RigidBody->StepIntegrate(g_FrameTime);
	MatchCameraOrientation();	

   if(!(FSOUND_IsPlaying(m_pEngine->m_Channel)))
      m_pEngine->Play3DAudio(20, 10000.0f, 0);

   GetPosition(&m_CurPosition, 0);
   CVector3f Velocity;
   Velocity.x = (m_CurPosition.x - m_LastPosition.x) / g_FrameTime;
   Velocity.y = (m_CurPosition.y - m_LastPosition.y) / g_FrameTime;
   Velocity.z = (m_CurPosition.z - m_LastPosition.z) / g_FrameTime;
   m_pEngine->UpdateSound(m_CurPosition, Velocity);
   m_pJumpJet->UpdateSound(m_CurPosition, Velocity);
   m_LastPosition = m_CurPosition;

   float mag = Vec3fMagnitude(&Velocity);
   m_pEngine->AdjustVolume((unsigned int)abs(mag * 2));

   if(m_CurPlayerState == SLIGHTLY_DAMAGED && m_Shield == m_MaxShield)
   {
      if(smoke)
          smoke->m_UpdateFlag = ONLY_UPDATE;
      m_CurPlayerState = FINE;
   }
   else if(m_CurPlayerState == HEAVILY_DAMAGED && m_Shield == m_MaxShield)
   {
      if(fire)
         fire->m_UpdateFlag = ONLY_UPDATE;
      if(smoke)
         smoke->m_UpdateFlag = ONLY_UPDATE;
      m_CurPlayerState = FINE;
   }
 
   // Locate the thruster positions
   FindThrusterPositions();

   // Update the state of the turret
   UpdateTurretState();
   
   // Regenerate the shields
   ShieldRegen();

   if(m_InAir == false && m_InBoost == false)
      EnergyRegen();

   m_IonCannonCountDown = (float)GetIonTimer();

   if (m_CurSecWeapon == SEC_TRK_MISSILE)
   {
      // update the target
      CGameObject* pCurr = GameMain.m_ObjList;
      CGameObject* pClosest = 0;
      float BestDistSq;
   
      // find camera's shot ray
      Ray ShotRay;
      g_Camera->GetPosition(&ShotRay.Origin);
      g_Camera->GetLookAt(&ShotRay.Direction);

      Point3f PlayerPos;
      m_RenderObj.GetPosition(&PlayerPos, 0);

      // loop through all game objects
      CVector3f EnemyPos;
      float EnemyDistSq;
      Sphere BoundSphere;
      while (pCurr)
      {
         // if it's an enemy
         if (pCurr->m_RadarType == RADAR_ENEMY)
         {
            BoundSphere = *pCurr->GetSphere();
            BoundSphere.Radius += 3.0f;

            // check for collision with our shot ray
            if (RayToSphere(&ShotRay, &BoundSphere))
            {
               // check to see if this is a better dist
               if (pClosest)
               {
                  pCurr->GetPosition(&EnemyPos, 0);

                  EnemyDistSq = powf(EnemyPos.x - PlayerPos.x, 2) +
                               powf(EnemyPos.y - PlayerPos.y, 2) +
                               powf(EnemyPos.z - PlayerPos.z, 2);         
            
                  if (EnemyDistSq < BestDistSq && EnemyDistSq <= 150000.0f)
                  {
                     BestDistSq = EnemyDistSq;
                     pClosest = pCurr;
                  }
               }
               else
               {
                  
                  pCurr->GetPosition(&EnemyPos, 0);

                  EnemyDistSq = powf(EnemyPos.x - PlayerPos.x, 2) +
                               powf(EnemyPos.y - PlayerPos.y, 2) +
                               powf(EnemyPos.z - PlayerPos.z, 2); 
                  
                  if(EnemyDistSq <= 150000.0f)
                  {
                     BestDistSq = EnemyDistSq;
                     pClosest = pCurr;
                  }
               }
            }
         }

         pCurr = pCurr->m_Next;
      }

      if(pClosest != m_pTarget && pClosest != NULL)
      {
         m_pLockOn->Play2DAudio(2000);   
      }

      m_pTarget = pClosest;
   }
   else
      m_pTarget = 0;

   // update the missile pods
   FindShotPosition(&MissilePodFrame);

   CObjFrame PlayerFrame = *m_RenderObj.GetFrame(0);

   m_pMissilePods->SetFrame(&PlayerFrame, 0);
   m_pMissilePods->MoveUp(m_MissilePodsPosition.y, 0);
   m_pMissilePods->MoveForward(m_MissilePodsPosition.z, 0);

   CQuaternion MissilePodQuat;
   MissilePodFrame.GetOrientation(&MissilePodQuat);
   m_pMissilePods->SetOrientation(&MissilePodQuat, 0);

   m_pMissilePods->Update();

   // update the shields
   UpdateShield();

   CGameObject::Update();
}

//////////////////////////////////////////////////////////////////////////
/// What to do when a collision is detected with another game object
/// 
/// Input:     CGameObject *obj - The object that got collided with
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::Collide(const CGameObject *obj)
{
   // Object is the terrain 
   if(!obj)
   {
      // DO TERRAIN FOLLOWING
   }
   
   // Object is a game object
   else
   {
      if(obj->m_RadarType == RADAR_ENEMY || obj->m_RadarType == RADAR_FRIENDLY || obj->m_RadarType == RADAR_ROCK || obj->m_RadarType == RADAR_BUILDING)
      {
         if(!(FSOUND_IsPlaying(m_pCollide->m_Channel)))
         {
            m_pCollide->Play3DAudio(20.0f, 10000.0f, 60);   
            CVector3f position;
            GetPosition(&position, 0);
            CVector3f velocity(0.0f, 0.0f, 0.0f);
            m_pCollide->UpdateSound(position, velocity); 
         }
      }

      CVector2f loc;
      loc.x = 0.05f;
      loc.y = 0.023f;
      Color4f color = {1.0f, 1.0f, 1.0f, 1.0f};

      switch(obj->GetType())
      {
        case OBJ_SHDPWRUP:
         {
            CShieldPowerUp * shldpowerup = (CShieldPowerUp*)obj;
            m_Shield += shldpowerup->GetAmountRestored();
            if(m_Shield > m_MaxShield)
            {
               m_Shield = m_MaxShield;
            }
            g_HudTextManager.CreateHudMessage("Shields Restored", loc, color, 2);
         }  
         break;

         case OBJ_MSLPACK:
         {
            CMissilePack * missilepack = (CMissilePack*)obj;
            m_NumMissiles += missilepack->GetNumMissiles();
            g_HudTextManager.CreateHudMessage("+20 Cluster Missiles", loc, color, 2);
         }  
         break;

         case OBJ_TRKMSLPACK:
         {
            CTrackingMissilePack * trkmissilepack = (CTrackingMissilePack*)obj;
            m_NumTrackingMissiles += trkmissilepack->GetNumMissiles();
            g_HudTextManager.CreateHudMessage("+20 Tracking Missiles", loc, color, 2);
         }  
         break;

         case OBJ_PLASMAPWRUP:
         {
            switch(m_CurMainWeaponUpgrade)
            {
               case MW_Level1:
                  m_MinMainWeaponDamage += 10.0f;
                  m_MaxMainWeaponDamage += 30.0f;
                  m_PlasmaShotSpeed += 20.0f;
                  m_MaxChargeTime -= 0.3f;
                  m_CurMainWeaponUpgrade = MW_Level2;
                  g_HudTextManager.CreateHudMessage("Plasma Shot Level2", loc, color, 2);
                  break;
               case MW_Level2:
                  m_MinMainWeaponDamage += 10.0f;
                  m_MaxMainWeaponDamage += 30.0f;
                  m_PlasmaShotSpeed += 20.0f;
                  m_MaxChargeTime -= 0.2f;
                  m_CurMainWeaponUpgrade = MW_Level3;
                  g_HudTextManager.CreateHudMessage("Plasma Shot Level3", loc, color, 2);
                  break;
               case MW_Level3:
                  g_HudTextManager.CreateHudMessage("Plasma Shot Level3", loc, color, 2);
                     // Don't do anything
                  break;
            }        
         }  
         break;

         case OBJ_MSLPWRUP:
         {
            switch(m_CurMissileUpgrade)
            {
               case MS_Level1:
                  m_MissileDamage += 20;
                  m_MissileSpeed += 20;
                  m_ClusterSize = 8;
                  m_CurMissileUpgrade = MS_Level2;
                  g_HudTextManager.CreateHudMessage("Cluster Missiles Level2", loc, color, 2);
                  break;
               case MS_Level2:
                  m_MissileDamage += 20;
                  m_MissileSpeed += 20;
                  m_ClusterSize = 10;
                  m_CurMissileUpgrade = MS_Level3;
                  g_HudTextManager.CreateHudMessage("Cluster Missiles Level3", loc, color, 2);
                  break;
               case MS_Level3:
                  g_HudTextManager.CreateHudMessage("Cluster Missiles Level3", loc, color, 2);
                  // Don't do anything
                  break;
            }        
         }  
         break;

         case OBJ_TRKMSLPWRUP:
         {
            switch(m_CurTrkMissileUpgrade)
            {
               case TMS_Level1:
                  m_TrkMissileDamage += 20;
                  m_TrkMissileSpeed += 20;
                  m_TrkMissileAccuracy = 0.4f;
                  m_CurTrkMissileUpgrade = TMS_Level2;
                  g_HudTextManager.CreateHudMessage("Tracking Missiles Level2", loc, color, 2);
                  break;
               case TMS_Level2:
                  m_TrkMissileDamage += 20;
                  m_TrkMissileSpeed += 20;
                  m_TrkMissileAccuracy = 0.2f;
                  m_CurTrkMissileUpgrade = TMS_Level3;
                  g_HudTextManager.CreateHudMessage("Tracking Missiles Level3", loc, color, 2);
                  break;
               case TMS_Level3:
                     // Don't do anything
                  g_HudTextManager.CreateHudMessage("Tracking Missiles Level3", loc, color, 2);
                  break;
            }        
         }  
         break;
      } 
   }    
}

//////////////////////////////////////////////////////////////////////////
/// What to do when the player dies
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::Die(void)
{ 
   // Setup player explosion
   explode = g_ParticleManager.ObtainSystem();
   if(explode)
      g_ParticleManager.CreateSystem(explode, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.AURA, LOD_NONE);
   
   // Kill all particle effects
   if(thruster1)
      g_ParticleManager.DestroySystem(thruster1);
   if(thruster2)
      g_ParticleManager.DestroySystem(thruster2);
   if(fire)
      g_ParticleManager.DestroySystem(fire);
   if(smoke)
      g_ParticleManager.DestroySystem(smoke);

   m_Active = false;	
   m_CurPlayerState = DEAD;
   GameMain.m_CurrentGameState = GameMain.MISSION_LOST;
   RemoveFromQuadtree();

   m_pShieldModel->RemoveFromQuadtree();
   m_pShieldMissilePods->RemoveFromQuadtree();
   m_pMissilePods->RemoveFromQuadtree();

   delete m_pMissilePods;
   delete m_pShieldMissilePods;
   delete m_pShieldModel;

   m_pMissilePods = 0;
   m_pShieldMissilePods = 0;
   m_pShieldModel = 0;
}

//////////////////////////////////////////////////////////////////////////
/// Reset the players attributes so that the player can respawn
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::Reset(void)
{
   // Kill all particle effects
   if(thruster1)
      g_ParticleManager.DestroySystem(thruster1);
   if(thruster2)
      g_ParticleManager.DestroySystem(thruster2);
   if(fire)
      g_ParticleManager.DestroySystem(fire);
   if(smoke)
      g_ParticleManager.DestroySystem(smoke);

   m_Active = false;	
   m_CurPlayerState = DEAD;
   RemoveFromQuadtree();

   if (m_pShieldModel)
      m_pShieldModel->RemoveFromQuadtree();
   
   if (m_pShieldMissilePods)
      m_pShieldMissilePods->RemoveFromQuadtree();
   
   if (m_pMissilePods)
      m_pMissilePods->RemoveFromQuadtree();

   delete m_pMissilePods;
   delete m_pShieldMissilePods;
   delete m_pShieldModel;
}

//////////////////////////////////////////////////////////////////////////
/// Player has taken damage 
/// 
/// Input:     int damage - the amount of damage taken
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::TakeDamage(float damage, CGameObject* guywhohitme)
{
   if(GameMain.m_GodMode == false)
   {
      // First take damage from the shields
      m_Shield -= damage;

      if(m_Shield > 0)
      {
         m_pShieldAbsorb->Play3DAudio(25.0f, 10000.0f, 128);
         CVector3f position;
	      GetPosition(&position, 0);
         CVector3f velocity;
         velocity.x = 0.0f;
         velocity.y = 0.0f;
         velocity.z = 0.0f;
         m_pShieldAbsorb->UpdateSound(position, velocity);

         if (!m_ShowShield)
         {
            m_ShowShield = true;
            m_StartShieldTime = g_time;
            UpdateShield();
         }

         m_StartShieldTime = g_time;
      }

      // See if shield has fallen
      else if(m_Shield < 0)
      {
         float overflow = fabs(m_Shield);

         m_pHullHit->Play3DAudio(25.0f, 10000.0f, 128);
         CVector3f position;
	      GetPosition(&position, 0);
         CVector3f velocity;
         velocity.x = 0.0f;
         velocity.y = 0.0f;
         velocity.z = 0.0f;
         m_pHullHit->UpdateSound(position, velocity);

         if(m_CurPlayerState == FINE)
         {
            // Get a particle effect for the shot if one available
            smoke = g_ParticleManager.ObtainSystem();

            // If obtained a particle effect then we use it
            if(smoke != NULL)
            {
               // Create an effect for the shot
               g_ParticleManager.CreateSystem(smoke, *(GetFrame(0)->GetPosition()),
               GetFrame(0)->GetPosition(), g_ParticleManager.SMOKE, LOD_FAR);
            } 

            m_CurPlayerState = SLIGHTLY_DAMAGED;

            // set the shield's display to true

         }

         // Take damage off the hull that the shield didn't absorb
         m_Hull -= overflow;

         if(m_Hull <= (m_MaxHull * .5f) && m_CurPlayerState == SLIGHTLY_DAMAGED)
         {
            // Get a particle effect for the shot if one available
            fire = g_ParticleManager.ObtainSystem();

            // If obtained a particle effect then we use it
            if(fire != NULL)
            {
               // Create an effect for the shot
               g_ParticleManager.CreateSystem(fire, *(GetFrame(0)->GetPosition()),
               GetFrame(0)->GetPosition(), g_ParticleManager.TIMED, LOD_FAR);
            }  

            m_CurPlayerState = HEAVILY_DAMAGED;
         }

         // If hull is gone the player dies
         if(m_Hull <= 0)
         {
            m_pPlayerExplode->Play3DAudio(40.0f, 10000.0f, 200);
            CVector3f position;
	         GetPosition(&position, 0);
            CVector3f velocity;
            velocity.x = 0.0f;
            velocity.y = 0.0f;
            velocity.z = 0.0f;
            m_pPlayerExplode->UpdateSound(position, velocity);

            // Kill the player
            Die();
         }
      
         // Set the shield back to 0
         m_Shield = 0;
      }
   }
}

//////////////////////////////////////////////////////////////////////////
/// Find the position of the thrusters
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::FindThrusterPositions()
{
   // Get the players current position
   CVector3f Position;
   GetPosition(&Position, 0);

   CVector3f vector;
   CVector3f vector2;
   CVector3f right;
   CVector3f left;
   CVector3f up;
   m_RenderObj.GetFrame(0)->GetForward(&vector);
   m_RenderObj.GetFrame(0)->GetForward(&vector2);
   m_RenderObj.GetFrame(0)->GetRight(&right);
   left = right;
   m_RenderObj.GetFrame(0)->GetUp(&up);

   Vec3fScale(&vector, &vector, -5.75f);
   Vec3fScale(&right, &right, 2.5f);
   Vec3fScale(&left, &left, -2.5f);
   Vec3fScale(&up, &up, -1.25f);

   Vec3fAdd(&vector, &Position, &vector);
   Vec3fAdd(&m_Burner1, &vector, &right);
   Vec3fAdd(&m_Burner2, &vector, &left);
   Vec3fAdd(&m_Burner1, &m_Burner1, &up);
   Vec3fAdd(&m_Burner2, &m_Burner2, &up);
}

//////////////////////////////////////////////////////////////////////////
/// Players shields regen over time 
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::ShieldRegen(void)
{
   m_Shield+= m_ShieldRegenPerSec * g_FrameTime;

   if(m_Shield > m_MaxShield)
   {
      m_Shield = m_MaxShield;
   }
}

//////////////////////////////////////////////////////////////////////////
/// Players energy regen over time 
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CPlayer::EnergyRegen(void)
{
   m_Energy += m_EnergyRegenPerSec * g_FrameTime;

   if(m_Energy > m_MaxEnergy)
   {
      m_Energy = m_MaxEnergy;
   }
}

//////////////////////////////////////////////////////////////////////////
/// Get time till ion cannon can be fired again
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

int CPlayer::GetIonTimer(void)
{
   float timeremaining;
   timeremaining = (g_time - (m_IonCoolDown + m_IonLastFired));
   if(timeremaining > 0.0f)
      timeremaining = 0.0f;
   static bool temp = true;
   if(timeremaining == 0.0f)
   {
      if(temp)
      {
         GameMain.m_CurrentCommunication = GameMain.COM_IONCHARGED;
         temp = false;
      }
   }
   else
      temp = true;

   return (int)abs(timeremaining);
}

void CPlayer::IonHitsShield(int damage)
{
   if(GameMain.m_GodMode == false)
   {
      // First take damage from the shields
      m_Shield -= damage;

      if(m_Shield > 0)
      {
         m_pShieldAbsorb->Play3DAudio(25.0f, 10000.0f, 128);
         CVector3f position;
	      GetPosition(&position, 0);
         CVector3f velocity;
         velocity.x = 0.0f;
         velocity.y = 0.0f;
         velocity.z = 0.0f;
         m_pShieldAbsorb->UpdateSound(position, velocity);

         if (!m_ShowShield)
         {
            m_ShowShield = true;
            m_StartShieldTime = g_time;
            UpdateShield();
         }

         m_StartShieldTime = g_time;
      }
   }
}


