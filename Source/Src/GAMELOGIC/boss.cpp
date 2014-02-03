/// \file boss.cpp
/// Enemy boss unit
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\boss.h"
#include "gamelogic\medtank.h"
#include "core\oglwindow.h"
#include "input\inputmanager.h"
#include "core\gamemain.h"
#include "math3d\intersection.h"
#include "gamelogic\objectfactory.h"
#include "camera\camera.h"
#include "core\gamemain.h"
#include "gamelogic\player.h"
#include "graphics\terrain.h"
#include "physics\physics.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CPlayer          g_Player;
extern CObjectFactory   g_ObjectFactory;
extern CGameObject*     g_CollisionAvoidanceList;
extern CTextureFonts    g_TextureFont;
extern CGameOptions     g_GameOptions;
extern CBaseCamera*     g_Camera;
extern CAudioManager    g_AudioManager;
extern CGameMain        GameMain;
extern float            g_FrameTime;
extern CTerrain*        g_pCurrLevel;
extern CGameMain        GameMain;

/////////////////////////////////
/*/ External Globals          /*/ 
/////////////////////////////////

extern float g_time;
extern int   g_AwakeEnemies;
extern float g_FrameTime;

CBoss::CBoss()
{

}

CBoss::~CBoss()
{

}

void CBoss::Setup()
{
   SetupAI();
   m_RigidBody = new CRigidBody;
   m_Type = OBJ_BOSS;
   m_RadarType = RADAR_ENEMY;
   m_pExplosion = NULL;
   m_pThrusterLeft = NULL;
   m_pThrusterRight = NULL;

   // Set up the sound
   m_pExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_HUGE], 4);
   m_pEngineSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLAYER_JUMPJET], 10);
   m_pMainGunsSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMY_TURRET], 7);
   m_pMissilesFired = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMYMISSILEFIRED], 7);

   m_RigidBody->Initialize(this, 200.0f, m_OBB.x * 2.0f, m_OBB.y * 2.0f, m_OBB.z * 2.0f);
   m_RigidBody->SetTerrainOffset(1.0f);

   m_AwakenByPlayerRange = 350.0f;
   m_AwakenByEnemyRange = 50.0f;
         
   m_GoingDown = false;
   m_CurrentMissileBay = 0;
   m_ClusterMissileDamage = 10.0f;
   m_ClusterMissileRadius = 10.0f;
   m_HomingMissileDamage = 10.0f;
   m_HomingMissileRadius = 5.0f;
   m_Altitude = 0.0f;
   m_StrafeDist = 0.0f;
   m_StrafeDistLimit = 100.0f;
   m_StrafeTimer = g_time;
   m_StrafeTime = .4f;
   m_StrafeRight = false;
   m_CurrentTarget = &g_Player;
   m_AITimer = g_time;
   m_AI = true;
   m_DebugAI = false;
   m_Awake = false;
   m_Active = true;
   m_TurretPos = 0;
   m_HitPoints = 5000;
   m_MaxHitPoints = 5000;
   m_CurrentAggro = 0;
   m_MainWeaponLastFired = g_time;
   m_RecentHitsByPlayer = 0;
   m_DefensivenessTimer = g_time;

   m_PitchTurnSpeed = DEG_TO_RAD(18.0f);
   m_YawTurnSpeed = DEG_TO_RAD(18.0f);
   m_Speed = 25.0f;
   m_ElevationSpeed = 5.0f;
   m_ElevationSpeed2 = 15.0f;
   m_Pitch = 0.0f;

   m_ShotDamage = 10.0f;

   m_NumClusterMissiles = 10;
   m_NumHomingMissiles = 2;
   m_ClusterMissilesLastFired = g_time;
   m_MainWeaponLastFired = g_time;
   m_HomingMisslesLastFired = g_time;
   m_RiseAttackTime = 5.0f;
   m_BarrageAttackTime = 5.0f;
   m_MainWeaponCooldown = 0.2f;
   m_HomingMissileFirePer = .5;
   m_ClusterMissileFirePer = .3f;
   m_RiseAltitude = 160.0f;
   m_DesiredAltitude = 50.0f;
   m_MaxRotateX = 0.4f;
