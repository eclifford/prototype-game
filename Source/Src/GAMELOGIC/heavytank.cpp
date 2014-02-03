/// \file heavytank.cpp
/// Enemy med tank unit
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\heavytank.h"
#include "core\oglwindow.h"
#include "input\inputmanager.h"
#include "core\gamemain.h"
#include "math3d\intersection.h"
#include "gamelogic\objectfactory.h"
#include "camera\camera.h"
#include "core\oglstatemanager.h"
#include "core\gamemain.h"
#include "gamelogic\player.h"
#include "physics\physics.h"

#include "utils\mmgr.h"

extern COpenGLStateManager* pGLSTATE;
extern CPlayer g_Player;
extern CObjectFactory g_ObjectFactory;
extern CGameObject* g_CollisionAvoidanceList;
extern CTextureFonts g_TextureFont;
extern CGameOptions g_GameOptions;
extern CBaseCamera* g_Camera;
extern CAudioManager g_AudioManager;
extern CGameMain GameMain;

extern float g_time;
extern int g_AwakeEnemies;
extern float g_FrameTime;

CHeavyTank::CHeavyTank(CHeavyTank& HeavyTank) : CEnemy(HeavyTank)
{
    SetupAI();
    m_RigidBody = new CRigidBody;
    m_Type = OBJ_HVYTANK;

    // Setup physics
    m_RigidBody->Initialize(this, 800.0f, m_OBB.x * 2.0f, m_OBB.y * 2.0f, m_OBB.z * 2.0f);
    m_RigidBody->SetTerrainOffset(1.0f);

    m_AwakenByPlayerRange = 350.0f;
    m_AwakenByEnemyRange = 50.0f;

    // Setup Audio
    m_pExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_LARGE], 6);
    m_pMainGunsSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMY_TURRET], 5);
    m_pMissilesFired = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMYMISSILEFIRED], 4);
    m_pEngineSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMY_ENGINE], 0);
}

CHeavyTank::CHeavyTank()
{

}

CHeavyTank::~CHeavyTank()
{

}

void CHeavyTank::Setup()
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
   m_MeleeDamage = 60.0f;
   m_ExplosionScale = 3.0f;
   m_CurTurretState = STANDBY;
   m_MissileCoolDown = 4.0f;
   m_MissileLastFired = g_time;
   m_MissileSpeed = 100.0f;
   m_MissileDamage = 10.0f;
   m_MissileClusterSize = 8;
   m_NumMissiles = 400;
   m_CurrentMissileBay = 0;
   m_MissileRadius = 50.0f;

   switch(g_GameOptions.GetDifficulty())
   {
      case EASY_DIFFICULTY:
      {
         m_MeleeDamage = 60.0f;
         m_HitPoints = 200.0f;
         m_MaxHitPoints = 200.0f;
         m_MainWeaponCooldown = 3.0f;
         m_ShotDamage = 50.0f;
      }
         break;

      case MEDIUM_DIFFICULTY:
      {
         m_MeleeDamage = 75.0f;
         m_HitPoints = 300.0f;
         m_MaxHitPoints = 300.0f;
         m_MainWeaponCooldown = 2.0f;
         m_ShotDamage = 60.0f;
      }
      	break;

      case HARD_DIFFICULTY:
      {
         m_MeleeDamage = 90.0f;
         m_HitPoints = 400.0f;
         m_MaxHitPoints = 400.0f;
         m_MainWeaponCooldown = 1.0f;
         m_ShotDamage = 70.0f;
      }

      default:
         break;
   };
 
   // Movement Info
   m_MaxVelocity  = 30.0f;    // Max velocity in meters per second
   m_AccelerationTime = 0.1f; // Time it takes to reach that velocity
   m_TurnSpeed = .01f;        // Turn speed
   m_RigidBody->SetParameters(m_MaxVelocity, m_AccelerationTime);

   DebugAIOff();

   // Get the turrets position
   m_RenderObj.GetPosition(&m_TurretPosition, 1);
}

void CHeavyTank::Update(void)
{
   // Tank is dead
   if(m_HitPoints <= 0.0f)
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

      m_RigidBody->StepIntegrate(g_FrameTime);
      UpdateTurretState();
      OrientMissileBays();

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

         if (m_DebugAI)
         {
            CVector2f vec;
            char str[255];

            vec.x = 0.0f;
            vec.y = 0.0f;

            sprintf(str, "%f", m_CurrentAggro);
            g_TextureFont.SetLocation(vec);
            g_TextureFont.PrintText(str);
         }

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
   {
      ScanForTarget();
   }

   CGameObject::Update();
}

