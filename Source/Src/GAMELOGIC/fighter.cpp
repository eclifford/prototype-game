/// \file fighter.cpp
/// Enemy fighter unit
//////////////////////////////////////////////////////////////////////////

//#include <float.h>

#include "gamelogic\fighter.h"
#include "core\oglwindow.h"
#include "input\inputmanager.h"
#include "core\gamemain.h"
#include "math3d\intersection.h"
#include "gamelogic\objectfactory.h"
#include "camera\camera.h"
#include "graphics\texturefonts.h"
#include "graphics\terrain.h"
#include "core\gamemain.h"
#include "gamelogic\bomb.h"
#include "gamelogic\player.h"

#include "utils\mmgr.h"

extern CGameMain GameMain;
extern CPlayer g_Player;
extern CObjectFactory g_ObjectFactory;
extern float g_time;
extern CGameObject* g_CollisionAvoidanceList;
extern CTextureFonts g_TextureFont;
extern int g_AwakeEnemies;
extern float g_FrameTime;
extern CGameOptions g_GameOptions;
extern CBaseCamera* g_Camera;
extern CTextureFonts g_TextureFont;
extern CTerrain *g_pCurrLevel;

CFighter::CFighter()
{

}

CFighter::~CFighter()
{

}

void CFighter::Setup()
{
   m_Active = true;
   m_HitPoints = 50;
   m_MaxHitPoints = 30;
   m_AI = true;
   m_AITimer = g_time;
   m_RiseTime = 1.5f;
   m_DiveTime = 3.0f;
   m_DesiredAltitude = 60.0f;
   m_UpDownSpeed = 30.0f;
   m_BombCoolDown = 0.5f;
   m_BombLastDropped = g_time;


//   m_RigidBody->SetTerrainOffset(100.0f);
   m_MaxRoll = DEG_TO_RAD(45.0f);
   m_MaxPitch = DEG_TO_RAD(45.0f);
   m_Roll = 0.0f;
   m_Pitch = 0.0f;
   m_MaxPitchAngle   = DEG_TO_RAD(3.0f);
   m_MaxYawAngle     = DEG_TO_RAD(5.0f);
   m_MaxRollAngle    = DEG_TO_RAD(1.0f);
   m_WanderToPursueDist =  500.0f * 500.0f;
   m_BeginRiseDist =       100.0f * 100.0f;
   m_DiveAtTargetDist =    150.0f * 150.0f;
   m_Speed = 100.0f;
   m_CurrentTarget = &g_Player;
   m_ShotDamage = 20.0f;

   //m_RigidBody->SetTerrainOffset(100.0f);

   CVector3f ptfront, ptleft, ptright;
   CQuaternion q;

   ptfront.x = 1.0f;
   ptleft.x  = 0.0f;
   ptright.x = 2.0f;

   ptfront.y = 1.0f;
   ptleft.y  = 0.0f;
   ptright.y = 0.0f;

   ptfront.z = 0.0;
   ptleft.z  = 0.0;
   ptright.z = 0.0;

   CObjFrame frame = *GetFrame(0);
   GetOrientationFromPoints(&q, &ptfront, &ptleft, &ptright);
   frame.SetOrientation(&q);

   // Setup thruster
   m_pThruster1 = g_ParticleManager.ObtainSystem();
   if(m_pThruster1)
      g_ParticleManager.CreateSystem(m_pThruster1, m_ThrusterLeftPos, &m_ThrusterLeftPos, g_ParticleManager.FIGHTER_THRUSTER, LOD_BOTH);
      
   // Setup thruster
   m_pThruster2 = g_ParticleManager.ObtainSystem();
   if(m_pThruster2)
      g_ParticleManager.CreateSystem(m_pThruster2, m_ThrusterRightPos, &m_ThrusterRightPos, g_ParticleManager.FIGHTER_THRUSTER, LOD_BOTH);   


   // Play the engine sound when enemy is awakened
   GetPosition(&m_CurPosition, 0);
   m_LastPosition = m_CurPosition; 

   // Play the engine sound	when enemy is awakened
   m_EngineSFX->Play3DAudio(50, 10000.0f, 150);

	m_Dying = false;
	m_DieNow = false;
	m_DyingTime = 0.0f;
}

