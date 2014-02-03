/// \file medtank.cpp
/// Enemy med tank unit
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\medtank.h"
#include "core\oglwindow.h"
#include "input\inputmanager.h"
#include "core\gamemain.h"
#include "math3d\intersection.h"
#include "gamelogic\objectfactory.h"
#include "camera\camera.h"
#include "gamelogic\player.h"
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
extern CGameMain        GameMain;
   
/////////////////////////////////
/*/ External Globals          /*/ 
/////////////////////////////////

extern float   g_time;
extern int     g_AwakeEnemies;
extern float   g_FrameTime;

CMedTank::CMedTank(CMedTank& MedTank) : CEnemy(MedTank)
{
    SetupAI();
    m_RigidBody = new CRigidBody;
    m_Type = OBJ_MEDTANK;

    m_RigidBody->Initialize(this, 200.0f, m_OBB.x * 2.0f, m_OBB.y * 2.0f, m_OBB.z * 2.0f);
    m_RigidBody->SetTerrainOffset(1.0f);

	// Setup Audio
	m_pExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_MED], 5);
	m_pMainGunsSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMY_TURRET], 4);
	m_pEngineSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMY_ENGINE], 0);

    m_AwakenByPlayerRange = 350.0f;
    m_AwakenByEnemyRange = 50.0f;
}

CMedTank::CMedTank()
{

}

CMedTank::~CMedTank()
{

}

void CMedTank::Setup()
{
   m_AI = true;
   m_Awake = false;
   m_Active = true;
   m_CurrentTarget = &g_Player;
   m_CurrentAggro = 0;
   m_MainWeaponLastFired = g_time;
   m_RecentHitsByPlayer = 0;
   m_DefensivenessTimer = g_time;
   m_MainWeaponLastFired = 0;
   m_TurretPos = 0.0f;
   m_CurTurretState = STANDBY;

   switch(g_GameOptions.GetDifficulty())
   {
      case EASY_DIFFICULTY:
      {
         m_HitPoints = 40;
         m_MaxHitPoints = 40;
         m_MainWeaponCooldown = 2.0f;
         m_ShotDamage = 15;
      }
         break;

      case MEDIUM_DIFFICULTY:
      {
         m_HitPoints = 50;
         m_MaxHitPoints = 50;
         m_MainWeaponCooldown = 1.0f;
         m_ShotDamage = 25;
      }
      	break;

      case HARD_DIFFICULTY:
      {
         m_HitPoints = 60;
         m_MaxHitPoints = 60;
         m_MainWeaponCooldown = .5f;
         m_ShotDamage = 35;
      }

      default:
         break;
   };

   // Movement Info
   m_MaxVelocity  = 60.0f;    // Max velocity in meters per second
   m_AccelerationTime = 0.5f; // Time it takes to reach that velocity
   m_TurnSpeed = 10.0f;       // Turn speed
   m_RigidBody->SetParameters(m_MaxVelocity, m_AccelerationTime);

   DebugAIOff();

   // Get the turrets position
   m_RenderObj.GetPosition(&m_TurretPosition, 1);
}

void CMedTank::SetupAI()
{
   m_AITimer = g_time;
   m_StrafeTimer = g_time;
   m_OffsetFromTarget.x = 0.0f;
   m_OffsetFromTarget.y = 0.0f;
   m_OffsetFromTarget.z = 0.0f;
   InitStateMachine();
}

void CMedTank::Print(string txt)
{
   g_TextureFont.PrintText(txt);
}


void CMedTank::TextLoc(CVector2f vec)
{
   g_TextureFont.SetLocation(vec);
}

bool CMedTank::InDistanceFromPlayer(float Dist)
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

bool CMedTank::InFrontOfMe(CGameObject* obj)
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

   CObjFrame   Frame;
   FindShotPosition(&Frame);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

void CMedTank::Update(void)
{
   // Tank is dead
   if(m_HitPoints <= 0)
   {
      Die();
      return;
   }

   if (m_Awake)
   {
      if (m_CurrentTarget != NULL)
      {
         if (!m_CurrentTarget->IsActive()
               || DistanceSquare(&m_CurrentTarget->GetSphere()->Center, &GetSphere()->Center) > 400.0f * 400.0f)
            TargetEnemy();
      }

      // Update the physics
      m_RigidBody->StepIntegrate(g_FrameTime);

      // Update the position of the turret
      UpdateTurretState();

      if (m_AI && g_GameOptions.GetAI())
      {

         CInputManager::GetInstance()->PollKeyboard();

         // unit recovers from fear of being pummeled over time
         if (g_time - m_DefensivenessTimer > 5.0f)
         {
            m_RecentHitsByPlayer --;

            if (m_RecentHitsByPlayer < 0)
               m_RecentHitsByPlayer = 0;

            m_DefensivenessTimer = g_time;
         }

         // update AGGRESSION level

         DetermineAggro();

         // update FSM
         m_StateMachine.Update();
      }

      // Update the engine sound
      GetPosition(&m_CurPosition, 0);
      CVector3f Velocity;
      Velocity.x = (m_CurPosition.x - m_LastPosition.x) / g_FrameTime;
      Velocity.y = (m_CurPosition.y - m_LastPosition.y) / g_FrameTime;
      Velocity.z = (m_CurPosition.z - m_LastPosition.z) / g_FrameTime;
      m_pEngineSFX->UpdateSound(m_CurPosition, Velocity);
      m_LastPosition = m_CurPosition;
   }
   else
      ScanForPlayer();

   CGameObject::Update();
}