void CHeavyTank::Collide(const CGameObject *obj)
{
   switch(obj->GetType())
   {
      case OBJ_PLAYER:
      case OBJ_FTURRET:
      {
         if (m_Suicidal)
         {
            Die();
            SplashDamage();
         }
			else if (!m_Awake)
			{
				Awaken();
				OnAwaken();
			}

         break;
      }

      default:
         break;
   };
}

void CHeavyTank::Die(void)
{
   // Setup enemy explosion
   m_Active = false;

   // Make explosion sound
   m_pExplosionSFX->Play3DAudio(40, 10000.0f, 120);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_pExplosionSFX->UpdateSound(position, velocity);

   explode = g_ParticleManager.ObtainSystem();
   if(explode)
      g_ParticleManager.CreateSystem(explode, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.HVYTANK_EXP, LOD_BOTH);
   Sleep();
   RemoveFromQuadtree();
   GameMain.m_NumEnemiesAlive --;
   g_Player.m_EnemyHvyTanksDestroyed++;
}

void CHeavyTank::Reset(void)
{
   
}

void CHeavyTank::TakeDamage(float damage, CGameObject * guywhohitme)
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

void CHeavyTank::InitStateMachine(void)
{

   CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;

      g_TextureFont.SetScale(.05f);
      g_TextureFont.SetRowSize(20);
      g_TextureFont.SetLocation(vec);

   // set base aggressiveness
   m_BaseAggro = .4f;

   if (g_GameOptions.GetDifficulty() == EASY_DIFFICULTY)
      m_BaseAggro -= .1f;

   if (g_GameOptions.GetDifficulty() == HARD_DIFFICULTY)
      m_BaseAggro += .2f;

   // add states
   
   m_StateMachine.SetObjectToModify((void*)this);
 
   m_StateMachine.AddState("Attack");
   m_StateMachine.AddState("GRRR");
   m_StateMachine.AddState("Pursue");

   CFSM* AttackMode;

   AttackMode   = m_StateMachine.AddStateMachine("Attack");

   AttackMode->AddState("Aim");
   AttackMode->AddState("Fire");
   AttackMode->AddState("Docile");
   AttackMode->AddTransitionCondition("Aim", "Fire", HeavyTankAimToFire);
   AttackMode->AddTransitionCondition("Fire", "Aim", HeavyTankFireToAim);
   AttackMode->AddTransitionCondition("Aim", "Docile", HeavyTankAimToDocile);
   AttackMode->AddTransitionCondition("Docile", "Aim", HeavyTankDocileToAim);
   AttackMode->AddUpdate("Aim", HeavyTankAimUpdate);
   AttackMode->AddUpdate("Fire", HeavyTankFireUpdate);
   AttackMode->AddUpdate("Docile", HeavyTankDocileUpdate);

   // add enter functions for each state
   m_StateMachine.AddOnEnter("Attack", HeavyTankAttackOnEnter);
   m_StateMachine.AddOnEnter("GRRR", HeavyTankGRRROnEnter);
   m_StateMachine.AddOnEnter("Pursue", HeavyTankPursueOnEnter);

   // add exit functions for each state
   m_StateMachine.AddOnExit("Attack", HeavyTankAttackOnExit);
   m_StateMachine.AddOnExit("GRRR", HeavyTankGRRROnExit);
   m_StateMachine.AddOnExit("Pursue", HeavyTankPursueOnExit);


   // add update functions for each state
   m_StateMachine.AddUpdate("Attack", HeavyTankAttackUpdate);
   m_StateMachine.AddUpdate("GRRR", HeavyTankGRRRUpdate);
   m_StateMachine.AddUpdate("Pursue", HeavyTankPursueUpdate);

   // set transitions between states
   m_StateMachine.AddTransitionCondition("Attack", "GRRR", HeavyTankAttackToGRRR);
   m_StateMachine.AddTransitionCondition("Attack", "Pursue", HeavyTankAttackToPursue);
   m_StateMachine.AddTransitionCondition("Pursue", "GRRR", HeavyTankPursueToGRRR);
   m_StateMachine.AddTransitionCondition("Pursue", "Attack", HeavyTankPursueToAttack);

   m_StateMachine.SetCurrentState(0);
   AttackMode->SetCurrentState(0);
   AttackMode->SetObjectToModify((void*)this);
}