void CFighter::SetupAI()
{
   m_AITimer = g_time;
   m_OffsetFromPlayer.x = 0.0f;
   m_OffsetFromPlayer.y = 0.0f;
   m_OffsetFromPlayer.z = 0.0f;
   InitStateMachine();
}

void CFighter::UpdateDying(void)
{
	if (m_DieNow)
	{
		Die();
		return;
	}

	m_DyingTime -= g_FrameTime;
	if (m_DyingTime < 0.0f)
	{
		Die();
		return;
	}

	CVector3f up(0.0f, 1.0f, 0.0f), zero(0.0f, 0.0f, 0.0f), dir, pos;
	CVector3f cross;
	CObjFrame frame = *GetFrame(0);
	frame.GetPosition(&pos);

	frame.GetForward(&dir);
	Vec3fCrossProduct(&cross, &up, &dir);
	Vec3fNormalize(&cross, &cross);

	frame.RotateLocalZ(m_SpinDir * PI * 2.0f * g_FrameTime);

	frame.SetPosition(&zero);
	frame.RotateWorldAxis(PI * 0.25f * g_FrameTime, &cross);
	frame.SetPosition(&pos);

	frame.MoveForward(80.0f * g_FrameTime);

	SetFrame(&frame, 0);
}

void CFighter::Update(void)
{
	if(m_HitPoints <= 0 && !m_Dying )
	{
		m_Dying = true;
		m_DyingTime = rand() % 3 + 1.0f + g_FrameTime;
		if(rand() % 100 < 50)
			m_SpinDir = 1.0f;
		else
			m_SpinDir = -1.0f;

       // Setup thruster
      m_Smoke = g_ParticleManager.ObtainSystem();
      if(m_Smoke)
         g_ParticleManager.CreateSystem(m_Smoke , m_WingPos, &m_ThrusterLeftPos, g_ParticleManager.FIGHTER_SMOKE, LOD_NONE);
      
      // Setup thruster
      m_OnFire = g_ParticleManager.ObtainSystem();
      if(m_OnFire)
         g_ParticleManager.CreateSystem(m_OnFire, m_WingPos, &m_ThrusterRightPos, g_ParticleManager.FIGHTER_FIRE, LOD_NONE);   

	}

  if (m_Dying)
		UpdateDying();

   // Update the engine sound
   GetPosition(&m_CurPosition, 0);
   CVector3f Velocity;
   Velocity.x = (m_CurPosition.x - m_LastPosition.x) / g_FrameTime;
   Velocity.y = (m_CurPosition.y - m_LastPosition.y) / g_FrameTime;
   Velocity.z = (m_CurPosition.z - m_LastPosition.z) / g_FrameTime;
   m_EngineSFX->UpdateSound(m_CurPosition, Velocity);
   m_LastPosition = m_CurPosition;

   float mag = Vec3fMagnitude(&Velocity);
   m_EngineSFX->AdjustVolume((unsigned int)abs(mag * 2));

   FindThrusterPositions();
   FindWingPosition();

   if (m_Awake && m_Active && !m_Dying)
   {
      if (m_AI && g_GameOptions.GetAI())
      {
         AdjustAltitude();

         if (m_Awake)
         {
            // update FSM
            m_StateMachine.Update();
         }
         else
         {
            // test to see if enemy sees player
            // or player sees enemy
            // make awake if one sees the other
            // upon awake add it to the awake enemy list
         }
      }
   }

   CGameObject::Update();
}

void CFighter::Collide(const CGameObject *obj)
{
   if (obj == NULL)
	{
      TakeDamage(m_MaxHitPoints, NULL);
		m_DieNow = true;
	}
   else
      switch (obj->GetType())
      {
         case OBJ_BUILDING:
         case OBJ_BUILDING2:
         case OBJ_BUILDING3:   
         {
            TakeDamage(m_MaxHitPoints, NULL);
				m_DieNow = true;
            break;
         }
         
         default:
            break;
      }
}