void CMedTank::ScanForPlayer()
{
   CGameObject* pCurrentObj = GetWhoIsAroundMe2(m_AwakenByPlayerRange);
   CGameObject* pFirstGuy = pCurrentObj;

   if (pCurrentObj == NULL)
      return;

   bool done = false;

   const Sphere* pMeSphere = GetSphere();
   Point3f pt1 = pMeSphere->Center;
   Point3f pt2;

   do 
   {  
      if (pCurrentObj->GetType() == OBJ_PLAYER && 
         (PlayerInFrontOfMe() || InDistanceFromPlayer(30.0f)))
      {
         
         Awaken();
         OnAwaken();

         pCurrentObj = pCurrentObj->m_GuyNextToMe;
         break;
      }

      switch(pCurrentObj->GetType())
      {
         case OBJ_BUILDING:
         case OBJ_BUILDING2:
         case OBJ_BUILDING3:
         case OBJ_CONVOY:
         case OBJ_FTURRET:
         {
            Awaken();
            OnAwaken();
         	break;
         }
         default:
            break;
      }

      pt2 = pCurrentObj->GetSphere()->Center;

      if (DistanceSquare(&pt1, &pt2) > m_AwakenByEnemyRange * m_AwakenByEnemyRange)
      {
         pCurrentObj = pCurrentObj->m_GuyNextToMe;
         continue;
      }

      switch(pCurrentObj->GetType())
      {
         case OBJ_MEDTANK:
         {
            if ( ((CEnemy*) pCurrentObj)->IsAwake() )
            {
               Awaken();
			   OnAwaken();
            }
         }
            break;

         case OBJ_KAMITANK:
         {
            if ( ((CEnemy*) pCurrentObj)->IsAwake() )
            {
               Awaken();
			   OnAwaken();
            }
         }
            break;

         case OBJ_HVYTANK:
         {
            if ( ((CEnemy*) pCurrentObj)->IsAwake() )
            {
               Awaken();
			   OnAwaken();
            }
         }
            break;

         case OBJ_SCOUT:
         {
            if ( ((CEnemy*) pCurrentObj)->IsAwake() )
            {
               Awaken();
			   OnAwaken();
            }
         }
            break;

         default:
         {
         }
            break;
      };

      if (IsAwake())
         break;

      pCurrentObj = pCurrentObj->m_GuyNextToMe;
   } while(pCurrentObj != NULL);

   ClearWhoIsAroundMe(pFirstGuy);

}