/*
   // Setup thruster
   m_pThrusterLeft = g_ParticleManager.ObtainSystem();
   if(m_pThrusterLeft)
      g_ParticleManager.CreateSystem(m_pThrusterLeft, m_ThrusterLeftPos, &m_ThrusterLeftPos, g_ParticleManager.BOSS_THRUSTER, LOD_BOTH);
      
   // Setup thruster
   m_pThrusterRight = g_ParticleManager.ObtainSystem();
   if(m_pThrusterRight)
      g_ParticleManager.CreateSystem(m_pThrusterRight, m_ThrusterRightPos, &m_ThrusterRightPos, g_ParticleManager.BOSS_THRUSTER, LOD_BOTH);   
	  */

   // Movement Info
   m_MaxVelocity  = 130.0f;    // Max velocity in meters per second
   m_AccelerationTime = 0.5f;  // Time it takes to reach that velocity
   m_TurnSpeed = 20.0f;       // Turn speed
   m_RigidBody->SetParameters(m_MaxVelocity, m_AccelerationTime);

   DebugAIOff();
}

void CBoss::SetupAI()
{
   InitStateMachine();
}

void CBoss::Update(void)
{
   if (m_HitPoints <= 0.0f)
      Die();

   /*
   // Update the engine sound
   GetPosition(&m_CurPosition, 0);
   Velocity.x = (m_CurPosition.x - m_LastPosition.x) / g_FrameTime;
   Velocity.y = (m_CurPosition.y - m_LastPosition.y) / g_FrameTime;
   Velocity.z = (m_CurPosition.z - m_LastPosition.z) / g_FrameTime;
   m_pEngineSFX->UpdateSound(m_CurPosition, Velocity);
   m_LastPosition = m_CurPosition;
   */
         
   if (m_Awake)
   {
      FindThrusterPositions();
      if (m_AI && g_GameOptions.GetAI())
      {
         // unit recovers from fear of being pummeled over time
         if (g_time - m_DefensivenessTimer > 2.0f)
         {
            m_RecentHitsByPlayer --;

            if (m_RecentHitsByPlayer < 0)
               m_RecentHitsByPlayer = 0;

            m_DefensivenessTimer = g_time;
         }

         float mag = Vec3fMagnitude(&Velocity);
         m_pEngineSFX->AdjustVolume((unsigned int)abs(mag * 50));

         // update FSM
         m_StateMachine.Update();
      }
   }
   else if (InDistanceFromPlayer(300.0f))
      Awaken();

   CGameObject::Update();
}

void CBoss::Collide(const CGameObject *obj)
{

}

void CBoss::Die(void)
{  
   if (!m_Active)
      return;

   // Make explosion sound
   m_pExplosionSFX->Play3DAudio(300, 10000.0f, 200);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_pExplosionSFX->UpdateSound(position, velocity);

   /*
   if(m_pThrusterLeft)
      m_pThrusterLeft->SetSystemAlive(false);
   if(m_pThrusterRight)
      m_pThrusterRight->SetSystemAlive(false);
	  */

   m_pEngineSFX->StopAudio();

   // Setup enemy explosion
   m_pExplosion = g_ParticleManager.ObtainSystem();
   if(m_pExplosion)
      g_ParticleManager.CreateSystem(m_pExplosion, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.BOSS_EXP, LOD_NONE);
   m_pExplosion->m_Speed = 6.0f;
   m_pExplosion->SetSize(30.0f, 30.0f);
   m_pExplosion->SetAttractionPower(5.0f, 5.0f, 5.0f);
   m_pExplosion->SetInitialParticles(600);
   m_pExplosion->SetLife(0.75f);

   Sleep();
   GameMain.m_BossAlive = false;
   
   m_Active = false;
   RemoveFromQuadtree(); 
}

void CBoss::Reset(void)
{  
   m_HitPoints = m_MaxHitPoints;
   m_pThrusterRight = NULL;
   m_pThrusterLeft = NULL; 
}

void CBoss::TakeDamage(float damage, CGameObject * guywhohitme)
{
   if (!m_Active)
      return;

   if (!m_Awake)
   {
      Awaken();
      OnAwaken();
   }

   // Take away the damage
   m_HitPoints -= damage;
   m_RecentHitsByPlayer ++;
}