void CFighter::Die(void)
{
   // Make main weapon sound
   m_ExplosionSFX->Play3DAudio(100, 10000.0f, 200);
   CVector3f position;
   GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_ExplosionSFX->UpdateSound(position, velocity);

   m_EngineSFX->StopAudio();

   if(m_pThruster1)
      m_pThruster1->SetSystemAlive(false);
   if(m_pThruster2)
      m_pThruster2->SetSystemAlive(false);

   if(m_Smoke)
      m_Smoke->SetSystemAlive(false);
   if(m_OnFire)
      m_OnFire->SetSystemAlive(false);

   // Setup thruster
   m_Explosion = g_ParticleManager.ObtainSystem();
   if(m_Explosion)
      g_ParticleManager.CreateSystem(m_Explosion, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.FIGHTER_EXP, LOD_BOTH);   


   GameMain.m_NumEnemiesAlive--;
   g_Player.m_EnemyFightersDestroyed++;
   m_Active = false;
   Sleep();
   RemoveFromQuadtree();
}

void CFighter::Reset(void)
{
   
}

void CFighter::TakeDamage(float damage, CGameObject * guywhohitme)
{
   m_HitPoints -= damage;
}

void CFighter::InitStateMachine()
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
 
   m_StateMachine.AddState("MoveTowardTarget");
   m_StateMachine.AddState("BombTarget");
   m_StateMachine.AddState("Wander");


   // add enter functions for each state
   m_StateMachine.AddOnEnter("BombTarget", FighterBombTargetOnEnter);
   m_StateMachine.AddOnEnter("Wander", FighterWanderOnEnter);
   m_StateMachine.AddOnEnter("MoveTowardTarget", FighterMoveTowardTargetOnEnter);

   // add exit functions for each state
   m_StateMachine.AddOnExit("BombTarget", FighterBombTargetOnExit);
   m_StateMachine.AddOnExit("Wander", FighterWanderOnExit);
   m_StateMachine.AddOnExit("MoveTowardTarget", FighterMoveTowardTargetOnExit);

   // add update functions for each state
   m_StateMachine.AddUpdate("BombTarget", FighterBombTargetUpdate);
   m_StateMachine.AddUpdate("Wander", FighterWanderUpdate);
   m_StateMachine.AddUpdate("MoveTowardTarget", FighterMoveTowardTargetUpdate);

   // set transitions between states
   m_StateMachine.AddTransitionCondition("Wander", "MoveTowardTarget", FighterWanderToMoveTowardTarget);
   m_StateMachine.AddTransitionCondition("MoveTowardTarget", "BombTarget", FighterMoveTowardTargetToBombTarget);
   m_StateMachine.AddTransitionCondition("BombTarget", "Wander", FighterBombTargetToWander);

   m_StateMachine.SetCurrentState(0);
}

bool CFighter::InFrontOfMe(CGameObject* obj)
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
   Frame = *GetFrame(0);

   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

void CFighter::AimYawTowardTarget()
{
   if (AimIsGood())
   {
      //UnRollAfterBank();
      return;
   }

   float RotX, RotY;

   TargetRelativePosition(RotY, RotX);

   RotateLocalY(RotY,0);
}

void CFighter::AimAwayFromTarget()
{
   float RotX, RotY;

   TargetRelativePosition(RotY, RotX);

   RotateLocalY(-RotY,0);
}

bool CFighter::AvoidCollision()
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

   if (GoBack > 0 && GoRight < 1 && GoLeft < 1)
   {
      MoveRight(m_Speed * g_FrameTime * .5f, 0);
   }
   else
   {
      if (GoRight > GoLeft)
         MoveRight(m_Speed * g_FrameTime * .5f, 0);
      else if (GoLeft > GoRight)
         MoveRight(-m_Speed * g_FrameTime * .5f, 0);
   }
   

   bool ret = false;

   if (GoFwd > 0 || GoBack > 0 || GoLeft > 0 || GoRight > 0)
      ret = true;

   return ret;
}