bool CMedTank::PlayerInFrontOfMe()
{
   bool Fudge = false;

   const Sphere* pEnemySphere = GetSphere();
   const Sphere* pPlayerSphere = g_Player.GetSphere();

   CVector3f PlayerOrigin;
   CVector3f EnemyOrigin;
   CVector3f EToP;
   CVector3f EnemyLookAt;

   PlayerOrigin   = pPlayerSphere->Center;
   EnemyOrigin    = pEnemySphere->Center;

   CObjFrame   Frame;
   FindShotPosition(&Frame);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &PlayerOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

bool CMedTank::TargetInFrontOfMe()
{
   bool Fudge = false;

   const Sphere* pEnemySphere = GetSphere();
   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   CVector3f TargetOrigin;
   CVector3f EnemyOrigin;
   CVector3f EToP;
   CVector3f EnemyLookAt;

   TargetOrigin   = pTargetSphere->Center;
   EnemyOrigin    = pEnemySphere->Center;

   CObjFrame   Frame;
   FindShotPosition(&Frame);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

void CMedTank::Collide(const CGameObject *obj)
{
   switch(obj->GetType())
   {
      case OBJ_PLAYER:
      {
         if (!m_Awake)
		 {
            Awaken();
            OnAwaken();
		 }
      }
         break;

      default:
         break;
   };

}

bool CMedTank::InDistanceFromTarget(float Dist)
{
   bool bReturnVal = false;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();
   const Sphere* pMySphere = GetSphere();

   if (DistanceSquare(&pTargetSphere->Center, &pMySphere->Center) < Dist * Dist)
      bReturnVal = true;

   return bReturnVal;
}


void CMedTank::Die(void)
{
   // Make explosion sound
   m_pExplosionSFX->Play3DAudio(40, 10000.0f, 120);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_pExplosionSFX->UpdateSound(position, velocity);

   // Setup enemy explosion
   explode = g_ParticleManager.ObtainSystem();
   if(explode)
      g_ParticleManager.CreateSystem(explode, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.AURA, LOD_BOTH);
   Sleep();
   m_Active = false;
   RemoveFromQuadtree();
   GameMain.m_NumEnemiesAlive--;
   g_Player.m_EnemyMedTanksDestroyed++;
   DropPowerup(25.0f);
}

void CMedTank::Reset(void)
{
   
}

void CMedTank::TakeDamage(float damage, CGameObject * guywhohitme)
{
   if (!m_Active)
      return;

   if (!m_Awake)
   {
      Awaken();
      OnAwaken();
   }

   if (guywhohitme != m_LastHitBy)
   {
      m_LastHitBy = guywhohitme;
      m_HitCounter = 1;
   }
   else
   {
      m_HitCounter ++;
      if (m_HitCounter > 1)
         m_CurrentTarget = guywhohitme;
   }

   // Take away the damage
   m_HitPoints -= damage;

   m_RecentHitsByPlayer ++;
}

void CMedTank::InitStateMachine(void)
{
   DebugAIOn();

   CInputManager::GetInstance()->Init();
   CInputManager::GetInstance()->SetTimerInput(-1.0f);
   CVector2f vec;
   
   if (m_DebugAI)
   {
      vec.x = 0.0f;
      vec.y = -.8f;

      g_TextureFont.SetScale(.05f);
      g_TextureFont.SetRowSize(20);
      g_TextureFont.SetLocation(vec);
   }

   // set base aggressiveness
   m_BaseAggro = .5f;

   if (g_GameOptions.GetDifficulty() == EASY_DIFFICULTY)
      m_BaseAggro -= .2f;

   if (g_GameOptions.GetDifficulty() == HARD_DIFFICULTY)
      m_BaseAggro += .25f;

   // add states
   
   m_StateMachine.SetObjectToModify((void*)this);
 
   m_StateMachine.AddState("Fight");
   m_StateMachine.AddState("Pursue");
   m_StateMachine.AddState("Retreat");

   CFSM* FightMachine;
   CFSM* ShootMachineStill;
   CFSM* ShootMachineStrafe;

   FightMachine   = m_StateMachine.AddStateMachine("Fight");

   FightMachine->AddState("Still");
   FightMachine->AddState("Strafe");
   
   ShootMachineStill    = FightMachine->AddStateMachine("Still");
   ShootMachineStrafe   = FightMachine->AddStateMachine("Strafe");

   ShootMachineStill->AddState("Aim");
   ShootMachineStill->AddState("Fire");
   ShootMachineStill->AddState("Docile");
   ShootMachineStill->AddTransitionCondition("Aim", "Fire", MedTankAimToFire);
   ShootMachineStill->AddTransitionCondition("Fire", "Aim", MedTankFireToAim);
   ShootMachineStill->AddTransitionCondition("Aim", "Docile", MedTankAimToDocile);
   ShootMachineStill->AddTransitionCondition("Docile", "Aim", MedTankDocileToAim);
   ShootMachineStill->AddUpdate("Aim", MedTankAimUpdate);
   ShootMachineStill->AddUpdate("Fire", MedTankFireUpdate);
   ShootMachineStill->AddUpdate("Docile", MedTankDocileUpdate);

   ShootMachineStill->AddOnEnter("Aim", MedTankAimOnEnter);
   ShootMachineStill->AddOnEnter("Fire", MedTankFireOnEnter);
   ShootMachineStill->AddOnEnter("Docile", MedTankDocileOnEnter);
   ShootMachineStill->AddOnExit("Aim", MedTankAimOnExit);
   ShootMachineStill->AddOnExit("Fire", MedTankFireOnExit);
   ShootMachineStill->AddOnExit("Docile", MedTankDocileOnExit);

   ShootMachineStrafe->AddState("Aim");
   ShootMachineStrafe->AddState("Fire");
   ShootMachineStrafe->AddState("Docile");
   ShootMachineStrafe->AddTransitionCondition("Aim", "Fire", MedTankAimToFire);
   ShootMachineStrafe->AddTransitionCondition("Fire", "Aim", MedTankFireToAim);
   ShootMachineStrafe->AddTransitionCondition("Aim", "Docile", MedTankAimToDocile);
   ShootMachineStrafe->AddTransitionCondition("Docile", "Aim", MedTankDocileToAim);
   ShootMachineStrafe->AddUpdate("Aim", MedTankAimUpdate);
   ShootMachineStrafe->AddUpdate("Fire", MedTankFireUpdate);
   ShootMachineStrafe->AddUpdate("Docile", MedTankDocileUpdate);

   ShootMachineStrafe->AddOnEnter("Aim", MedTankAimOnEnter);
   ShootMachineStrafe->AddOnEnter("Fire", MedTankFireOnEnter);
   ShootMachineStrafe->AddOnEnter("Docile", MedTankDocileOnEnter);
   ShootMachineStrafe->AddOnExit("Aim", MedTankAimOnExit);
   ShootMachineStrafe->AddOnExit("Fire", MedTankFireOnExit);
   ShootMachineStrafe->AddOnExit("Docile", MedTankDocileOnExit);


   CFSM* ShootMachineRetreat;

   ShootMachineRetreat = m_StateMachine.AddStateMachine("Retreat");

   ShootMachineRetreat->AddState("Aim");
   ShootMachineRetreat->AddState("Fire");
   ShootMachineRetreat->AddState("Docile");
   ShootMachineRetreat->AddTransitionCondition("Aim", "Fire", MedTankAimToFire);
   ShootMachineRetreat->AddTransitionCondition("Fire", "Aim", MedTankFireToAim);
   ShootMachineRetreat->AddTransitionCondition("Aim", "Docile", MedTankAimToDocile);
   ShootMachineRetreat->AddTransitionCondition("Docile", "Aim", MedTankDocileToAim);
   ShootMachineRetreat->AddUpdate("Aim", MedTankAimUpdate);
   ShootMachineRetreat->AddUpdate("Fire", MedTankFireUpdate);
   ShootMachineRetreat->AddUpdate("Docile", MedTankDocileUpdate);
   ShootMachineRetreat->AddOnEnter("Aim", MedTankAimOnEnter);
   ShootMachineRetreat->AddOnEnter("Fire", MedTankFireOnEnter);
   ShootMachineRetreat->AddOnEnter("Docile", MedTankDocileOnEnter);
   ShootMachineRetreat->AddOnExit("Aim", MedTankAimOnExit);
   ShootMachineRetreat->AddOnExit("Fire", MedTankFireOnExit);
   ShootMachineRetreat->AddOnExit("Docile", MedTankDocileOnExit);



   FightMachine->AddOnEnter("Still", MedTankStillOnEnter);
   FightMachine->AddOnEnter("Strafe", MedTankStrafeOnEnter);
   FightMachine->AddOnExit("Still", MedTankStillOnExit);
   FightMachine->AddOnExit("Strafe", MedTankStrafeOnExit);
   FightMachine->AddUpdate("Still", MedTankStillUpdate);
   FightMachine->AddUpdate("Strafe", MedTankStrafeUpdate);
   FightMachine->AddTransitionCondition("Still", "Strafe", MedTankStillToStrafe);
   FightMachine->AddTransitionCondition("Strafe", "Still", MedTankStrafeToStill);



   // add enter functions for each state
   m_StateMachine.AddOnEnter("Fight", MedTankFightOnEnter);
   m_StateMachine.AddOnEnter("Retreat", MedTankRetreatOnEnter);
   m_StateMachine.AddOnEnter("Pursue", MedTankPursueOnEnter);

   // add exit functions for each state
   m_StateMachine.AddOnExit("Fight", MedTankFightOnExit);
   m_StateMachine.AddOnExit("Retreat", MedTankRetreatOnExit);
   m_StateMachine.AddOnExit("Pursue", MedTankPursueOnExit);


   // add update functions for each state
   m_StateMachine.AddUpdate("Fight", MedTankFightUpdate);
   m_StateMachine.AddUpdate("Retreat", MedTankRetreatUpdate);
   m_StateMachine.AddUpdate("Pursue", MedTankPursueUpdate);

   // set transitions between states
   m_StateMachine.AddTransitionCondition("Fight", "Retreat", MedTankFightToRetreat);
   m_StateMachine.AddTransitionCondition("Fight", "Pursue", MedTankFightToPursue);
   m_StateMachine.AddTransitionCondition("Retreat", "Fight", MedTankRetreatToFight);
   m_StateMachine.AddTransitionCondition("Retreat", "Pursue", MedTankRetreatToPursue);
   m_StateMachine.AddTransitionCondition("Pursue", "Retreat", MedTankPursueToRetreat);
   m_StateMachine.AddTransitionCondition("Pursue", "Fight", MedTankPursueToFight);

   m_StateMachine.SetCurrentState(1);
   FightMachine->SetCurrentState(0);
   FightMachine->SetObjectToModify((void*)this);
   ShootMachineStill->SetCurrentState(0);
   ShootMachineStill->SetObjectToModify((void*)this);
   ShootMachineStrafe->SetCurrentState(0);
   ShootMachineStrafe->SetObjectToModify((void*)this);
   ShootMachineRetreat->SetCurrentState(0);
   ShootMachineRetreat->SetObjectToModify((void*)this);
}


void CMedTank::FindShotPosition(CObjFrame * frame)
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
   Vec3fScale(&turretAt, &turretAt, 7.5f);
   m_RenderObj.GetFrame(0)->RotateToWorldVector(&turretAt, &turretAt);

   Vec3fAdd(&shotPos, &tankPos, &turretPos);
   Vec3fAdd(&shotPos, &shotPos, &turretAt);

   frame->SetOrientation(&qShot);
   frame->SetPosition(&shotPos);  
}


//////////////////////////////////////////////////////////////////////////
/// Fire the main weapon
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMedTank::FireMainWeapon()
{ 
   // Check to see if the weapon is ready to be fired
   if(g_time > m_MainWeaponCooldown + m_MainWeaponLastFired && m_CurTurretState == STANDBY)
   {
      m_MainWeaponLastFired = g_time;

      m_CurTurretState = RECOIL;

      // Make main weapon sound
      m_pMainGunsSFX->Play3DAudio(40, 10000.0f, 150);
      CVector3f position;
	   GetPosition(&position, 0);
      CVector3f velocity(0.0f, 0.0f, 0.0f);
      m_pMainGunsSFX->UpdateSound(position, velocity);
    
      CPlasmaShot * shot = g_ObjectFactory.CreatePlasmaShot();
      shot->SetUp(ENEMY_SHOT, this, m_ShotDamage);

      // Find the position of the shot
      CObjFrame frame;
      FindShotPosition(&frame);

      int ShotFudge = 0;

      switch(g_GameOptions.GetDifficulty())
      {
         case EASY_DIFFICULTY:
      	{
            ShotFudge = 50;
         }
            break;

         case MEDIUM_DIFFICULTY:
         {
            ShotFudge = 35;
         }
      	   break;
         case HARD_DIFFICULTY:
         {
            ShotFudge = 10;
         }

         default:
            break;
      };

      // fudge the shot on purpose sometimes
      if (rand() % 100 < ShotFudge) 
      {
         float roty = (float) (rand() % 10);
         float rotx = (float) (rand() % 10);

         roty /= 100.0f;
         rotx /= 100.0f;

         if (rand() % 10 < 5)
            roty *= -1.0f;

         if (rand() % 10 < 5)
            rotx *= -1.0f;

         frame.RotateLocalY(roty);
         frame.RotateLocalX(rotx);
      }

      shot->SetFrame(&frame, 0);
      
      CParticleSystem * effect = g_ParticleManager.ObtainSystem();

      // If obtained a particle effect then we use it
      if(effect)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(effect, *(shot->GetFrame(0)->GetPosition()),
         NULL, g_ParticleManager.ENEMY_SHOT, LOD_BOTH);
      }   
 
      // Get a particle effect for the shot if one available
      shot->m_ParticleEffect = g_ParticleManager.ObtainSystem();

      // If obtained a particle effect then we use it
      if(shot->m_ParticleEffect != NULL)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(shot->m_ParticleEffect, *(shot->GetFrame(0)->GetPosition()),
         shot->GetFrame(0)->GetPosition(), g_ParticleManager.ENEMY_PLASMA, LOD_BOTH);
      }  
   }
}