bool CBoss::InDistanceFromPlayer(float Dist)
{
   bool bReturnVal = false;

   Dist = Dist * Dist;

   const Sphere* pTargetSphere = g_Player.GetSphere();
   const Sphere* pMySphere = GetSphere();

   float RadiusD = pTargetSphere->Radius + pMySphere->Radius;
   RadiusD *= RadiusD;

   float ActualDist = DistanceSquare(&pTargetSphere->Center, &pMySphere->Center) - RadiusD;

   if (ActualDist < Dist)
      bReturnVal = true;

   return bReturnVal;
}

void CBoss::AimXYTowardPlayer()
{
   float x, y;

   TargetRelativePosition(y, x);

   if (m_Pitch + x > m_MaxRotateX || m_Pitch + x < -m_MaxRotateX)
      x = 0.0f;

   RotateLocalX(-m_Pitch, 0);
   RotateLocalY(y, 0);
   RotateLocalX(m_Pitch, 0);
   RotateLocalX(x, 0);

   m_Pitch += x;
}

void CBoss::AimYTowardPlayer()
{
   float x, y;

   TargetRelativePosition(y, x);

   RotateLocalY(y, 0);
}

void CBoss::TargetRelativePosition(float& RotY, float& RotX)
{
   int TargetPosition = 0;

   // aim toward Target
   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   CVector3f TargetOrigin;
   CVector3f MyOrigin;
   CVector3f PToP;
   CVector3f MyLookAt;
   CVector3f Result;

   float MaxYTurn = m_YawTurnSpeed * g_FrameTime;

   float MaxXTurn = m_PitchTurnSpeed * g_FrameTime;

   TargetOrigin   = pTargetSphere->Center;
   
   CObjFrame Frame = *GetFrame(0);

   Frame.RotateToLocalPoint(&PToP, &TargetOrigin);

   MyLookAt.x = 0.0f;
   MyLookAt.y = 0.0f;
   MyLookAt.z = 1.0f;

   CVector3f PToP2;
   
   PToP2.x = PToP.x;
   PToP2.y = PToP.y;
   PToP2.z = PToP.z;

   PToP.y = 0.0f;

   Vec3fCrossProduct(&Result, &MyLookAt, &PToP);

   if (Result.y > 0.0f)
      TargetPosition = 1;
   else if (Result.y < 0.0f)
      TargetPosition = -1;

   if (InFrontOfMe(m_CurrentTarget))
   {
      if (PToP.z <= 0.0f || fabs(PToP.x/PToP.z) >= MaxYTurn)
         RotY = MaxYTurn * TargetPosition;
      else
         RotY = (float) atan2(PToP.x, PToP.z);

         if (RotY > MaxYTurn)
            RotY = MaxYTurn;
         else if (RotY < -MaxYTurn)
            RotY = -MaxYTurn;


      // ascertain angle to tilt turret
      float Mag = Vec3fMagnitude(&PToP2);

      if (Mag != 0.0f)
      {
         float Angle = float(asin(PToP2.y/Mag));

         if (Angle > MaxXTurn)
            Angle = MaxXTurn;
         else if (Angle < -MaxXTurn)
            Angle = -MaxXTurn;

         RotX = -Angle;
      }
      else
         RotX = 0.0f;
   }
   else
      RotX = 0.0f;
}

bool CBoss::AimIsGood()
{
   bool GotShot = false;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere.Radius = pTargetSphere->Radius;
   TempSphere.Center = pTargetSphere->Center;

   TempSphere.Radius = m_CurrentTarget->GetOBB()->y * .5f;

   Ray         ShotTrajectory;
   CObjFrame   Frame = *GetFrame(0);

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      GotShot = true;

   return GotShot;
}

