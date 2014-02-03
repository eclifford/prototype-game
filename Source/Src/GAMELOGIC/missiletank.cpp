/// \file missiletank.cpp
/// Enemy missile tank unit
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\missiletank.h"
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


/////////////////////////////////
/*/ External Globals          /*/ 
/////////////////////////////////

extern float g_FrameTime;
extern CPlayer g_Player;
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

CMissileTank::CMissileTank(CMissileTank& MissileTank) : CEnemy(MissileTank)
{
    SetupAI();
    m_RigidBody = new CRigidBody;
    m_Type = OBJ_MSLETANK;

    // Rigid body
	m_RigidBody->Initialize(this, 200.0f, m_OBB.x * 2.0f, m_OBB.y * 2.0f, m_OBB.z * 2.0f);
    m_RigidBody->SetTerrainOffset(1.0f);

    // Setup Audio
    m_pExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_MED], 5);
    m_pMissilesFired = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMYMISSILEFIRED], 4);
    m_pEngineSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMY_ENGINE], 0);

    m_AwakenByPlayerRange = 350.0f;
    m_AwakenByEnemyRange = 50.0f;
}

CMissileTank::CMissileTank()
{

}

CMissileTank::~CMissileTank()
{

}

void CMissileTank::Setup()
{
   m_AI = true;
   m_Awake = false;
   m_Active = true;

   m_CurrentAggro = 0;
   m_MainWeaponLastFired = g_time;
   m_RecentHitsByPlayer = 0;
   m_DefensivenessTimer = g_time;
   m_MainWeaponLastFired = 0;

   m_MissileBayPos = 0.0f;
   m_MissileLastFired = g_time;
   m_MissileLastFired2 = g_time;
   m_MissileSpeed = 100.0f;   
   m_MissileClusterSize = 10;
   m_CurrentMissileBay = 0;
   m_MissileRadius = 50.0f;
   
   m_MissileCoolDown2 = 0.05f;
   m_MissileCoolDown = 4.0f;

   switch(g_GameOptions.GetDifficulty())
   {
      case EASY_DIFFICULTY:
      {
         m_HitPoints = 50.0f;
         m_MaxHitPoints = 50.0f;
         m_NumMissiles = 2;

         m_MissileDamage = 5.0f;
      }
         break;

      case MEDIUM_DIFFICULTY:
      {
         m_HitPoints = 60.0f;
         m_MaxHitPoints = 60.0f;
         m_NumMissiles = 4;

         m_MissileDamage = 10.0f;
      }
      	break;

      case HARD_DIFFICULTY:
      {
         m_HitPoints = 70.0f;
         m_MaxHitPoints = 70.0f;
         m_NumMissiles = 6;

         m_MissileDamage = 15.0f;
      }

      default:
         break;
   };

   DebugAIOff();

   // Setup thruster
   m_pThrusterLeft = g_ParticleManager.ObtainSystem();
   if(m_pThrusterLeft)
      g_ParticleManager.CreateSystem(m_pThrusterLeft, m_ThrusterLeftPos, &m_ThrusterLeftPos, g_ParticleManager.KAMI_THRUSTER, LOD_BOTH, 40.0f, 500.0f);
      
   // Setup thruster
   m_pThrusterRight = g_ParticleManager.ObtainSystem();
   if(m_pThrusterRight)
      g_ParticleManager.CreateSystem(m_pThrusterRight, m_ThrusterRightPos, &m_ThrusterRightPos, g_ParticleManager.KAMI_THRUSTER, LOD_BOTH, 40.0f, 500.0f);   

   // Movement Info
   m_MaxVelocity  = 70.0f;
   m_AccelerationTime = 1.0f;
   m_TurnSpeed = 20.0f;
   m_RigidBody->SetParameters(m_MaxVelocity, m_AccelerationTime);
}

void CMissileTank::SetupAI()
{
   m_AITimer = g_time;
   m_OffsetFromTarget.x = 0.0f;
   m_OffsetFromTarget.y = 0.0f;
   m_OffsetFromTarget.z = 0.0f;
   InitStateMachine();
}


void CMissileTank::Update(void)
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
      FindThrusterPositions();
     

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

void CMissileTank::Collide(const CGameObject *obj)
{

}