void CMedTank::AimTowardTarget()
{
   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();


   CObjFrame   Frame;

   FindShotPosition(&Frame);

   Ray         ShotTrajectory;
   Sphere      TargetSphere = *pTargetSphere;

   switch (m_CurrentTarget->GetType())
   {
      case OBJ_BUILDING:
      case OBJ_BUILDING2:
      case OBJ_BUILDING3:
      {
         TargetSphere.Center.y = GetSphere()->Center.y;
         TargetSphere.Radius = 20.0f;
         break;
      }

      default:
         break;
   };


   float       Scale = 1.0f;
   float       Dist = DistanceSquare(&TargetSphere.Center, &(GetSphere()->Center));

   if (Dist > 100.0f * 100.0f)
   {
      Scale = 3.0f;
   }
   else if (Dist < 80.0f * 80.0f)
      Scale = .2f;

   TargetSphere.Radius *= Scale;

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (!RayToSphere(&ShotTrajectory, &TargetSphere))
   {

      const Sphere* pEnemySphere = GetSphere();

      CVector3f TargetOrigin;
      CVector3f EnemyOrigin;
      CVector3f EToP;
      CVector3f EnemyLookAt;
      CVector3f Result;

      TargetOrigin   = TargetSphere.Center;
      EnemyOrigin    = pEnemySphere->Center;

      // aim tank on y
      Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);
      Frame.GetForward(&EnemyLookAt);

      EnemyLookAt.y = 0.0f;
      EToP.y = 0.0f;

      Vec3fCrossProduct(&Result, &EnemyLookAt, &EToP);

      if (Result.y == 0.0f)
      {
         // do nothing for now, good aim
      }
      else if (Result.y > 0.0f)
         RBTurnRight(-m_TurnSpeed);
      else
         RBTurnRight(m_TurnSpeed);

      if (Vec3fDotProduct(&EnemyLookAt, &EToP) > 0.0f)
      { 
         float TurretRot = .01f;

         // aim turret on x axis
         Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

         Frame.GetForward(&EnemyLookAt);

         float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

         CVector3f Proj;

         Vec3fScale(&Proj, &EnemyLookAt, Dot);

         Vec3fAdd(&Proj, &EnemyOrigin, &Proj);

         CVector3f Q;

         Vec3fSubtract(&Q, &TargetOrigin, &Proj);

         if (Q.y < 0)
         {
            if (m_TurretPos + TurretRot > .2f)
               TurretRot = 0;

            m_TurretPos += TurretRot;
            RotateLocalX(TurretRot,1);
         }
         else if (Q.y > 0)
         {
            if (m_TurretPos - TurretRot < -.4f)
               TurretRot = 0;
            m_TurretPos -= TurretRot;
            RotateLocalX(-TurretRot,1);
         }

      }
   }
   else
      m_RigidBody->StopTurning();
}


void CMedTank::TurnToAvoid(CGameObject* obj)
{
   const Sphere* pMySphere = GetSphere();
   Sphere OtherSphere = *(obj->GetSphere());

   CVector3f PointToPoint;

   Vec3fSubtract(&PointToPoint, &OtherSphere.Center, &(pMySphere->Center));
   
   float MagOfPointToPoint = Vec3fMagnitude(&PointToPoint);

   float Angle1 = asin((OtherSphere.Radius*1.2f)/MagOfPointToPoint);

   CVector3f MyLookAt;

   const CObjFrame* Frame = GetFrame(0);
   
   Frame->GetForward(&MyLookAt);

   float Angle2 = acos(Vec3fDotProduct(&MyLookAt, &PointToPoint) * MagOfPointToPoint);

   float Angle3 = Angle1 - Angle2;

}