bool CHeavyTank::AvoidCollision()
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

   ClearWhoIsAroundMe(pFirstGuy);

   if(GoFwd == 0 && GoBack == 0 && GoRight == 0 && GoLeft == 0)
   {
  //    RBMoveForward();
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

      if (GoRight < GoLeft)
         RBMoveLeft();
      else if (GoLeft < GoRight)
         RBMoveRight();
   }

   bool ret = false;

   if (GoFwd > 0 || GoBack > 0 || GoLeft > 0 || GoRight > 0)
      ret = true;

   return ret;
}

void CHeavyTank::FireMainWeapon()
{ 
   // Check to see if the weapon is ready to be fired
   if(g_time > m_MainWeaponCooldown + m_MainWeaponLastFired && m_CurTurretState == STANDBY)
   {
      m_MainWeaponLastFired = g_time;
      m_CurTurretState = RECOIL;

      // Play sound of main gun firing
      m_pMainGunsSFX->Play3DAudio(30, 10000.0f, 200);

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
      FindShotPosition(&frame1, 14.0f);

      FindShotPosition(&frame2, 14.0f);
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

         frame1.RotateLocalY(roty);
         frame1.RotateLocalX(rotx);
         frame2.RotateLocalY(roty);
         frame2.RotateLocalX(rotx);
      }

      frame1.MoveRight(1.0f);
      frame2.MoveRight(-1.0f);

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

void CHeavyTank::DetermineAggro(void)
{
   // AGGRESSIVENESS for Medium Tank
	m_CurrentAggro = m_BaseAggro;

   // each value below added to or subtracted from base AGGRO

	// player's health affects enemy
	float Health = (float) g_Player.GetShield()/ (float) g_Player.GetMaxShield();

   if (Health > .75f)
      m_CurrentAggro -= .1f;
   else if (Health <= .75f && Health > .5f)
		m_CurrentAggro -= .05f;
	else if (Health <= .5f && Health > .25f)
		m_CurrentAggro += .15f;
	else if (Health <= .25f)
		m_CurrentAggro += .3f;

	// NumAllies = GetNumAllies()
	if (g_AwakeEnemies == 0)
		m_CurrentAggro += .2f;

	// number of recent hits effects enemy
	m_CurrentAggro += m_RecentHitsByPlayer * .1f; // number gets decreased over time in enemy update function

	// unit health affect on aggression
   float UnitHealth = (float) m_HitPoints/ (float) m_MaxHitPoints;

   // below 50%
   if (UnitHealth < .5f && UnitHealth >= .25f)
      m_CurrentAggro += .3f;
   else if (UnitHealth < .25f && UnitHealth >= .15f)
   // below 25%
      m_CurrentAggro += .5f;
   else if (UnitHealth < .15f)
      m_CurrentAggro += 3.0f;

   if (m_CurrentAggro > 1.0f)
      m_CurrentAggro = 1.0f;
   else if (m_CurrentAggro < 0.0f)
      m_CurrentAggro = 0.0f;
}

void CHeavyTank::FindShotPosition(CObjFrame * frame, float ForwardScale)
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
   Vec3fScale(&turretAt, &turretAt, ForwardScale);
   m_RenderObj.GetFrame(0)->RotateToWorldVector(&turretAt, &turretAt);

   Vec3fAdd(&shotPos, &tankPos, &turretPos);
   Vec3fAdd(&shotPos, &shotPos, &turretAt);

   frame->SetOrientation(&qShot);
   frame->SetPosition(&shotPos);  
}

bool CHeavyTank::AimIsGood()
{
   bool GotShot = false;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere.Radius = m_CurrentTarget->GetOBB()->y * .5f;
   TempSphere.Center = pTargetSphere->Center;
  
   TempSphere.Center.x += m_OffsetFromTarget.x;

   Ray         ShotTrajectory;
   CObjFrame   Frame;

   float Scale = 1.0f;

   float RadiusD = pTargetSphere->Radius + GetSphere()->Radius;
   RadiusD *= RadiusD;

   float Dist = DistanceSquare(&pTargetSphere->Center, &GetSphere()->Center) - RadiusD;

   if (Dist < 130.0f * 130.0f)
      Scale = .2f;
/*   else
   {
      Scale = 5.0f;
   }
*/

   FindShotPosition(&Frame, 14.0f);

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      GotShot = true;

   return GotShot;
}