void CMissileTank::Die(void)
{
   // Make explosion sound
   m_pExplosionSFX->Play3DAudio(40, 10000.0f, 120);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_pExplosionSFX->UpdateSound(position, velocity);

   m_pEngineSFX->StopAudio();

   if(m_pThrusterLeft)
      m_pThrusterLeft->SetSystemAlive(false);
   if(m_pThrusterRight)
      m_pThrusterRight->SetSystemAlive(false);

   // Setup enemy explosion
   explode = g_ParticleManager.ObtainSystem();
   if(explode)
      g_ParticleManager.CreateSystem(explode, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.ENEMYEXP_SMALL, LOD_BOTH);
   Sleep();
   m_Active = false;
   RemoveFromQuadtree();
   GameMain.m_NumEnemiesAlive --;
   g_Player.m_EnemyMslTanksDestroyed++;
   DropPowerup(25.0f);
}

void CMissileTank::Reset(void)
{
   
}

void CMissileTank::TakeDamage(float damage, CGameObject * guywhohitme)
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

   m_HitPoints -= damage;
}

void CMissileTank::OnAwaken(void)
{
   // Play the engine sound when enemy is awakened
   GetPosition(&m_CurPosition, 0);
   m_LastPosition = m_CurPosition; 

   // Play the engine sound	when enemy is awakened
   m_pEngineSFX->Play3DAudio(20, 10000.0f, 80);
}


bool CMissileTank::AimIsGood()
{
   bool GotShot = false;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere.Radius = pTargetSphere->Radius;
   TempSphere.Center = pTargetSphere->Center;
  
   TempSphere.Center.x += m_OffsetFromTarget.x;

   Ray         ShotTrajectory;
   CObjFrame   Frame;

   float Scale;

   float Dist = DistanceSquare(&(TempSphere.Center), &(GetSphere()->Center));

   if (Dist < 200.0f * 200.0f)
      Scale = .4f;
   else
   {
      Scale = 5.0f;
   }

   TempSphere.Radius *= Scale;

   FindMissilePosition(&Frame, 0.0f, 0.0f);

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      GotShot = true;

   return GotShot;
}

bool CMissileTank::AimIsGoodNowWithLeadTM()
{
   bool GotShot = false;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere.Radius = pTargetSphere->Radius;
   TempSphere.Center = pTargetSphere->Center;
  
   TempSphere.Center.x += m_OffsetFromTarget.x;

   Ray         ShotTrajectory;
   const CObjFrame* Frame = GetFrame(0);

   float Scale;

   float Dist = DistanceSquare(&(TempSphere.Center), &(GetSphere()->Center));

   Scale = 1.0f;

   TempSphere.Radius *= Scale;

   CVector3f TargetVel;
   m_CurrentTarget->GetVelocity(&TargetVel);

   TempSphere.Center.x += TargetVel.x;
   TempSphere.Center.y += TargetVel.y;
   TempSphere.Center.z += TargetVel.z;

   Frame->GetPosition(&(ShotTrajectory.Origin));
   Frame->GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      GotShot = true;

   return GotShot;
}

int CMissileTank::TargetRelativePosition()
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

int CMissileTank::TargetRelativePositionWithLead()
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
//   float Scale;
   CVector3f TargetVel;
   m_CurrentTarget->GetVelocity(&TargetVel);

/*
   if (Dist < 25.0f * 25.0f)
   {
      Scale = 1.0f;
   }
   else if (Dist >= 25.0f * 25.0f && Dist < 50.0f * 50.0f)
   {
      Scale = 1.2f;
   }
   else if (Dist >= 50.0f * 50.0f && Dist < 75.0f * 75.0f)
   {
      Scale = 1.4f;
   }
   else if (Dist >= 75.0f * 75.0f && Dist < 100.0f * 100.0f)
   {
      Scale = 1.6f;
   }
   else
   {
      Scale = 1.8f;
   }
   
   TargetVel.x *= Scale;
   TargetVel.y *= Scale;
   TargetVel.z *= Scale;
*/
   TargetOrigin.x += TargetVel.x;
   TargetOrigin.y += TargetVel.y;
   TargetOrigin.z += TargetVel.z;

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

bool CMissileTank::InDistanceFromPlayer(float Dist)
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

bool CMissileTank::InDistanceFromTarget(float Dist)
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

void CMissileTank::ScanForTarget()
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