bool CMedTank::AvoidCollision()
{
   CObjFrame Frame = *GetFrame(0);

   Ray Front, FrontLeft, FrontRight, Right, Left, Back, BackLeft, BackRight;
   int GoFwd = 0, GoBack = 0, GoRight = 0, GoLeft = 0;

   Frame.GetPosition(&Front.Origin);
   Front.Direction.x = 0.0f;
   Front.Direction.y = 0.0f;
   Front.Direction.z = 1.0f;
   Frame.RotateToWorldVector(&Front.Direction, &Front.Direction);

   Back.Origin = Front.Origin;
   Back.Direction.x = 0.0f;
   Back.Direction.y = 0.0f;
   Back.Direction.z = -1.0f;
   Frame.RotateToWorldVector(&Back.Direction, &Back.Direction);

   Right.Origin = Front.Origin;
   Right.Direction.x = 1.0f;
   Right.Direction.y = 0.0f;
   Right.Direction.z = 0.0f;
   Frame.RotateToWorldVector(&Right.Direction, &Right.Direction);

   Left.Origin = Front.Origin;
   Left.Direction.x = -1.0f;
   Left.Direction.y = 0.0f;
   Left.Direction.z = 0.0f;
   Frame.RotateToWorldVector(&Left.Direction, &Left.Direction);

   FrontLeft.Origin = Front.Origin;
   FrontLeft.Direction.x = 0.7f;
   FrontLeft.Direction.y = 0.0f;
   FrontLeft.Direction.z = 0.7f;
   Frame.RotateToWorldVector(&FrontLeft.Direction, &FrontLeft.Direction);

   FrontRight.Origin = Front.Origin;
   FrontRight.Direction.x = -0.7f;
   FrontRight.Direction.y = 0.0f;
   FrontRight.Direction.z = 0.7f;
   Frame.RotateToWorldVector(&FrontRight.Direction, &FrontRight.Direction);

   BackLeft.Origin = Front.Origin;
   BackLeft.Direction.x = 0.7f;
   BackLeft.Direction.y = 0.0f;
   BackLeft.Direction.z = -0.7f;
   Frame.RotateToWorldVector(&BackLeft.Direction, &BackLeft.Direction);

   BackRight.Origin = Front.Origin;
   BackRight.Direction.x = -0.7f;
   BackRight.Direction.y = 0.0f;
   BackRight.Direction.z = -0.7f;
   Frame.RotateToWorldVector(&BackRight.Direction, &BackRight.Direction);

   glPushMatrix();
   g_Camera->ApplyTransform();
   glLineWidth(7);
   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   Sphere TempSphere;

   CGameObject* pCurrentObj = GetWhoIsAroundMe(1.07f);
   CGameObject* pFirstGuy = pCurrentObj;
   bool bCollision = false;

   float PlasmaScale = 10.0f;

   if (pCurrentObj != NULL)
   {
      // do I collide with anyone when I try to move?
      do 
      {  
         TempSphere = *(pCurrentObj->GetSphere());

         if (pCurrentObj->GetType() == OBJ_PLASMA)
         {
            TempSphere.Radius *= PlasmaScale;
         }

         // front 
         if (RayToSphere(&Front, &TempSphere))
         {
            GoBack ++;
         }


         if (RayToSphere(&Back, &TempSphere))
         {
            GoFwd ++;
         }

         if (RayToSphere(&Right, &TempSphere))
         {
            GoLeft ++;
         }

         if (RayToSphere(&Left, &TempSphere))
         {
            GoRight ++;
         }

         if (RayToSphere(&FrontRight, &TempSphere))
         {
            GoBack ++;
            GoLeft ++;
         }

         if (RayToSphere(&FrontLeft, &TempSphere))
         {
            GoBack ++;
            GoRight ++;
         }

         if (RayToSphere(&BackLeft, &TempSphere))
         {
            GoFwd ++;
            GoRight ++;
         }

         if (RayToSphere(&BackRight, &TempSphere))
         {
            GoFwd ++;
            GoLeft ++;
         }

         pCurrentObj = pCurrentObj->m_GuyNextToMe;
      } while(pCurrentObj != NULL);
   }

   glPopMatrix();

   ClearWhoIsAroundMe(pFirstGuy);

   if(GoFwd == 0 && GoBack == 0 && GoRight == 0 && GoLeft == 0)
   {
   //   RBMoveForward();
   }
   else if (GoFwd > 2 && GoBack > 2 && GoLeft > 2 && GoRight > 2)
   {
      // stay still
   }
   else
   {
      if (GoFwd < GoBack)
         RBMoveBackward();
      else if (GoBack < GoFwd)
         RBMoveForward();
      else if (GoFwd > 1 && GoFwd == GoBack)
         RBMoveLeft();

      if (GoRight < GoLeft)
         RBMoveLeft();
      else if (GoLeft < GoRight)
         RBMoveRight();
      else if (GoRight > 1 && GoRight == GoLeft)
         RBMoveBackward();
   }

   bool ret = false;

   if (GoFwd > 0 || GoBack > 0 || GoLeft > 0 || GoRight > 0)
      ret = true;

   return ret;
}

void CMedTank::DetermineAggro(void)
{
   // AGGRESSIVENESS for Medium Tank
	m_CurrentAggro = m_BaseAggro;

   // each value below added to or subtracted from base AGGRO

	// player's health affects enemy
	float Health = (float) g_Player.GetShield()/ (float) g_Player.GetMaxShield();

   if (Health > .75f)
      m_CurrentAggro -= .2f;
   else if (Health <= .75f && Health > .5f)
		m_CurrentAggro -= .1f;
	else if (Health <= .5f && Health > .25f)
		m_CurrentAggro += .15f;
	else if (Health <= .25f)
		m_CurrentAggro += .3f;

	// NumAllies = GetNumAllies()
	if (g_AwakeEnemies == 0)
		m_CurrentAggro -= .3f;
	else
		m_CurrentAggro += ((float) g_AwakeEnemies * .05f);

	// number of recent hits effects enemy
	m_CurrentAggro -= m_RecentHitsByPlayer * .1f; // number gets decreased over time in enemy update function

	// unit health affect on aggression
   float UnitHealth = (float) m_HitPoints/ (float) m_MaxHitPoints;

	// above 75%
   if (UnitHealth >= .75f)
      m_CurrentAggro += .2f;
   // above 50%
   else if (UnitHealth < .75f && UnitHealth >= .5f)
      m_CurrentAggro += .1f;
   // below 50%
   else if (UnitHealth < .5f && UnitHealth >= .25f)
      m_CurrentAggro -= .2f;
   else
   // below 25%
      m_CurrentAggro -= .3f;

   if (m_CurrentAggro > 1.0f)
      m_CurrentAggro = 1.0f;
   else if (m_CurrentAggro < 0.0f)
      m_CurrentAggro = 0.0f;

   if (m_DebugAI)
   {
      CVector2f vec;
      char str[255];

      vec.x = 0.0f;
      vec.y = 0.0f;

      TextLoc(vec);

      sprintf(str, "%f", m_CurrentAggro);

      Print(str);
   }
}