bool CHeavyTank::AimIsGoodNowWithLeadTM()
{
   bool GotShot = false;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere.Radius = pTargetSphere->Radius;
   TempSphere.Center = pTargetSphere->Center;
  
   TempSphere.Center.x += m_OffsetFromTarget.x;

   Ray         ShotTrajectory;
   CObjFrame   Frame;

   CVector3f TargetVel;
   m_CurrentTarget->GetVelocity(&TargetVel);

   TempSphere.Center.x += TargetVel.x * .5f;
   TempSphere.Center.y += TargetVel.y * .5f;
   TempSphere.Center.z += TargetVel.z * .5f;

   FindShotPosition(&Frame, 14.0f);

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      GotShot = true;

   return GotShot;
}

int CHeavyTank::TargetRelativePosition()
{
   int TargetPosition = 0;

   // aim toward Target
   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();
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

int CHeavyTank::TargetRelativePositionWithLead()
{
   int TargetPosition = 0;

   // aim toward Target
   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();
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

   float Dist = DistanceSquare(&EnemyOrigin, &TargetOrigin);

   CVector3f TargetVel;
   m_CurrentTarget->GetVelocity(&TargetVel);

   TargetOrigin.x += TargetVel.x * .5f;
   TargetOrigin.y += TargetVel.y * .5f;
   TargetOrigin.z += TargetVel.z * .5f;

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

bool CHeavyTank::InDistanceFromTarget(float Dist)
{
   bool bReturnVal = false;

   Dist = Dist * Dist;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();
   const Sphere* pMySphere = GetSphere();

   float RadiusD = pTargetSphere->Radius + pMySphere->Radius;
   RadiusD *= RadiusD;

   float ActualDist = DistanceSquare(&pTargetSphere->Center, &pMySphere->Center) - RadiusD;

   if (ActualDist < Dist)
      bReturnVal = true;

   return bReturnVal;
}

bool CHeavyTank::InDistanceFromPlayer(float Dist)
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

void CHeavyTank::ScanForTarget()
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
         (PlayerInFrontOfMe() || InDistanceFromPlayer(50.0f)))
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

      float RadiusD = pMeSphere->Radius + pCurrentObj->GetSphere()->Radius;

      RadiusD *= RadiusD;

      float Dist = DistanceSquare(&pt1, &pt2) - RadiusD;

      if (Dist > m_AwakenByEnemyRange * m_AwakenByEnemyRange)
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

bool CHeavyTank::PlayerInFrontOfMe()
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
   FindShotPosition(&Frame, 14.0f);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &PlayerOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

bool CHeavyTank::InFrontOfMe(CGameObject* obj)
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
   FindShotPosition(&Frame, 14.0f);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

void CHeavyTank::AimTurret()
{
   const Sphere* pEnemySphere = GetSphere();
   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   CVector3f TargetOrigin;
   CVector3f EnemyOrigin;
   CVector3f EToP;
   CVector3f EnemyLookAt;

   TargetOrigin   = pTargetSphere->Center;
   EnemyOrigin    = pEnemySphere->Center;

   CObjFrame   Frame;
   FindShotPosition(&Frame, 14.0f);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
   { 
      float TurretRot = .001f;

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
         if (m_TurretPos - TurretRot < -.3f)
            TurretRot = 0;

         m_TurretPos -= TurretRot;
         RotateLocalX(-TurretRot,1);
      }
   }
}

void CHeavyTank::MoveTowardTarget()
{
   if (!AvoidCollision())
      RBMoveForward();
}

void CHeavyTank::SuicideRun()
{
   RBMoveForward();
}

void CHeavyTank::AimTowardTarget()
{
   if (AimIsGood())
   {
      m_RigidBody->StopTurning();
      return;
   }

   int pos = TargetRelativePosition();

   if (pos > 0)
   {
      RBTurnRight(-m_TurnSpeed);
   }
   else if (pos < 0)
   {
      RBTurnRight(m_TurnSpeed);
   }

   AimTurret();
}

void CHeavyTank::AimTowardTargetNowWithLeadTM()
{
   if (AimIsGoodNowWithLeadTM())
   {
      m_RigidBody->StopTurning();
      return;
   }

   int pos = TargetRelativePositionWithLead();

   if (pos > 0)
   {
      RBTurnRight(-m_TurnSpeed);
   }
   else if (pos < 0)
   {
      RBTurnRight(m_TurnSpeed);
   }

   AimTurret();
}