bool CBoss::InFrontOfMe(CGameObject* obj)
{
   bool Fudge = false;

   const Sphere* pEnemySphere = GetSphere();
   const Sphere* pTargetSphere = obj->GetSphere();

   CVector3f TargetOrigin;
   CVector3f EnemyOrigin;
   CVector3f EToP;
   CVector3f EnemyLookAt;

   TargetOrigin   = pTargetSphere->Center;
   EnemyOrigin    = pEnemySphere->Center;

   CObjFrame   Frame = *GetFrame(0);

   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

int CBoss::TargetRelativePosition()
{
   int TargetPosition = 0;

   // aim toward Target
   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();
   Sphere TargetSphere = *pTargetSphere;
     
   float       Scale = 1.0f;

   float RadiusD = pTargetSphere->Radius + GetSphere()->Radius;
   RadiusD *= RadiusD;

   float Dist = DistanceSquare(&pTargetSphere->Center, &GetSphere()->Center) - RadiusD;

   if (Dist > 100.0f * 100.0f)
   {
      Scale = 5.0f;
   }
   else if (Dist < (80.0f * 80.0f))
      Scale = .5f;

//   TargetSphere.Radius *= Scale;

   TargetSphere.Radius = 10.0f;

   const Sphere* pEnemySphere = GetSphere();

   CVector3f TargetOrigin;
   CVector3f EnemyOrigin;
   CVector3f EToP;
   CVector3f EnemyLookAt;
   CVector3f Result;

   TargetOrigin   = pTargetSphere->Center;
   EnemyOrigin    = pEnemySphere->Center;

   TargetOrigin.x += m_OffsetFromTarget.x;

   const CObjFrame* Frame = GetFrame(0);

   // aim tank on y
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);
   Frame->GetForward(&EnemyLookAt);

   EnemyLookAt.y = 0.0f;
   EToP.y = 0.0f;

   Vec3fCrossProduct(&Result, &EnemyLookAt, &EToP);

   if (Result.y > 0.0f)
      TargetPosition = 1;
   else if (Result.y < 0.0f)
      TargetPosition = -1;

   return TargetPosition;
}


void CBoss::AdjustAltitude(float level)
{
   Plane p;
	CVector3f pos;
   CObjFrame frame = *GetFrame(0);
   
	frame.GetPosition(&pos);

	if (!g_pCurrLevel->GetPlaneAtPoint(pos.x, pos.z, &p))
      return;

	float offset = ClampToPlane(&pos, &p, level);

   if (offset == 0.0f)
      return;

   float speed;
   
   speed = m_ElevationSpeed2 * g_FrameTime;


   if (offset < speed && offset > -speed)
   {
      MoveUp(offset, 0);
      return;
   }

   if (offset > 0.0f)
   {
      // move up
      MoveUp(speed, 0);
      m_Altitude += speed;
   }
   else if (offset < 0.0f)
   {
      // move down
      MoveUp(-speed, 0);
      m_Altitude -= speed;
   }

}

bool CBoss::HasRisen()
{
   Plane p;
	CVector3f pos;
   CObjFrame frame = *GetFrame(0);
   
	frame.GetPosition(&pos);

	if (!g_pCurrLevel->GetPlaneAtPoint(pos.x, pos.z, &p))
      return true;

	float offset = ClampToPlane(&pos, &p, m_RiseAltitude);

   if (offset < 1.0f && offset > -1.0f)
      return true;
   else
      return false;
}

bool CBoss::HasFallen()
{
   if (m_GoingDown == false)
      return false;

   Plane p;
	CVector3f pos;
   CObjFrame frame = *GetFrame(0);
   
	frame.GetPosition(&pos);

	if (!g_pCurrLevel->GetPlaneAtPoint(pos.x, pos.z, &p))
      return false;

	float offset = ClampToPlane(&pos, &p, m_DesiredAltitude);

   if (offset < 1.0f && offset > -1.0f)
      return true;
   else
      return false;
}

void CBoss::AdjustAltitude()
{
   Plane p;
	CVector3f pos;
   CObjFrame frame = *GetFrame(0);
   
	frame.GetPosition(&pos);

	if (!g_pCurrLevel->GetPlaneAtPoint(pos.x, pos.z, &p))
      return;

	float offset = ClampToPlane(&pos, &p, m_DesiredAltitude);

   if (offset == 0.0f)
      return;

   float speed = m_ElevationSpeed * g_FrameTime;

   if (offset < speed  && offset > -speed)
   {
      MoveUp(offset, 0);
      offset = 0.0f;
   }

   if (offset > 0.0f)
   {
      // move up
      MoveUp(speed, 0);
      m_Altitude += speed;
   }
   else if (offset < 0.0f)
   {
      // move down
      MoveUp(-speed, 0);
      m_Altitude -= speed;
   }
}