bool MedTankFightToRetreat(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   bool Fudge = false;

   int ChaosFactor = rand() % 90;

   if (g_GameOptions.GetDifficulty() == HARD_DIFFICULTY)
      ChaosFactor += 25;

   if (pObj->GetRecentHitsByPlayer() > 1 && pObj->GetCurrentAggro() < ChaosFactor)
      Fudge = true;

   const Sphere* pTargetSphere = pObj->m_CurrentTarget->GetSphere();
   const Sphere* es = pObj->GetSphere();

   float RadiusD = pTargetSphere->Radius + es->Radius;
   RadiusD *= RadiusD;

   Sphere TargetSphere = *pTargetSphere;

   switch (pObj->m_CurrentTarget->GetType())
   {
      case OBJ_BUILDING:
      case OBJ_BUILDING2:
      case OBJ_BUILDING3:
      {
         TargetSphere.Center.y -= TargetSphere.Radius;
         break;
      }

      default:
         break;
   };

   float Dist = DistanceSquare(&TargetSphere.Center, &es->Center) - RadiusD;

   if (Dist < (10.0f * 10.0f))
      Fudge = true;
   
   if (CInputManager::GetInstance()->GetKey(DIK_3))
      Fudge = true;

   return Fudge;
}


bool MedTankFightToPursue(void* pVoidObj)
{
   bool Fudge = false;

   CMedTank* pObj = (CMedTank*) pVoidObj;

   const Sphere* pTargetSphere = pObj->m_CurrentTarget->GetSphere();
   const Sphere* es = pObj->GetSphere();

   float RadiusD = pTargetSphere->Radius + es->Radius;
   RadiusD *= RadiusD;

   Sphere TargetSphere = *pTargetSphere;

   switch (pObj->m_CurrentTarget->GetType())
   {
      case OBJ_BUILDING:
      case OBJ_BUILDING2:
      case OBJ_BUILDING3:
      {
         TargetSphere.Center.y -= TargetSphere.Radius;
         break;
      }

      default:
         break;
   };

   float Dist = DistanceSquare(&TargetSphere.Center, &es->Center) - RadiusD;

   if (Dist > 150.0f * 150.0f)
      Fudge = true;

   return Fudge;
}


bool MedTankRetreatToFight(void* pVoidObj)
{
   bool Fudge = false;

   return Fudge;
}


bool MedTankRetreatToPursue(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;
   bool Fudge = false;

   const Sphere* pTargetSphere = pObj->m_CurrentTarget->GetSphere();
   const Sphere* es = pObj->GetSphere();

   float RadiusD = pTargetSphere->Radius + es->Radius;
   RadiusD *= RadiusD;

   Sphere TargetSphere = *pTargetSphere;

   switch (pObj->m_CurrentTarget->GetType())
   {
      case OBJ_BUILDING:
      case OBJ_BUILDING2:
      case OBJ_BUILDING3:
      {
         TargetSphere.Center.y -= TargetSphere.Radius;
         break;
      }

      default:
         break;
   };

   float Dist = DistanceSquare(&TargetSphere.Center, &es->Center) - RadiusD;

   if (Dist > 100.0f * 100.0f)
      Fudge = true;

   return Fudge;
}


bool MedTankPursueToRetreat(void* pVoidObj)
{
   bool Fudge = false;

   return Fudge;
}


bool MedTankPursueToFight(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;
   bool Fudge = false;

   const Sphere* pTargetSphere = pObj->m_CurrentTarget->GetSphere();
   const Sphere* es = pObj->GetSphere();

   float RadiusD = pTargetSphere->Radius + es->Radius;
   RadiusD *= RadiusD;

   Sphere TargetSphere = *pTargetSphere;

   switch (pObj->m_CurrentTarget->GetType())
   {
      case OBJ_BUILDING:
      case OBJ_BUILDING2:
      case OBJ_BUILDING3:
      {
         TargetSphere.Center.y -= TargetSphere.Radius;
         break;
      }

      default:
         break;
   };

   float Dist = DistanceSquare(&TargetSphere.Center, &es->Center) - RadiusD;

   if (Dist < 150.0f * 150.0f)
      Fudge = true;

   return Fudge;
}

bool MedTankStillToStrafe(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   bool Fudge = false;

   int Aggro = pObj->GetCurrentAggro();

   int StrafeChance = 0;

   switch(g_GameOptions.GetDifficulty()) 
   {
      case EASY_DIFFICULTY:
      {
         StrafeChance = 25;
      }
         break;

      case MEDIUM_DIFFICULTY:
      {
         StrafeChance = 50;
      }
      	break;
      case HARD_DIFFICULTY:
      {
         StrafeChance = 75;
      }

      default:
         break;
   };

   
   if (pObj->m_CurrentTarget == &g_Player)
   {
      if(pObj->InPlayersAim() != 0 && Aggro < (rand() % StrafeChance))
      {
         Fudge = true;
      }
   }

   if (g_time - pObj->m_StrafeTimer > 1.0f)
   {
      pObj->m_StrafeTimer = g_time;

      if (rand() % 100 < (StrafeChance/2.0f))
         Fudge = true;

   }

   return Fudge;
}