void CHeavyTank::SplashDamage()
{
   Sphere MySphere = *(GetSphere());

   CGameObject* pCurrentObj = GetWhoIsAroundMe();
   CGameObject* pFirstGuy = pCurrentObj;
   bool bCollision = false;

   MySphere.Radius *= m_ExplosionScale;

   if (pCurrentObj != NULL)
   {
      // who is caught in the explosion?
      do 
      {  
         if (pCurrentObj->IsActive())
         {
            if (SphereToSphere(&MySphere, pCurrentObj->GetSphere()))
            {
                int Type = pCurrentObj->GetType();
            
                if (Type == OBJ_MEDTANK || Type == OBJ_KAMITANK || Type == OBJ_SCOUT || Type == OBJ_FTURRET || Type == OBJ_ETURRET ||
                    Type == OBJ_HVYTANK || Type == OBJ_CONVOY || Type == OBJ_MSLETANK || Type == OBJ_PLAYER || Type == OBJ_BUILDING || Type == OBJ_BUILDING2 || Type == OBJ_BUILDING3)
                {
                    pCurrentObj->TakeDamage(m_MeleeDamage, this);
                }
            
            }
         }
      
         pCurrentObj = pCurrentObj->m_GuyNextToMe;
      } while(pCurrentObj != NULL);
   }

   ClearWhoIsAroundMe(pFirstGuy);
}

void CHeavyTank::SetupAI()
{
   m_Suicidal = false;
   m_AITimer = g_time;
   m_OffsetFromTarget.x = 0.0f;
   m_OffsetFromTarget.y = 0.0f;
   m_OffsetFromTarget.z = 0.0f;
   InitStateMachine();
}

void HeavyTankAimOnEnter(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;
   pObj->ResetAITimer();
}

void HeavyTankFireOnEnter(void* pVoidObj)
{ 
}

void HeavyTankDocileOnEnter(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;
   pObj->ResetAITimer();
}

void HeavyTankPursueOnEnter(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;
   pObj->m_OffsetFromTarget.x = float (rand()%60);
}

void HeavyTankAttackOnEnter(void* pVoidObj)
{
}

void HeavyTankGRRROnEnter(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;
   pObj->m_Suicidal = true;
}


void HeavyTankAimUpdate(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;
   pObj->AimTowardTargetNowWithLeadTM();

   if(pObj->m_DebugAI)
   { 
      CVector2f vec;
      vec.x = 0.0f;
      vec.y = -.8f;
      g_TextureFont.SetLocation(vec);  

      g_TextureFont.PrintText("aim:update");
   }
}

void HeavyTankFireUpdate(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;
   pObj->FireMainWeapon();
   pObj->FireMissiles((int)pObj->m_MissileClusterSize);

   if(pObj->m_DebugAI)
   { 
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.8f;
   
      g_TextureFont.SetLocation(vec);  

      g_TextureFont.PrintText("fire:update");
   }
}

void HeavyTankDocileUpdate(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;
   pObj->AimTowardTarget();
   if(pObj->m_DebugAI)
   { 
      CVector2f vec;
      vec.x = 0.0f;
      vec.y = -.8f;
   
      g_TextureFont.SetLocation(vec);  

      g_TextureFont.PrintText("docile:update");
   }
}

void HeavyTankPursueUpdate(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;

   pObj->AimTowardTarget();
   pObj->MoveTowardTarget();

   if(pObj->m_DebugAI)
   { 
         CVector2f vec;
      vec.x = 0.0f;
      vec.y = -.6f;
   
      g_TextureFont.SetLocation(vec);
      g_TextureFont.PrintText("pursue:update");
   }
}

void HeavyTankAttackUpdate(void* pVoidObj)
{ 
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;

   pObj->AvoidCollision();

   if(pObj->m_DebugAI)
   { 
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.6f;
   
      g_TextureFont.SetLocation(vec);

      g_TextureFont.PrintText("attack:update");
   }
}

void HeavyTankGRRRUpdate(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;
   pObj->AimTowardTarget();
   pObj->SuicideRun();

   if(pObj->m_DebugAI)
   { 
      CVector2f vec;

      vec.x = 0.0f;
      vec.y = -.6f;
   
      g_TextureFont.SetLocation(vec);

      g_TextureFont.PrintText("grrr:update");
   }
}


void HeavyTankAimOnExit(void* pVoidObj)
{ 
}

void HeavyTankFireOnExit(void* pVoidObj)
{ 
}

void HeavyTankDocileOnExit(void* pVoidObj)
{ 
}

void HeavyTankPursueOnExit(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;
   pObj->m_OffsetFromTarget.x = 0.0f;
}

void HeavyTankAttackOnExit(void* pVoidObj)
{ 
}