void CBoss::AdjustPitchToDefault()
{
   float MaxXTurn = m_PitchTurnSpeed * g_FrameTime;

   if (m_Pitch < MaxXTurn && m_Pitch > -MaxXTurn)
   {
      RotateLocalX(-m_Pitch, 0);
      m_Pitch = 0.0f;
   }

   if (m_Pitch > 0.0f)
   {
      RotateLocalX(-MaxXTurn,0);
      m_Pitch -= MaxXTurn;
   }
   else if (m_Pitch < 0.0f)
   {
      RotateLocalX(MaxXTurn,0);
      m_Pitch += MaxXTurn;
   }
}

void CBoss::FirePlasma()
{
   // Check to see if the weapon is ready to be fired
   if(g_time - m_MainWeaponLastFired > m_MainWeaponCooldown)
   {
      m_MainWeaponLastFired = g_time;

      // Play sound of main gun firing
      m_pMainGunsSFX->Play3DAudio(75, 10000.0f, 200);     
      // Update that sound
      CVector3f position;
	   GetPosition(&position, 0);
      CVector3f velocity(0.0f, 0.0f, 0.0f);
      m_pMainGunsSFX->UpdateSound(position, velocity);
   
      CPlasmaShot * shot1 = g_ObjectFactory.CreatePlasmaShot();
      CPlasmaShot * shot2 = g_ObjectFactory.CreatePlasmaShot();

      shot1->SetUp(ENEMY_SHOT, this, m_ShotDamage);
      shot2->SetUp(ENEMY_SHOT, this, m_ShotDamage);

      // Find the position of the shot
      CObjFrame frame1, frame2;

      frame1 = *GetFrame(0);

      frame1.MoveForward(30.0f);

      frame2 = frame1;

      frame1.MoveRight(14.0f);
      frame2.MoveRight(-14.0f);

      frame1.RotateLocalY(.07f);
      frame2.RotateLocalY(-.07f);

      shot1->SetFrame(&frame1, 0);
      shot2->SetFrame(&frame2, 0);
      
      CParticleSystem * effect1 = g_ParticleManager.ObtainSystem();
      // If obtained a particle effect then we use it
      if(effect1)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(effect1, *(shot1->GetFrame(0)->GetPosition()),
         NULL, g_ParticleManager.ENEMY_SHOT, LOD_BOTH);
      }

      // Get a particle effect for the shot if one available
      shot1->m_ParticleEffect = g_ParticleManager.ObtainSystem();
      // If obtained a particle effect then we use it
      if(shot1->m_ParticleEffect != NULL)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(shot1->m_ParticleEffect, *(shot1->GetFrame(0)->GetPosition()),
         shot1->GetFrame(0)->GetPosition(), g_ParticleManager.ENEMY_PLASMA, LOD_BOTH);
      }  

      CParticleSystem * effect2 = g_ParticleManager.ObtainSystem();
      if(effect2)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(effect2, *(shot2->GetFrame(0)->GetPosition()),
         NULL, g_ParticleManager.ENEMY_SHOT, LOD_BOTH);
      }   

      shot2->m_ParticleEffect = g_ParticleManager.ObtainSystem();
      if(shot2->m_ParticleEffect != NULL)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(shot2->m_ParticleEffect, *(shot2->GetFrame(0)->GetPosition()),
         shot2->GetFrame(0)->GetPosition(), g_ParticleManager.ENEMY_PLASMA, LOD_BOTH);
      }  
   }
}