bool MedTankStrafeToStill(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   bool Fudge = false;

   int GoStillChance = 0;

   switch(g_GameOptions.GetDifficulty())
   {
      case EASY_DIFFICULTY:
      {
         GoStillChance = 25;
      }
         break;

      case MEDIUM_DIFFICULTY:
      {
         GoStillChance = 50;
      }
      	break;
      case HARD_DIFFICULTY:
      {
         GoStillChance = 75;
      }

      default:
         break;
   };

   if (g_time - pObj->m_StrafeTimer > 1.0f)
   {
      if (rand() % 100 > GoStillChance)
         Fudge = true;
      pObj->m_StrafeTimer = g_time;
   }

   return Fudge;
}


bool MedTankAimToFire(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   bool Fudge = false;

   const Sphere* pTargetSphere = pObj->m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere = *pTargetSphere;

   TempSphere.Radius = pObj->m_CurrentTarget->GetOBB()->y * .5f;

   switch(pObj->m_CurrentTarget->GetType())
   {
      case OBJ_BUILDING:
      case OBJ_BUILDING2:
      case OBJ_BUILDING3:
      {
         TempSphere.Center.y = pObj->GetSphere()->Center.y;
         TempSphere.Radius = 30.0f;
         break;
      }

      default:
         break;
   }

   Ray         ShotTrajectory;
   CObjFrame   Frame;

   pObj->FindShotPosition(&Frame);

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      Fudge = true;

   return Fudge;
}

bool MedTankFireToAim(void* pVoidObj)
{
   bool Fudge = false;

   CMedTank* pObj = (CMedTank*) pVoidObj;

   const Sphere* pTargetSphere = pObj->m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere = *pTargetSphere;

   switch(pObj->m_CurrentTarget->GetType())
   {
      case OBJ_BUILDING:
      case OBJ_BUILDING2:
      case OBJ_BUILDING3:
      {
         TempSphere.Center.y = pObj->GetSphere()->Center.y;
         TempSphere.Radius = 30.0f;
         break;
      }

      default:
         break;
   }
     
   Ray         ShotTrajectory;
   CObjFrame   Frame;

   pObj->FindShotPosition(&Frame);

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (!RayToSphere(&ShotTrajectory, &TempSphere))
      Fudge = true;

   return Fudge;
}

bool MedTankAimToDocile(void* pVoidObj)
{

   CMedTank* pObj = (CMedTank*) pVoidObj;

   bool Fudge = false;

   int GoDocileChance = 0;

   switch(g_GameOptions.GetDifficulty())
   {
      case EASY_DIFFICULTY:
      {
         GoDocileChance = 125;
      }
         break;

      case MEDIUM_DIFFICULTY:
      {
         GoDocileChance = 110;
      }
      	break;
      case HARD_DIFFICULTY:
      {
         GoDocileChance = 35;
      }

      default:
         break;
   };

   if (g_time - pObj->GetAITimer() > 1 + (rand () % 3))
   {
      pObj->ResetAITimer();
      if (pObj->GetCurrentAggro() < rand() % GoDocileChance)
         Fudge = true;
   }

   return Fudge;
}

bool MedTankDocileToAim(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   bool Fudge = false;

   int LeaveDocileChance = 0;

   switch(g_GameOptions.GetDifficulty()) 
   {
      case EASY_DIFFICULTY:
      {
         LeaveDocileChance = 110;
      }
         break;

      case MEDIUM_DIFFICULTY:
      {
         LeaveDocileChance = 90;
      }
      	break;
      case HARD_DIFFICULTY:
      {
         LeaveDocileChance = 75;
      }

      default:
         break;
   };

   if (g_time - pObj->GetAITimer() > 2 + (rand () % 3))
   {
      if (pObj->GetRecentHitsByPlayer() > 0)
         Fudge = true;
      
      if (pObj->GetCurrentAggro() > rand () % LeaveDocileChance)
         Fudge = true;

      pObj->ResetAITimer();
   }

   return Fudge;
}


void MedTankFightOnEnter(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.6f;

      pObj->TextLoc(vec);

      pObj->Print("fight:enter");
   }
}

void MedTankRetreatOnEnter(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.6f;
   
      pObj->TextLoc(vec);  

      pObj->Print("retreat:enter");
   }
}


void MedTankPursueOnEnter(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

//   pObj->TargetEnemy();

   pObj->m_OffsetFromTarget.x = float (rand()%60);

   if (pObj->IsDebugAI())
   {
      CVector2f vec;
      vec.x = 0.0f;
      vec.y = -.6f;
   
      pObj->TextLoc(vec);   

      pObj->Print("pursue:enter");
   }
}

void MedTankStillOnEnter(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.7f;
   
      pObj->TextLoc(vec);   

      pObj->Print("still:enter");
   }
}

void MedTankStrafeOnEnter(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   pObj->m_StrafeTimer = g_time;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.7f;
   
      pObj->TextLoc(vec);   

      pObj->Print("strafe:enter");
   }

   if (rand() % 10 < 5)
      pObj->StrafeDirectionLeft();
   else
      pObj->StrafeDirectionRight();

}

void MedTankAimOnEnter(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;
   
      pObj->TextLoc(vec);
      pObj->Print("aim:enter");
   }
}


void MedTankFireOnEnter(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;
   
      pObj->TextLoc(vec);

      pObj->Print("fire:enter");
   }
}

void MedTankDocileOnEnter(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   pObj->ResetAITimer();

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;
   
      pObj->TextLoc(vec);

      pObj->Print("docile:enter");
   }
}





void MedTankFightOnExit(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.6f;
   
      pObj->TextLoc(vec); 

      pObj->Print("fight:exit");
   }
}


void MedTankRetreatOnExit(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.6f;
   
      pObj->TextLoc(vec);   

      pObj->Print("retreat:exit");
   }
}

void MedTankPursueOnExit(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.6f;
   
      pObj->TextLoc(vec);

      pObj->Print("purse:exit");
   }
}

void MedTankStillOnExit(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.7f;
   
      pObj->TextLoc(vec);

      pObj->Print("still:exit");
   }
}

void MedTankStrafeOnExit(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.7f;
   
      pObj->TextLoc(vec);

      pObj->Print("strafe:exit");
   }
}

void MedTankAimOnExit(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;
   
      pObj->TextLoc(vec);

      pObj->Print("aim:exit");
   }
}

void MedTankFireOnExit(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;
   
      pObj->TextLoc(vec);

      pObj->Print("fire:exit");
   }
}

void MedTankDocileOnExit(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;
   
      pObj->TextLoc(vec);

      pObj->Print("docile:exit");
   }
}


void MedTankFightUpdate(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   pObj->AvoidCollision();

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.6f;
   
      pObj->TextLoc(vec);

      pObj->Print("fight:update");
   }
}

