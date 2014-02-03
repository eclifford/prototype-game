/// \file scout.cpp
/// Enemy scout unit
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\scout.h"
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
extern CAudioManager    g_AudioManager;
extern CGameMain        GameMain;

/////////////////////////////////
/*/ External Globals          /*/ 
/////////////////////////////////

extern float g_time;
extern int   g_AwakeEnemies;
extern float g_FrameTime;

CScout::CScout(CScout& scout) : CEnemy(scout)
{
    SetupAI();
    m_RigidBody = new CRigidBody;
    m_Type = OBJ_SCOUT;
    m_pExplosion = NULL;
    m_pThrusterLeft = NULL;
    m_pThrusterRight = NULL;

    // Set up the sound
    m_pExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_MED], 4);
    m_pEngineSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMY_ENGINE], 0);
    m_pMainGunsSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LASERSHOT], 5);

    m_RigidBody->Initialize(this, 200.0f, m_OBB.x * 2.0f, m_OBB.y * 2.0f, m_OBB.z * 2.0f);
    m_RigidBody->SetTerrainOffset(1.0f);

    m_AwakenByPlayerRange = 350.0f;
    m_AwakenByEnemyRange = 50.0f;
}

CScout::CScout()
{

}

CScout::~CScout()
{

}

void CScout::Setup()
{
   m_CurrentTarget = &g_Player;
   m_AITimer = g_time;
   m_AI = true;
   m_DebugAI = false;
   m_Awake = false;
   m_Active = true;
   m_TurretPos = 0;
   m_HitPoints = 10;
   m_MaxHitPoints = 10;
   m_CurrentAggro = 0;
   m_MainWeaponLastFired = g_time;
   m_RecentHitsByPlayer = 0;
   m_DefensivenessTimer = g_time;
   m_ShotDamage = 10.0f;
   m_MainWeaponCooldown = 0.5f;
   m_CurTurretState = STANDBY;

   // Setup thruster
   m_pThrusterLeft = g_ParticleManager.ObtainSystem();
   if(m_pThrusterLeft)
      g_ParticleManager.CreateSystem(m_pThrusterLeft, m_ThrusterLeftPos, &m_ThrusterLeftPos, g_ParticleManager.KAMI_THRUSTER, LOD_BOTH);
      
   // Setup thruster
   m_pThrusterRight = g_ParticleManager.ObtainSystem();
   if(m_pThrusterRight)
      g_ParticleManager.CreateSystem(m_pThrusterRight, m_ThrusterRightPos, &m_ThrusterRightPos, g_ParticleManager.KAMI_THRUSTER, LOD_BOTH);   

   // Movement Info
   m_MaxVelocity  = 130.0f;    // Max velocity in meters per second
   m_AccelerationTime = 0.5f;  // Time it takes to reach that velocity
   m_TurnSpeed = 20.0f;       // Turn speed
   m_RigidBody->SetParameters(m_MaxVelocity, m_AccelerationTime);

   DebugAIOff();
   
   // Get the turrets position
   m_RenderObj.GetPosition(&m_TurretPosition, 1);
}

//////////////////////////////////////////////////////////////////////////
/// Find the position of the thrusters
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CScout::FindThrusterPositions()
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

//////////////////////////////////////////////////////////////////////////
/// Update the location and status of the turret
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CScout::UpdateTurretState(void)
{
   static float turretpos = 0.0f;   // Current position of turret
   static float velocity = 8.0f;    // Speed of recoil
   static float maxrecoil = -0.5f;  // Max/min distance of recoil
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

void CScout::SetupAI()
{
   m_AITimer = g_time;
   m_OffsetFromTarget.x = 0.0f;
   m_OffsetFromTarget.y = 0.0f;
   m_OffsetFromTarget.z = 0.0f;
   InitStateMachine();
}

void CScout::Update(void)
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

      m_RigidBody->StepIntegrate(g_FrameTime);

      FindThrusterPositions();
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

         // update FSM
         m_StateMachine.Update();
      }

      // Update the engine sound
      GetPosition(&m_CurPosition, 0);
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

void CScout::Collide(const CGameObject *obj)
{

}