void CBoss::FireClusterMissiles()
{
   // Check to see if the weapon is ready to be fired
   if(g_time - m_ClusterMissilesLastFired > m_ClusterMissileFirePer)
   { 
      m_ClusterMissilesLastFired = g_time;

      float offsetx, offsety;
      
      for(int i = 0; i < m_NumClusterMissiles; i++)
      {
         CMissile * shot = g_ObjectFactory.CreateMissile();

         // Play sound of missile firing
         m_pMissilesFired->Play3DAudio(50, 10000.0f, 120);

         // Update that sound
         CVector3f position;
	      GetPosition(&position, 0);
         CVector3f velocity(0.0f, 0.0f, 0.0f);
         m_pMissilesFired->UpdateSound(position, velocity);

         // Find the position of the shot
         CObjFrame frame;

         switch(i)
         {
            case 0:
            {
               offsetx = -7.0f;
               offsety = -1.0f; 
            }          
            break;

            case 1:
            {
               offsetx = 7.0f;
               offsety = -1.0f;
            }
            break;

            case 2:
            {
               offsetx = -6.0f;
               offsety = 0.0f; 
            }          
            break;

            case 3:
            {
               offsetx = 6.0f;
               offsety = 0.0f;
            }
            break;

            case 4:
            {
               offsetx = -5.0f;
               offsety = -1.0f; 
            }          
            break;

            case 5:
            {
               offsetx = 5.0f;
               offsety = -1.0f;
            }
            break;

            case 6:
            {
               offsetx = -6.0f;
               offsety = -2.0f; 
            }          
            break;

            case 7:
            {
               offsetx = 6.0f;
               offsety = -2.0f;
            }
            break;

            case 8:
            {
               offsetx = -6.0f;
               offsety = -1.0f; 
            }          
            break;

            case 9:
            {
               offsetx = 6.0f;
               offsety = -1.0f;
            }
            break;
         }
     
         frame = *GetFrame(0);

         frame.MoveForward(30.0f);
         frame.MoveUp(10.0f);

         shot->SetFrame(&frame, 0);
         shot->SetUp(ENEMY_SHOT, (int)m_ClusterMissileDamage, i, m_ClusterMissileRadius, this);
      
         shot->m_FireTrail = g_ParticleManager.ObtainSystem();

         // If obtained a particle effect then we use it
         if(shot->m_FireTrail != NULL)
         {
            // Create an effect for the shot
            g_ParticleManager.CreateSystem(shot->m_FireTrail, *(shot->GetFrame(0)->GetPosition()),
            shot->GetFrame(0)->GetPosition(), g_ParticleManager.MISSILE_TRAIL, LOD_FAR);
         }  
      }
   }
}

void CBoss::FireHomingMissiles()
{
   // Check to see if the weapon is ready to be fired
   if(g_time - m_HomingMisslesLastFired > m_HomingMissileFirePer)
   {            
      m_HomingMisslesLastFired = g_time;  
                   
      // Play sound of missile firing
      m_pMissilesFired->Play3DAudio(50, 10000.0f, 120);

      // Update that sound
      CVector3f position;
	   GetPosition(&position, 0);
      CVector3f velocity(0.0f, 0.0f, 0.0f);
      m_pMissilesFired->UpdateSound(position, velocity);

      // Fire the missile at the right place
      float offsetx, offsety;    


      CTrackingMissile * shot;
      
      // Find the position of the shot
      CObjFrame frame;
      frame = *GetFrame(0);

      frame.MoveUp(20.0f);

      for (int i = 0; i < m_NumHomingMissiles; i++)
      {

         shot = g_ObjectFactory.CreateTrackingMissile();

         switch(m_CurrentMissileBay)
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

         shot->SetFrame(&frame, 0);

         shot->SetUp(ENEMY_SHOT, (int)m_HomingMissileDamage, false, 10.0f, 0.6f, this);
         shot->SetTarget(m_CurrentTarget);
   
         shot->m_FireTrail = g_ParticleManager.ObtainSystem();

         // If obtained a particle effect then we use it
         if(shot->m_FireTrail != NULL)
         {
            // Create an effect for the shot
            g_ParticleManager.CreateSystem(shot->m_FireTrail, *(shot->GetFrame(0)->GetPosition()),
            shot->GetFrame(0)->GetPosition(), g_ParticleManager.TRKMISSILE_TRAIL, LOD_BOTH);
         }  

         shot->m_SmokeTrail = g_ParticleManager.ObtainSystem();

         // If obtained a particle effect then we use it
         if(shot->m_SmokeTrail != NULL)
         {
            // Create an effect for the shot
            g_ParticleManager.CreateSystem(shot->m_SmokeTrail, *(shot->GetFrame(0)->GetPosition()),
            shot->GetFrame(0)->GetPosition(), g_ParticleManager.TRKMISSILE_TRAIL_SMOKE, LOD_BOTH);
         }  

         m_CurrentMissileBay ++;

         if (m_CurrentMissileBay > 10)
            m_CurrentMissileBay = 0;
      }
   }
}

void CBoss::OnAwaken(void)
{  
  // Play the engine sound	when enemy is awakened
  GetPosition(&m_CurPosition, 0);
  m_LastPosition = m_CurPosition; 
  m_pEngineSFX->Play3DAudio(100, 10000.0f, 200); 
}