bool CMissileTank::PlayerInFrontOfMe()
{
   bool Fudge = false;

   const Sphere* pEnemySphere = GetSphere();
   const Sphere* pTargetSphere = g_Player.GetSphere();

   CVector3f TargetOrigin;
   CVector3f EnemyOrigin;
   CVector3f EToP;
   CVector3f EnemyLookAt;

   TargetOrigin   = pTargetSphere->Center;
   EnemyOrigin    = pEnemySphere->Center;

   CObjFrame   Frame;
   FindMissilePosition(&Frame, 0.0f, 0.0f);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

void CMissileTank::InitStateMachine(void)
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
 
   m_StateMachine.AddState("Pursue");
   m_StateMachine.AddState("Attack");
   m_StateMachine.AddState("Retreat");

   // add enter functions for each state
   m_StateMachine.AddOnEnter("Attack", MissileTankAttackOnEnter);
   m_StateMachine.AddOnEnter("Retreat", MissileTankRetreatOnEnter);
   m_StateMachine.AddOnEnter("Pursue", MissileTankPursueOnEnter);

   // add exit functions for each state
   m_StateMachine.AddOnExit("Attack", MissileTankAttackOnExit);
   m_StateMachine.AddOnExit("Retreat", MissileTankRetreatOnExit);
   m_StateMachine.AddOnExit("Pursue", MissileTankPursueOnExit);


   // add update functions for each state
   m_StateMachine.AddUpdate("Attack", MissileTankAttackUpdate);
   m_StateMachine.AddUpdate("Retreat", MissileTankRetreatUpdate);
   m_StateMachine.AddUpdate("Pursue", MissileTankPursueUpdate);

   // set transitions between states
   m_StateMachine.AddTransitionCondition("Attack", "Retreat", MissileTankAttackToRetreat);
   m_StateMachine.AddTransitionCondition("Attack", "Pursue", MissileTankAttackToPursue);
   m_StateMachine.AddTransitionCondition("Pursue", "Retreat", MissileTankPursueToRetreat);
   m_StateMachine.AddTransitionCondition("Pursue", "Attack", MissileTankPursueToAttack);
   m_StateMachine.AddTransitionCondition("Retreat", "Pursue", MissileTankRetreatToPursue);
   m_StateMachine.AddTransitionCondition("Retreat", "Attack", MissileTankRetreatToAttack);

   m_StateMachine.SetCurrentState(0);
}



bool CMissileTank::AvoidCollision()
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

   CGameObject* pCurrentObj = GetWhoIsAroundMe();
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
//      RBMoveForward();
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



//////////////////////////////////////////////////////////////////////////
/// Fire the tracking Missiles
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMissileTank::FireTrackingMissiles(int NumMissiles)
{ 
   // Check to see if the weapon is ready to be fired
   if(g_time - m_MissileLastFired > m_MissileCoolDown)
   {
   
      if (g_time - m_MissileLastFired2 < m_MissileCoolDown2)
         return;
         
      m_MissileLastFired2 = g_time;  
                   
      // Cycle through each missile pod
      m_CurrentMissileBay++;

      if (m_CurrentMissileBay >= m_NumMissiles)
      {
         // Save time last fired
         m_MissileLastFired = g_time;
      }

      if(m_CurrentMissileBay >= m_NumMissiles)
         m_CurrentMissileBay = 0;



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

      shot->SetUp(ENEMY_SHOT, (int)m_MissileDamage, false, 10.0f, 0.6f, this);
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

void CMissileTank::FindMissilePosition(CObjFrame * frame, float offsetx, float offsety)
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


void CMissileTank::AimTowardTargetNowWithLeadTM()
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

}

bool CMissileTank::InFrontOfMe(CGameObject* obj)
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

   const CObjFrame* Frame = GetFrame(0);

   Frame->GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

void CMissileTank::AimMissiles()
{
   const Sphere* pEnemySphere = GetSphere();
   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   CVector3f TargetOrigin;
   CVector3f EnemyOrigin;
   CVector3f EToP;
   CVector3f EnemyLookAt;

   TargetOrigin   = pTargetSphere->Center;
   EnemyOrigin    = pEnemySphere->Center;


   CVector3f TargetVel;
   m_CurrentTarget->GetVelocity(&TargetVel);

   TargetOrigin.x += TargetVel.x;
   TargetOrigin.y += TargetVel.y;
   TargetOrigin.z += TargetVel.z;

   CObjFrame   Frame;
   FindMissilePosition(&Frame, 0.0f, 0.0f);
   Frame.GetForward(&EnemyLookAt);

   // aim missiles on x axis
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
         if (m_MissileBayPos + TurretRot > .3f)
            TurretRot = 0;

         m_MissileBayPos += TurretRot;
         RotateLocalX(TurretRot,1);
      }
      else if (Q.y > 0)
      {
         if (m_MissileBayPos - TurretRot < -.1f)
            TurretRot = 0;

         m_MissileBayPos -= TurretRot;
         RotateLocalX(-TurretRot,1);
      }
   }
}