void CScout::Die(void)
{
   if (!m_Active)
      return;

   // Make explosion sound
   m_pExplosionSFX->Play3DAudio(40, 10000.0f, 120);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_pExplosionSFX->UpdateSound(position, velocity);

   if(m_pThrusterLeft)
      m_pThrusterLeft->SetSystemAlive(false);
   if(m_pThrusterRight)
      m_pThrusterRight->SetSystemAlive(false);

   m_pEngineSFX->StopAudio();

   // Setup enemy explosion
   explode = g_ParticleManager.ObtainSystem();
   if(explode)
      g_ParticleManager.CreateSystem(explode, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.ENEMYEXP_SMALL, LOD_BOTH);
   Sleep();
   GameMain.m_NumEnemiesAlive --;
   g_Player.m_EnemyScoutsDestroyed++;
   DropPowerup(100.0f);

   m_Active = false;
   RemoveFromQuadtree();
}

void CScout::Reset(void)
{
   m_HitPoints = m_MaxHitPoints;
   m_pThrusterRight = NULL;
   m_pThrusterLeft = NULL;
}

void CScout::TakeDamage(float damage, CGameObject * guywhohitme)
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
      if (m_HitCounter > 3)
         m_CurrentTarget = guywhohitme;
   }

   // Take away the damage
   m_HitPoints -= damage;
}

void CScout::FindShotPosition(CObjFrame * frame)
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

int CScout::TargetRelativePosition()
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

void CScout::AimTowardTarget()
{
   if (AimIsGood())
      return;

   int pos = TargetRelativePosition();

   if (pos > 0)
      RBTurnRight(-m_TurnSpeed);
   else if (pos < 0)
      RBTurnRight(m_TurnSpeed);

   AimTurret();
}

void CScout::AimTurret()
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
   FindShotPosition(&Frame);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
   { 
      float TurretRot = .01f;

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

bool CScout::InDistanceFromPlayer(float Dist)
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

bool CScout::AvoidCollision()
{
   CObjFrame Frame = *GetFrame(0);

   Ray Front, FrontLeft, FrontRight, Right, Left, Back, BackLeft, BackRight;
   int GoFwd = 0, GoBack = 0, GoRight = 0, GoLeft = 0;

   if (InDistanceFromTarget(40))
   {
      int pos = TargetRelativePosition();

      if (pos > 0)
         RBMoveRight();
      else if (pos < 0)
         RBMoveLeft();

      return true;
   }

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


bool CScout::InFrontOfMe(CGameObject* obj)
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

bool CScout::PlayerInFrontOfMe()
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

void CScout::ScanForTarget()
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
         continue;
      }

      if (InFrontOfMe(pCurrentObj))
      {
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

      pCurrentObj = pCurrentObj->m_GuyNextToMe;
   } while(pCurrentObj != NULL);

   ClearWhoIsAroundMe(pFirstGuy);

}


bool CScout::AimIsGood()
{
   bool GotShot = false;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere.Radius = pTargetSphere->Radius * 1.5f;
   TempSphere.Center = pTargetSphere->Center;
  
   Ray         ShotTrajectory;
   CObjFrame   Frame;

   FindShotPosition(&Frame);

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      GotShot = true;

   return GotShot;
}

void CScout::FireMainWeapon()
{ 
   // Check to see if the weapon is ready to be fired
   if(g_time > m_MainWeaponCooldown + m_MainWeaponLastFired && m_CurTurretState == STANDBY)
   {
      m_MainWeaponLastFired = g_time;
      m_CurTurretState = RECOIL;

      CLaser * shot = g_ObjectFactory.CreateLaser();
      shot->SetUp(ENEMY_SHOT, 20, 5.0f, this);

      // Find the position of the shot
      CObjFrame frame;
      FindShotPosition(&frame);

      // Make main weapon sound
      m_pMainGunsSFX->Play3DAudio(20, 10000.0f, 80);
      CVector3f position;
	   GetPosition(&position, 0);
      CVector3f velocity(0.0f, 0.0f, 0.0f);
      m_pMainGunsSFX->UpdateSound(position, velocity);

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
                 
      m_MainWeaponFlash = g_ParticleManager.ObtainSystem();
      if(m_MainWeaponFlash)
      {
         g_ParticleManager.CreateSystem(m_MainWeaponFlash, *(shot->GetFrame(0)->GetPosition()), NULL, g_ParticleManager.LASER_TURRET, LOD_BOTH);

         m_MainWeaponFlash->SetLife(0.1f);
      }

   }
}