void CBoss::InitStateMachine(void)
{
   CVector2f vec;
   
   if (m_DebugAI)
   {
      vec.x = 0.0f;
      vec.y = -.8f;

      g_TextureFont.SetScale(.05f);
      g_TextureFont.SetRowSize(20);
      g_TextureFont.SetLocation(vec);
   }

   // add states
    
   m_StateMachine.AddState("AvoidPlayer");
   m_StateMachine.AddState("RiseAttack");
   m_StateMachine.AddState("BarrageAttack");

   CFSM* RiseMachine;

   RiseMachine   = m_StateMachine.AddStateMachine("RiseAttack");
   
   m_Rise = RiseMachine;

   RiseMachine->AddState("GoUp");
   RiseMachine->AddState("Attack");
   RiseMachine->AddState("GoDown");

// sub-state2 transition
   RiseMachine->AddTransitionCondition("GoUp", "Attack", BossGoUpToAttack);
   RiseMachine->AddTransitionCondition("Attack", "GoDown", BossAttackToGoDown);

// sub-state2 update
   RiseMachine->AddUpdate("GoUp", BossGoUpUpdate);
   RiseMachine->AddUpdate("Attack", BossAttackUpdate);
   RiseMachine->AddUpdate("GoDown", BossGoDownUpdate);

// sub-state2 enter
   RiseMachine->AddOnEnter("GoUp", BossGoUpOnEnter);
   RiseMachine->AddOnEnter("Attack", BossAttackOnEnter);
   RiseMachine->AddOnEnter("GoDown", BossGoDownOnEnter);

// sub-state2 exit
   RiseMachine->AddOnExit("GoUp", BossGoUpOnExit);
   RiseMachine->AddOnExit("Attack", BossAttackOnExit);
   RiseMachine->AddOnExit("GoDown", BossGoDownOnExit);

   // add enter functions for each state
   m_StateMachine.AddOnEnter("AvoidPlayer", BossAvoidPlayerOnEnter);
   m_StateMachine.AddOnEnter("RiseAttack", BossRiseAttackOnEnter);
   m_StateMachine.AddOnEnter("BarrageAttack", BossBarrageAttackOnEnter);

   // add exit functions for each state
   m_StateMachine.AddOnExit("AvoidPlayer", BossAvoidPlayerOnExit);
   m_StateMachine.AddOnExit("RiseAttack", BossRiseAttackOnExit);
   m_StateMachine.AddOnExit("BarrageAttack", BossBarrageAttackOnExit);


   // add update functions for each state
   m_StateMachine.AddUpdate("AvoidPlayer", BossAvoidPlayerUpdate);
   m_StateMachine.AddUpdate("RiseAttack", BossRiseAttackUpdate);
   m_StateMachine.AddUpdate("BarrageAttack", BossBarrageAttackUpdate);

   // set transitions between states
   m_StateMachine.AddTransitionCondition("AvoidPlayer", "RiseAttack", BossAvoidPlayerToRiseAttack);
   m_StateMachine.AddTransitionCondition("AvoidPlayer", "BarrageAttack", BossAvoidPlayerToBarrageAttack);
   m_StateMachine.AddTransitionCondition("RiseAttack", "AvoidPlayer", BossRiseAttackToAvoidPlayer);
   m_StateMachine.AddTransitionCondition("RiseAttack", "BarrageAttack", BossRiseAttackToBarrageAttack);
   m_StateMachine.AddTransitionCondition("BarrageAttack", "AvoidPlayer", BossBarrageAttackToAvoidPlayer);

   m_StateMachine.SetCurrentState(0);
   m_StateMachine.SetObjectToModify((void*)this);
   RiseMachine->SetCurrentState(0);
   RiseMachine->SetObjectToModify((void*)this);
}

// sub-state2 transition
bool BossGoUpToAttack(void* pVoidObj)
{

   CBoss* pObj = (CBoss*) pVoidObj;
   pObj->m_AITimer = g_time;
   return pObj->HasRisen();
}

bool BossAttackToGoDown(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;
   bool Fudge = false;

   if (g_time - pObj->m_AITimer > pObj->m_RiseAttackTime)
      Fudge = true;

   return Fudge;
}

// sub-state2 update
void BossGoUpUpdate(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;

   if (pObj->m_Pitch != 0.0f)
      pObj->AdjustPitchToDefault();
   else
      pObj->AdjustAltitude(pObj->m_RiseAltitude);
}