void CMissileTank::MoveTowardTarget()
{
   if (!AvoidCollision())
      RBMoveForward();
}


void CMissileTank::AimAwayFromTarget()
{
   if (AimIsGood())
   {
      m_RigidBody->StopTurning();
      return;
   }

   int pos = TargetRelativePosition();

   if (pos > 0)
   {
      RBTurnRight(m_TurnSpeed);
   }
   else if (pos < 0)
   {
      RBTurnRight(-m_TurnSpeed);
   }
}


// on enter functions for each state
void MissileTankAttackOnEnter(void* pVoidObj) 
{ }

void MissileTankRetreatOnEnter(void* pVoidObj) 
{ }

void MissileTankPursueOnEnter(void* pVoidObj) 
{ }


// add exit functions for each state
void MissileTankAttackOnExit(void* pVoidObj) 
{ }

void MissileTankRetreatOnExit(void* pVoidObj) 
{ }

void MissileTankPursueOnExit(void* pVoidObj) 
{ }


// add update functions for each state
void MissileTankAttackUpdate(void* pVoidObj) 
{
   CMissileTank* pObj = (CMissileTank*) pVoidObj;

//   pObj->AimMissiles();

   if (!pObj->AimIsGoodNowWithLeadTM())
      pObj->AimTowardTargetNowWithLeadTM();

      pObj->FireTrackingMissiles((int)pObj->m_NumMissiles);
}

void MissileTankRetreatUpdate(void* pVoidObj) 
{
   CMissileTank* pObj = (CMissileTank*) pVoidObj;


   pObj->AimAwayFromTarget();
   pObj->MoveTowardTarget();

}

void MissileTankPursueUpdate(void* pVoidObj) 
{
   CMissileTank* pObj = (CMissileTank*) pVoidObj;

   if (!pObj->AimIsGoodNowWithLeadTM())
      pObj->AimTowardTargetNowWithLeadTM();

   pObj->MoveTowardTarget();

}


// set transitions between states
bool MissileTankAttackToRetreat(void* pVoidObj) 
{
   CMissileTank* pObj = (CMissileTank*) pVoidObj;
   bool Fudge = false;

   if (pObj->InDistanceFromTarget(100.0f))
      Fudge = true;

   return Fudge;
}

bool MissileTankAttackToPursue(void* pVoidObj) 
{
   CMissileTank* pObj = (CMissileTank*) pVoidObj;
   bool Fudge = false;

   if (!pObj->InDistanceFromTarget(400.0f))
      Fudge = true;

   return Fudge;
}

bool MissileTankPursueToRetreat(void* pVoidObj) 
{
   CMissileTank* pObj = (CMissileTank*) pVoidObj;
   bool Fudge = false;
   return Fudge;
}

bool MissileTankPursueToAttack(void* pVoidObj) 
{
   CMissileTank* pObj = (CMissileTank*) pVoidObj;
   bool Fudge = false;

   if (pObj->InDistanceFromTarget(300.0f))
      Fudge = true;

   return Fudge;
}

bool MissileTankRetreatToPursue(void* pVoidObj) 
{
   CMissileTank* pObj = (CMissileTank*) pVoidObj;
   bool Fudge = false;

   if (!pObj->InDistanceFromTarget(320.0f))
      Fudge = true;

   return Fudge;
}

bool MissileTankRetreatToAttack(void* pVoidObj) 
{
   CMissileTank* pObj = (CMissileTank*) pVoidObj;
   bool Fudge = false;

   return Fudge;
}

//////////////////////////////////////////////////////////////////////////
/// Find the position of the thrusters
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMissileTank::FindThrusterPositions()
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

   Vec3fScale(&vector, &vector, -4.5f);
   Vec3fScale(&right, &right, 0.6f);
   Vec3fScale(&left, &left, -0.6f);
   Vec3fScale(&up, &up, -0.2f);

   Vec3fAdd(&vector, &Position, &vector);
   Vec3fAdd(&m_ThrusterLeftPos, &vector, &right);
   Vec3fAdd(&m_ThrusterRightPos, &vector, &left);
   Vec3fAdd(&m_ThrusterLeftPos, &m_ThrusterLeftPos, &up);
   Vec3fAdd(&m_ThrusterRightPos, &m_ThrusterRightPos, &up);
}