void CFighter::FlyTowardTarget()
{
   if (AvoidCollision())
   {
      AimYawTowardTarget();
      MoveForward(m_Speed * g_FrameTime * .5f, 0);
   }
   else
   {
      AimYawTowardTarget();
      MoveForward(m_Speed * g_FrameTime, 0);
   }
}

void CFighter::AdjustAltitude()
{
   Plane p;
   CVector3f pos;
   CObjFrame frame = *GetFrame(0);

   frame.MoveForward(m_Speed * g_FrameTime);
   
	frame.GetPosition(&pos);

	if (!g_pCurrLevel->GetPlaneAtPoint(pos.x, pos.z, &p))
      return;

	float offset = ClampToPlane(&pos, &p, m_DesiredAltitude);

   float speed;
   
   if (offset == 0.0f)
      speed = 0.0f;
   else
      speed = m_UpDownSpeed * (offset/m_DesiredAltitude);

   if (offset > 0.0f)
   {
      // move up
      MoveUp(speed * g_FrameTime, 0);
   }
   else if (offset < 0.0f)
   {
      // move down
      MoveUp(speed * g_FrameTime, 0);
   }
}

void CFighter::BombsAway()
{
   if (g_time - m_BombLastDropped > m_BombCoolDown)
   {
      m_BombLastDropped = g_time;

      CBomb * bomb = g_ObjectFactory.CreateBomb();
      bomb->SetUp(ENEMY_SHOT, (int)m_ShotDamage, 50.0f, this);
   
      CObjFrame frame = *GetFrame(0);

      bomb->SetFrame(&frame,0);
      
      bomb->MoveUp(-4.0f,0);

      float offset = .01f * (rand() % 100);

      if (rand() %10 < 5)
         bomb->MoveRight(offset, 0);
      else
         bomb->MoveRight(-offset, 0);
   }
}

bool CFighter::AimIsGood()
{
   bool GotShot = false;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere.Radius = pTargetSphere->Radius;
   TempSphere.Center = pTargetSphere->Center;

   TempSphere.Radius = GetOBB()->y * .5f;

   Ray         ShotTrajectory;
   CObjFrame   Frame = *GetFrame(0);

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      GotShot = true;

   return GotShot;
}


void CFighter::TargetRelativePosition(float& RotY, float& RotX)
{
   int TargetPosition = 0;

   // aim toward Target
   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   CVector3f TargetOrigin;
   CVector3f MyOrigin;
   CVector3f PToP;
   CVector3f MyLookAt;
   CVector3f Result;

   TargetOrigin   = pTargetSphere->Center;
   
   CObjFrame Frame;

   Frame = *(GetFrame(0));

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
      if (PToP.z <= 0.0f || fabs(PToP.x/PToP.z) >= m_MaxYawAngle)
         RotY = m_MaxYawAngle * TargetPosition;
      else
         RotY = (float) atan2(PToP.x, PToP.z);

      if (RotY > m_MaxYawAngle)
         RotY = m_MaxYawAngle;
      if (RotY < -m_MaxYawAngle)
         RotY = - m_MaxYawAngle;

      // ascertain angle to tilt turret
      float Mag = Vec3fMagnitude(&PToP2);

      if (Mag != 0.0f)
      {
         float Angle = float(asin(PToP2.y/Mag));

         RotX = -Angle;
      }
      else
         RotX = 0.0f;
   }
   else
      RotX = 0.0f;

   if (RotX > m_MaxPitchAngle)
      RotX = m_MaxPitchAngle;
   if (RotX < -m_MaxPitchAngle)
      RotX = - m_MaxPitchAngle;
}


// enter functions for each state
void FighterBombTargetOnEnter(void* pVoidObj)
{
   CFighter* pObj = (CFighter*) pVoidObj;
   pObj->m_AITimer = g_time;
}

void FighterWanderOnEnter(void* pVoidObj)
{
   CFighter* pObj = (CFighter*) pVoidObj;
}

void FighterMoveTowardTargetOnEnter(void* pVoidObj)
{

}


// exit functions for each state
void FighterBombTargetOnExit(void* pVoidObj)
{
}

void FighterWanderOnExit(void* pVoidObj)
{
}