void MedTankRetreatUpdate(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   const Sphere* pTargetSphere = pObj->m_CurrentTarget->GetSphere();
   const Sphere* pEnemySphere = pObj->GetSphere();

   CVector3f TargetOrigin;
   CVector3f EnemyOrigin;
   CVector3f EToP;
   CVector3f EnemyLookAt;
   CVector3f Result;

   TargetOrigin   = pTargetSphere->Center;
   EnemyOrigin    = pEnemySphere->Center;

   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   CObjFrame   Frame;

   pObj->FindShotPosition(&Frame);

   Frame.GetForward(&EnemyLookAt);

   EnemyLookAt.y = 0.0f;
   EToP.y = 0.0f;

   Vec3fCrossProduct(&Result, &EnemyLookAt, &EToP);

  
   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.6f;
   
      pObj->TextLoc(vec); 

      pObj->Print("retreat:update");
   }
   
   if (!pObj->AvoidCollision())
      pObj->RBMoveBackward();
}


void MedTankPursueUpdate(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   const Sphere* pTargetSphere = pObj->m_CurrentTarget->GetSphere();
   const Sphere* pEnemySphere = pObj->GetSphere();

   CVector3f TargetOrigin;
   CVector3f EnemyOrigin;
   CVector3f EToP;
   CVector3f EnemyLookAt;
   CVector3f Result;
   Sphere      TargetSphere = *pTargetSphere;

/*
   if (DistanceSquare(&EnemyOrigin, &TargetSphere.Center) > 400.0f * 400.0f)
   {
      pObj->TargetEnemy();
      TargetSphere = *(pObj->m_CurrentTarget->GetSphere());
   }
*/

   TargetSphere.Center.x += pObj->m_OffsetFromTarget.x;
   EnemyOrigin    = pEnemySphere->Center;

   float       Scale = 1.0f;
   float       RadiusD = TargetSphere.Radius + pObj->GetSphere()->Radius;

   RadiusD *= RadiusD;

   switch (pObj->m_CurrentTarget->GetType())
   {
      case OBJ_BUILDING:
      case OBJ_BUILDING2:
      case OBJ_BUILDING3:
      {
         TargetSphere.Center.y = EnemyOrigin.y + 10.0f;
         break;
      }

      default:
         break;
   };

   TargetOrigin   = TargetSphere.Center;

   TargetOrigin.x += pObj->m_OffsetFromTarget.x;

   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   CObjFrame   Frame;

   pObj->FindShotPosition(&Frame);

   Frame.GetForward(&EnemyLookAt);

   EnemyLookAt.y = 0.0f;
   EToP.y = 0.0f;

   Vec3fCrossProduct(&Result, &EnemyLookAt, &EToP);

   if (pObj->IsDebugAI())
   {
      CVector2f Vec;

      Vec.x = 0.0f;
      Vec.y = -.6f;
   
      pObj->TextLoc(Vec);

      pObj->Print("pursue:update");
   }

   Ray         ShotTrajectory;

   float       Dist = DistanceSquare(&TargetSphere.Center, &pObj->GetSphere()->Center) - RadiusD;

   if (Dist < 90.0f * 90.0f)
   {
      pObj->m_OffsetFromTarget.y = 0.0f;
      pObj->m_OffsetFromTarget.x = 0.0f;
      pObj->m_OffsetFromTarget.z = 0.0f;
   }

   if (Dist > 100.0f * 100.0f)
   {
      Scale = 5.0f;
   }

   TargetSphere.Radius *= Scale;

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (!RayToSphere(&ShotTrajectory, &TargetSphere))
   {
      if (Result.y == 0)
      {
         // do nothing for now, good aim
      }
      else if (Result.y > 0)
         pObj->RBTurnRight(-pObj->m_TurnSpeed);
      
      else
         pObj->RBTurnRight(pObj->m_TurnSpeed);
   }

   if (!pObj->AvoidCollision())
      pObj->RBMoveForward();

}

void MedTankStrafeUpdate(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   const Sphere* pEnemySphere = pObj->GetSphere();
   CVector3f pos = pEnemySphere->Center;

   if (!pObj->AvoidCollision())
   {
      if (pObj->GetStrafeDirection())
         pObj->RBMoveRight();
      else
         pObj->RBMoveLeft();
   }


   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.7f;
   
      pObj->TextLoc(vec);   pObj->Print("strafe:update");
   }
}


void MedTankAimUpdate(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   pObj->AimTowardTarget();

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;
   
      pObj->TextLoc(vec);
      pObj->Print("aim:update");
   }
}


void MedTankStillUpdate(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.7f;
   
      pObj->TextLoc(vec);

      pObj->Print("still:update");
   }
}

void MedTankFireUpdate(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   pObj->FireMainWeapon();

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;
   
      pObj->TextLoc(vec);  

      pObj->Print("fire:update");
   }
}

void MedTankDocileUpdate(void* pVoidObj)
{
   CMedTank* pObj = (CMedTank*) pVoidObj;

   pObj->AimTowardTarget();

   if (pObj->IsDebugAI())
   {
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;
   
      pObj->TextLoc(vec); 

      pObj->Print("docile:update");
   }
}

//////////////////////////////////////////////////////////////////////////
/// Update the location and status of the turret
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMedTank::UpdateTurretState(void)
{
   static float turretpos = 0.0f;   // Current position of turret
   static float velocity = 6.0f;    // Speed of recoil
   static float maxrecoil = -1.0f;  // Max/min distance of recoil
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
            turretpos += (velocity * g_FrameTime);
         else
            turretpos -= (velocity * g_FrameTime);
         
         if (turretpos <= maxrecoil)
         {
            forward = true;
         }
         
         else if (turretpos > 0.0f)
         {
            m_CurTurretState = STANDBY;
            forward = false;
            m_RenderObj.SetPosition(&m_TurretPosition, 1);
            turretpos = 0.0f;
            break;
         }

         m_RenderObj.SetPosition(&m_TurretPosition, 1);
         m_RenderObj.MoveForward(turretpos, 1);
      }
      break;
   }
}

void CMedTank::OnAwaken(void)
{
   // Play the engine sound when enemy is awakened
   GetPosition(&m_CurPosition, 0);
   m_LastPosition = m_CurPosition; 

   // Play the engine sound	when enemy is awakened
   m_pEngineSFX->Play3DAudio(20, 10000.0f, 100);
}