void HeavyTankGRRROnExit(void* pVoidObj)
{ 
}

bool HeavyTankAttackToGRRR(void* pVoidObj)
{
   bool Fudge = false;
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;

   if (pObj->GetCurrentAggro() > 95)
      Fudge = true;

   return Fudge;
}

bool HeavyTankAttackToPursue(void* pVoidObj)
{
   bool Fudge = false;
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;

   if (!pObj->InDistanceFromTarget(130))
      Fudge = true;

   return Fudge;
}

bool HeavyTankPursueToGRRR(void* pVoidObj)
{
   bool Fudge = false;
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;

   if (pObj->GetCurrentAggro() > 95)
      Fudge = true;

   return Fudge;
}

bool HeavyTankPursueToAttack(void* pVoidObj)
{ 
   bool Fudge = false;
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;

   if (pObj->InDistanceFromTarget(129))
      Fudge = true;

   return Fudge;
}

bool HeavyTankAimToFire(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;

   return pObj->AimIsGoodNowWithLeadTM();
}

bool HeavyTankAimToDocile(void* pVoidObj)
{
   bool Fudge = false;
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;

   if (g_time - pObj->GetAITimer() > 6)
   {
      if (pObj->GetCurrentAggro() < rand() % 100)
         Fudge = true;
   }

   return Fudge;
}

bool HeavyTankDocileToAim(void* pVoidObj)
{
   bool Fudge = false;
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;

   if (g_time - pObj->GetAITimer() > 1)
   {
      if (pObj->GetCurrentAggro() > rand() % 100)
         Fudge = true;
   }

   return Fudge;
}

bool HeavyTankFireToAim(void* pVoidObj)
{
   CHeavyTank* pObj = (CHeavyTank*) pVoidObj;

   return (!pObj->AimIsGoodNowWithLeadTM());
}

//////////////////////////////////////////////////////////////////////////
/// Update the location and status of the turret
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CHeavyTank::UpdateTurretState(void)
{
   static float turretpos = 0.0f;   // Current position of turret
   static float velocity = 6.0f;    // Speed of recoil
   static float maxrecoil = -2.5f;  // Max/min distance of recoil
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

//////////////////////////////////////////////////////////////////////////
/// Fire the Missiles
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CHeavyTank::FireMissiles(int NumMissiles)
{ 
 // Check to see if the weapon is ready to be fired
   if(g_time >= (m_MissileCoolDown + m_MissileLastFired))
   { 
      m_NumMissiles -= NumMissiles;

      if(m_NumMissiles < 0)
      {
         NumMissiles += (int)m_NumMissiles;
         m_NumMissiles = 0;
      }
      
      m_MissileLastFired = g_time;

      float offsetx, offsety;
      
      for(int i = 0; i < NumMissiles; i++)
      {
         CMissile * shot = g_ObjectFactory.CreateMissile();

         m_pMissilesFired->Play3DAudio(20, 10000.0f, 70);
         CVector3f position;
	      GetPosition(&position, 0);
         CVector3f velocity;
         velocity.x = 0.0f;
         velocity.y = 0.0f;
         velocity.z = 0.0f;
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
     
         FindMissilePosition(&frame, offsetx, offsety);
         shot->SetFrame(&frame, 0);
         shot->SetUp(ENEMY_SHOT, (int)m_MissileDamage, i, m_MissileRadius, this);
      
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

//////////////////////////////////////////////////////////////////////////
/// Find the point where a shot will come out
/// 
/// Input:     CObjFrame * frame - frame to copy to shot
//             float offsetx 
//             float offsety
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CHeavyTank::FindMissilePosition(CObjFrame * frame, float offsetx, float offsety)
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
   Vec3fScale(&turretAt, &turretAt, 0.5f);
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

void CHeavyTank::OrientMissileBays(void)
{
   //SetFrame(m_RenderObj.GetFrame(1), 2);
   CQuaternion Turret;
   m_RenderObj.GetFrame(1)->GetOrientation(&Turret);
   m_RenderObj.GetFrame(2)->SetOrientation(&Turret);
   //MoveForward(-2.5f, 2);
   //MoveUp(-1.0f, 2);
}

void CHeavyTank::OnAwaken(void)
{
   // Play the engine sound when enemy is awakened
   GetPosition(&m_CurPosition, 0);
   m_LastPosition = m_CurPosition; 

   // Play the engine sound when enemy is awakened
   m_pEngineSFX->Play3DAudio(20, 10000.0f, 120);
}
