void FighterMoveTowardTargetOnExit(void* pVoidObj)
{
}


// update functions for each state
void FighterBombTargetUpdate(void* pVoidObj)
{
   CFighter* pObj = (CFighter*) pVoidObj;

   if (pObj->AvoidCollision())
      pObj->MoveForward(pObj->m_Speed * g_FrameTime * .5f,0);
   else
      pObj->MoveForward(pObj->m_Speed * g_FrameTime,0);


   pObj->BombsAway();
}

void FighterWanderUpdate(void* pVoidObj)
{
   CFighter* pObj = (CFighter*) pVoidObj;

   if (pObj->AvoidCollision())
      pObj->MoveForward(pObj->m_Speed * g_FrameTime * .5f,0);
   else
      pObj->MoveForward(pObj->m_Speed * g_FrameTime,0);

}


void FighterMoveTowardTargetUpdate(void* pVoidObj)
{
   CFighter* pObj = (CFighter*) pVoidObj;

   pObj->FlyTowardTarget();
//   pObj->FollowTerrain();
}


// Transitions between states
bool FighterWanderToMoveTowardTarget(void* pVoidObj)
{
   CFighter* pObj = (CFighter*) pVoidObj; 
   bool Fudge = false;

   Point3f MyPoint = pObj->GetSphere()->Center;
   Point3f TargetPoint = pObj->m_CurrentTarget->GetSphere()->Center;

   MyPoint.y = 0.0f;
   TargetPoint.y = 0.0f;

   if (DistanceSquare(&MyPoint, &TargetPoint) > pObj->m_WanderToPursueDist)
      Fudge = true;

   return Fudge;
}

bool FighterMoveTowardTargetToBombTarget(void* pVoidObj)
{
   CFighter* pObj = (CFighter*) pVoidObj; 
   bool Fudge = false;

   Point3f MyPoint = pObj->GetSphere()->Center;
   Point3f TargetPoint = pObj->m_CurrentTarget->GetSphere()->Center;

   MyPoint.y = 0.0f;
   TargetPoint.y = 0.0f;

   if (DistanceSquare(&MyPoint, &TargetPoint) < pObj->m_BeginRiseDist)
      Fudge = true;

   return Fudge;
}


bool FighterBombTargetToWander(void* pVoidObj)
{
   CFighter* pObj = (CFighter*) pVoidObj; 
   bool Fudge = false;
/*
   Point3f MyPoint = pObj->GetSphere()->Center;
   Point3f TargetPoint = pObj->m_CurrentTarget->GetSphere()->Center;

   MyPoint.y = 0.0f;
   TargetPoint.y = 0.0f;

   if (DistanceSquare(&MyPoint, &TargetPoint) < 40.0f * 40.0f)
      Fudge = true;
*/

   if (g_time - pObj->m_AITimer > pObj->m_DiveTime)
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

void CFighter::FindThrusterPositions()
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

   Vec3fScale(&vector, &vector, -4.0f);
   Vec3fScale(&right, &right, 2.0f);
   Vec3fScale(&left, &left, -2.0f);
   Vec3fScale(&up, &up, 1.7f);

   Vec3fAdd(&vector, &Position, &vector);
   Vec3fAdd(&m_ThrusterLeftPos, &vector, &right);
   Vec3fAdd(&m_ThrusterRightPos, &vector, &left);
   Vec3fAdd(&m_ThrusterLeftPos, &m_ThrusterLeftPos, &up);
   Vec3fAdd(&m_ThrusterRightPos, &m_ThrusterRightPos, &up);
}

void CFighter::FindWingPosition()
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

   Vec3fScale(&vector, &vector, -4.0f);

   int chance = rand() % 2;
   if(chance == 0)
       Vec3fScale(&right, &right, 20.0f);
   else
       Vec3fScale(&right, &right, -20.0f);

   Vec3fScale(&up, &up, 0.0f);

   Vec3fAdd(&vector, &Position, &vector);
   Vec3fAdd(&m_WingPos, &vector, &right);
   Vec3fAdd(&m_WingPos, &m_WingPos, &up);
}





