void BossAttackUpdate(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;
   pObj->FireHomingMissiles();
}

void BossGoDownUpdate(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;

   pObj->m_GoingDown = true;
   pObj->AdjustAltitude(pObj->m_DesiredAltitude);
}

// sub-state2 enter
void BossGoUpOnEnter(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;
}

void BossAttackOnEnter(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;

   pObj->m_AITimer = g_time;
}

void BossGoDownOnEnter(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;
   pObj->m_GoingDown = true;
}

// sub-state2 exit
void BossGoUpOnExit(void* pVoidObj)
{

}

void BossAttackOnExit(void* pVoidObj)
{

}

void BossGoDownOnExit(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;
}

// enter functions for each state
void BossAvoidPlayerOnEnter(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;

   pObj->m_AITimer = g_time;
}

void BossRiseAttackOnEnter(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;

   pObj->m_AITimer = g_time;
}

void BossBarrageAttackOnEnter(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;

   pObj->m_AITimer = g_time;
}

// exit functions for each state
void BossAvoidPlayerOnExit(void* pVoidObj)
{

}

void BossRiseAttackOnExit(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;
   pObj->m_GoingDown = false;
   pObj->m_Rise->SetCurrentState(0);
}

void BossBarrageAttackOnExit(void* pVoidObj)
{

}

// update functions for each state
void BossAvoidPlayerUpdate(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;
   pObj->AdjustAltitude();

   if (pObj->InPlayersAim())
   {
      float speed = pObj->m_Speed * g_FrameTime;

      if (pObj->m_StrafeDist + speed > pObj->m_StrafeDistLimit)
      {
         pObj->m_StrafeRight = !pObj->m_StrafeRight;
         pObj->m_StrafeDist = 0.0f;
      }

      if (pObj->m_StrafeRight)
         pObj->MoveRight(speed, 0);
      else
         pObj->MoveRight(-speed, 0);

      pObj->m_StrafeDist += speed;
   }

   if (!pObj->AimIsGood())
      pObj->AimXYTowardPlayer();
   else
      pObj->FirePlasma();
}


void BossRiseAttackUpdate(void* pVoidObj)
{

}

void BossBarrageAttackUpdate(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;

   if (!pObj->AimIsGood())
      pObj->AimXYTowardPlayer();

   pObj->FireClusterMissiles();
}

// transitions between states
bool BossAvoidPlayerToRiseAttack(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;
   bool Fudge = false;

   if(pObj->m_RecentHitsByPlayer > 5)
      Fudge = true;

   return Fudge;
}

bool BossAvoidPlayerToBarrageAttack(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;
   bool Fudge = false;

   float thetime = g_time - pObj->m_AITimer;

   if (thetime > 6.0f)
   {
      pObj->m_AITimer = g_time;
      if (rand()% 100 < 35)
         Fudge = true;
   }

   return Fudge;
}

bool BossRiseAttackToAvoidPlayer(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;

   return pObj->HasFallen();
}

bool BossRiseAttackToBarrageAttack(void* pVoidObj)
{
   return false;
}

bool BossBarrageAttackToAvoidPlayer(void* pVoidObj)
{
   CBoss* pObj = (CBoss*) pVoidObj;
   bool Fudge = false;
   
   float thetime = g_time - pObj->m_AITimer;

   if (thetime > pObj->m_BarrageAttackTime)
      Fudge = true;

   return Fudge;
}

//////////////////////////////////////////////////////////////////////////
/// Find the position of the thrusters
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CBoss::FindThrusterPositions()
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

   Vec3fScale(&vector, &vector, -25.0f);
   Vec3fScale(&right, &right, 5.0f);
   Vec3fScale(&left, &left, -5.0f);
   Vec3fScale(&up, &up, 5.0f);

   Vec3fAdd(&vector, &Position, &vector);
   Vec3fAdd(&m_ThrusterLeftPos, &vector, &right);
   Vec3fAdd(&m_ThrusterRightPos, &vector, &left);
   Vec3fAdd(&m_ThrusterLeftPos, &m_ThrusterLeftPos, &up);
   Vec3fAdd(&m_ThrusterRightPos, &m_ThrusterRightPos, &up);
}