void CScout::InitStateMachine(void)
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
   
   m_StateMachine.SetObjectToModify((void*)this);
 
   m_StateMachine.AddState("Aim");
   m_StateMachine.AddState("Fire");
   m_StateMachine.AddState("MoveForward");

   // add enter functions for each state
   m_StateMachine.AddOnEnter("Aim", ScoutAimOnEnter);
   m_StateMachine.AddOnEnter("Fire", ScoutFireOnEnter);
   m_StateMachine.AddOnEnter("MoveForward", ScoutMoveForwardOnEnter);

   // add exit functions for each state
   m_StateMachine.AddOnExit("Aim", ScoutAimOnExit);
   m_StateMachine.AddOnExit("Fire", ScoutFireOnExit);
   m_StateMachine.AddOnExit("MoveForward", ScoutMoveForwardOnExit);

   // add update functions for each state
   m_StateMachine.AddUpdate("Aim", ScoutAimUpdate);
   m_StateMachine.AddUpdate("Fire", ScoutFireUpdate);
   m_StateMachine.AddUpdate("MoveForward", ScoutMoveForwardUpdate);

   // set transitions between states
   m_StateMachine.AddTransitionCondition("Aim", "MoveForward", ScoutAimToMoveForward);
   m_StateMachine.AddTransitionCondition("Aim", "Fire", ScoutAimToFire);
   m_StateMachine.AddTransitionCondition("Fire", "MoveForward", ScoutFireToMoveForward);
   m_StateMachine.AddTransitionCondition("MoveForward", "Aim", ScoutMoveForwardToAim);

   m_StateMachine.SetCurrentState(0);
}


void CScout::MoveTowardTarget()
{
   if (!AvoidCollision())
      RBMoveForward();
}


bool CScout::InDistanceFromTarget(float Dist)
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

bool ScoutAimToMoveForward(void* pVoidObj)
{
   CScout* pObj = (CScout*) pVoidObj;

   return !(pObj->AimIsGood());
}

bool ScoutAimToFire(void* pVoidObj)
{
   CScout* pObj = (CScout*) pVoidObj;

   return pObj->AimIsGood();
}

bool ScoutFireToMoveForward(void* pVoidObj)
{
   return true;
}

bool ScoutMoveForwardToAim(void* pVoidObj)
{
   return true;
}


// state update
void ScoutAimUpdate(void* pVoidObj)
{
   CScout* pObj = (CScout*) pVoidObj;

   float Dist = 60.0f;

   float Scale = pObj->GetSphere()->Radius / g_Player.GetSphere()->Radius;

   Dist = Dist + (Dist * Scale * 2);

   if (!pObj->InDistanceFromTarget(Dist))
      pObj->AimTowardTarget();
}

void ScoutFireUpdate(void* pVoidObj)
{
   CScout* pObj = (CScout*) pVoidObj;

   CVector3f Me, Target;

   Me = pObj->GetSphere()->Center;
   Target = pObj->m_CurrentTarget->GetSphere()->Center;

   Me.y = 0.0f;
   Target.y = 0.0f;

   float Dist = DistanceSquare(&Me, &Target);

   if (g_time - pObj->m_AITimer > .08 && Dist < 100.0f * 100.0f)
   {
      {
         pObj->FireMainWeapon();
         pObj->m_AITimer = g_time;
      }
   }
}

void ScoutMoveForwardUpdate(void* pVoidObj)
{
   CScout* pObj = (CScout*) pVoidObj;
   pObj->MoveTowardTarget();
}


// state on enter
void ScoutAimOnEnter(void* pVoidObj)
{

}

void ScoutFireOnEnter(void* pVoidObj)
{

}

void ScoutMoveForwardOnEnter(void* pVoidObj)
{

}


// state on exit
void ScoutAimOnExit(void* pVoidObj)
{

}

void ScoutFireOnExit(void* pVoidObj)
{

}

void ScoutMoveForwardOnExit(void* pVoidObj)
{

}

void CScout::OnAwaken(void)
{
  // Play the engine sound	when enemy is awakened
  GetPosition(&m_CurPosition, 0);
  m_LastPosition = m_CurPosition; 
  m_pEngineSFX->Play3DAudio(20, 10000.0f, 80);
}