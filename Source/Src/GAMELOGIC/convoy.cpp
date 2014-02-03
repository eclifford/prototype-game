/// \file convoy.cpp
/// The Convoy
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/convoy.h"
#include "level editor/editormain.h"
#include "math3d\intersection.h"
#include "core\gamemain.h"
#include "physics\physics.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ Global Variables          /*/ 
/////////////////////////////////

extern float g_FrameTime;
extern float g_time;
extern CGameMain GameMain;

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CParticleManager g_ParticleManager;
extern CGameObject* g_CollisionAvoidanceList;

CConvoy::CConvoy(CConvoy& Convoy) : CFriendly(Convoy)
{
    // Setup the object type
    m_Type = OBJ_CONVOY;

    // Set the convoy state
    m_CurConvoyState = CONVOY_FINE;

    // Setup the physics
    m_RigidBody = new CRigidBody; 
    m_RigidBody->Initialize(this, 2000.0f, m_OBB.x * 2.0f, m_OBB.y * 2.0f, m_OBB.z * 2.0f);
    m_RigidBody->SetTerrainOffset(1.0f);

    // Setup the sounds
    m_pEngineSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_CONVOYENGINE], 5);
    m_pExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_LARGE], 6);

}

//////////////////////////////////////////////////////////////////////////
/// Constructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CConvoy::CConvoy()
{
    m_Waypoints = 0;
}

//////////////////////////////////////////////////////////////////////////
/// Destructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CConvoy::~CConvoy()
{
   delete [] m_Waypoints;
    m_Waypoints = 0;
}

//////////////////////////////////////////////////////////////////////////
/// Setup the convoy
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CConvoy::Setup()
{
   m_Active = true;
   tempbool1 = true;
   tempbool2 = true;
   tempbool3 = true;
   m_HitPoints = m_MaxHitPoints = 5000;

   // Movement Info
   m_MaxVelocity  = 30.0f;       // Max velocity in meters per second
   m_AccelerationTime = 3.0f;    // Time it takes to reach that velocity
   m_TurnSpeed = 5.0f;           // Turn speed
   m_RigidBody->SetParameters(m_MaxVelocity, m_AccelerationTime);

   m_CurConvoyMoveState = CONVOY_MOVING;

   CVector3f TempVec;

   m_Waypoints = new Waypoint[23];

   // Waypoint information
   TempVec.x = 933.0f; TempVec.y = 0.0f; TempVec.z = 958.15f;
   m_Waypoints[0].m_Frame.SetPosition(&TempVec);

   TempVec.x = 848.78f; TempVec.y = 0.0f; TempVec.z = 784.57f;
   m_Waypoints[1].m_Frame.SetPosition(&TempVec);

   TempVec.x = 848.38f; TempVec.y = 0.0f; TempVec.z = 822.15f;
   m_Waypoints[2].m_Frame.SetPosition(&TempVec);

   TempVec.x = 874.78f; TempVec.y = 0.0f; TempVec.z = 707.57f;
   m_Waypoints[3].m_Frame.SetPosition(&TempVec);

   TempVec.x = 881.52f; TempVec.y = 0.0f; TempVec.z = 592.30f;
   m_Waypoints[4].m_Frame.SetPosition(&TempVec);

   TempVec.x = 785.57f; TempVec.y = 0.0f; TempVec.z = 512.92f;
   m_Waypoints[5].m_Frame.SetPosition(&TempVec);

   TempVec.x = 798.40f; TempVec.y = 0.0f; TempVec.z = 368.26f;
   m_Waypoints[6].m_Frame.SetPosition(&TempVec);

   TempVec.x = 815.51f; TempVec.y = 0.0f; TempVec.z = 207.88f;
   m_Waypoints[7].m_Frame.SetPosition(&TempVec);

   TempVec.x = 801.42f; TempVec.y = 0.0f; TempVec.z = -3.68f;
   m_Waypoints[8].m_Frame.SetPosition(&TempVec);

   TempVec.x = 670.5f; TempVec.y = 0.0f; TempVec.z = -97.54f;
   m_Waypoints[9].m_Frame.SetPosition(&TempVec);

   TempVec.x = 659.57f; TempVec.y = 0.0f; TempVec.z = -250.80f;
   m_Waypoints[10].m_Frame.SetPosition(&TempVec);

   TempVec.x = 562.22f; TempVec.y = 0.0f; TempVec.z = -420.95f;
   m_Waypoints[11].m_Frame.SetPosition(&TempVec);

   TempVec.x = 553.57f; TempVec.y = 0.0f; TempVec.z = -609.40f;
   m_Waypoints[12].m_Frame.SetPosition(&TempVec);

   TempVec.x = 407.52f; TempVec.y = 0.0f; TempVec.z = -669.00f;
   m_Waypoints[13].m_Frame.SetPosition(&TempVec);

   TempVec.x = 235.57f; TempVec.y = 0.0f; TempVec.z = -488.80f;
   m_Waypoints[14].m_Frame.SetPosition(&TempVec);

   TempVec.x = 160.0f; TempVec.y = 0.0f; TempVec.z = -219.00f;
   m_Waypoints[15].m_Frame.SetPosition(&TempVec);

   TempVec.x = -76.0f; TempVec.y = 0.0f; TempVec.z = -46.00f;
   m_Waypoints[16].m_Frame.SetPosition(&TempVec);

   TempVec.x = -284.0f; TempVec.y = 0.0f; TempVec.z = 37.00f;
   m_Waypoints[17].m_Frame.SetPosition(&TempVec);
  
   TempVec.x = -416.0f; TempVec.y = 0.0f; TempVec.z = -65.00f;
   m_Waypoints[18].m_Frame.SetPosition(&TempVec);
   
   TempVec.x = -596.0f; TempVec.y = 0.0f; TempVec.z = -125.00f;
   m_Waypoints[19].m_Frame.SetPosition(&TempVec);
  
   TempVec.x = -792.0f; TempVec.y = 0.0f; TempVec.z = -105.00f;
   m_Waypoints[20].m_Frame.SetPosition(&TempVec);

   TempVec.x = -753.0f; TempVec.y = 0.0f; TempVec.z = -230.00f;
   m_Waypoints[21].m_Frame.SetPosition(&TempVec);

   m_AimTimer = g_time;
   m_AimWait = 0.3f;

   m_LastWaypoint = 21;
   m_CurrentWaypointIndex = 0;

   // Get the position of the object
   GetPosition(&m_CurPosition, 0);
   m_LastPosition = m_CurPosition; 

   // Play Engine sound
   m_pEngineSFX->Play3DAudio(30.0f, 10000.0f, 150);
}

//////////////////////////////////////////////////////////////////////////
/// Update
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CConvoy::Update(void)
{
   if(m_HitPoints <= 0)
   {
      Die();
      return;
   }

   ConvoyTopMid = *GetFrame(0);
   ConvoyTopMid.MoveUp(5.0f);
  
   if (IsAtCurrentWaypoint())
      NextWaypoint();

   // Update the ambient engine sound
   GetPosition(&m_CurPosition, 0);
   CVector3f Velocity;
   Velocity.x = (m_CurPosition.x - m_LastPosition.x) / g_FrameTime;
   Velocity.y = (m_CurPosition.y - m_LastPosition.y) / g_FrameTime;
   Velocity.z = (m_CurPosition.z - m_LastPosition.z) / g_FrameTime;
   m_pEngineSFX->UpdateSound(m_CurPosition, Velocity);
   m_LastPosition = m_CurPosition;

   // Update the physics
   m_RigidBody->StepIntegrate(g_FrameTime);

   // Temp Moving
   if(m_CurConvoyMoveState == CONVOY_MOVING)
      MoveTowardWaypoint();

   // Update 
   CGameObject::Update();
}

//////////////////////////////////////////////////////////////////////////
/// On collision
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CConvoy::Collide(const CGameObject *obj)
{

}

//////////////////////////////////////////////////////////////////////////
/// Die and remove from the game
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CConvoy::Die(void)
{
   m_Active = false;
   RemoveFromQuadtree();

   if(m_pFire)
      m_pFire->SetSystemAlive(false);
   if(m_pSmoke)
      m_pSmoke->SetSystemAlive(false);

   if(tempbool1)
   {
      if(GameMain.m_CurrentConvoyCom == GameMain.COM_CONVOYNONE)
         GameMain.m_CurrentConvoyCom = GameMain.COM_CONVOYDEAD;
      tempbool1 = false;
   }

   GameMain.m_ConvoysDead++;

   // Make explosion sound
   m_pExplosionSFX->Play3DAudio(40, 10000.0f, 120);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_pExplosionSFX->UpdateSound(position, velocity);

   m_pExplode = g_ParticleManager.ObtainSystem();
   if(m_pExplode)
   {
      g_ParticleManager.CreateSystem(m_pExplode, *GetFrame(0)->GetPosition(), NULL, g_ParticleManager.ENEMYEXP_LARGE, LOD_BOTH);
   }
}

//////////////////////////////////////////////////////////////////////////
/// Reset
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CConvoy::Reset(void)
{
   
}

//////////////////////////////////////////////////////////////////////////
/// Take damage
/// 
/// Input:     float damage - amount of damage to be taken
///            CGameObject * guywhohit me - pointer to object that hit convoy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CConvoy::TakeDamage(float damage, CGameObject * guywhohitme)
{
   m_HitPoints -= damage;

   if(m_HitPoints <= m_MaxHitPoints * 0.5f && m_CurConvoyState == CONVOY_FINE)
   {
      m_CurConvoyState = CONVOY_SMOKE;
      m_pSmoke = g_ParticleManager.ObtainSystem(); 
      if(m_pSmoke)
          g_ParticleManager.CreateSystem(m_pSmoke, *ConvoyTopMid.GetPosition(), ConvoyTopMid.GetPosition(), g_ParticleManager.CONVOY_SMOKE, LOD_BOTH);
   }
   else if(m_HitPoints <= m_MaxHitPoints * 0.25f && m_CurConvoyState == CONVOY_SMOKE)
   {
      if(tempbool2)
      {
         if(GameMain.m_CurrentConvoyCom == GameMain.COM_CONVOYNONE)
            GameMain.m_CurrentConvoyCom = GameMain.COM_CONVOYHURT;
         tempbool2 = false;
      }
      
      m_CurConvoyState = CONVOY_FIRE;
      m_pFire = g_ParticleManager.ObtainSystem(); 
      if(m_pFire)
          g_ParticleManager.CreateSystem(m_pFire, *ConvoyTopMid.GetPosition(), ConvoyTopMid.GetPosition(), g_ParticleManager.CONVOY_FIRE, LOD_BOTH);
   }

}

//////////////////////////////////////////////////////////////////////////
/// SetWayPoints
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CConvoy::SetWaypoints(Waypoint* WaypointArray, int Indices)
{
   m_Waypoints = WaypointArray;
   m_LastWaypoint = Indices;
}

void CConvoy::MoveTowardWaypoint()
{
   AimTowardTarget();
   RBMoveForward();
}


int CConvoy::TargetRelativePosition()
{
   int TargetPosition = 0;
     
   const Sphere* pMySphere = GetSphere();

   CVector3f TargetOrigin;
   CVector3f MyOrigin;
   CVector3f PToP;
   CVector3f MyLookAt;
   CVector3f Result;

   TargetOrigin   = *(m_Waypoints[m_CurrentWaypointIndex].m_Frame.GetPosition());

   MyOrigin    = pMySphere->Center;

   const CObjFrame* Frame = GetFrame(0);

   Ray         ShotTrajectory;
   Sphere TargetSphere;

   TargetSphere.Center = TargetOrigin;

   float       Scale = 1.0f;

   float       Dist = DistanceSquare(&MyOrigin, &TargetOrigin);
/*
   if (Dist > 100.0f * 100.0f)
   {
      Scale = Dist / 100.0f * 100.0f ;

      if (Scale > 5.0f)
         Scale = 5.0f;
   }
   */

   TargetSphere.Radius = 40.0f * Scale;

   Frame->GetPosition(&(ShotTrajectory.Origin));
   Frame->GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TargetSphere))
      return TargetPosition;

   Vec3fSubtract(&PToP, &TargetOrigin, &MyOrigin);
   Frame->GetForward(&MyLookAt);

   MyLookAt.y = 0.0f;
   PToP.y = 0.0f;

   Vec3fCrossProduct(&Result, &MyLookAt, &PToP);

   if (Result.y > 0.0f)
      TargetPosition = 1;
   else if (Result.y < 0.0f)
      TargetPosition = -1;

   return TargetPosition;
}

void CConvoy::AimTowardTarget()
{
   if (g_time - m_AimTimer < m_AimWait)
      return;

   m_AimTimer = g_time;

   int pos = TargetRelativePosition();

   if (pos > 0)
      RBTurnRight(-m_TurnSpeed);
   else if (pos < 0)
      RBTurnRight(m_TurnSpeed);
}

bool CConvoy::IsAtCurrentWaypoint()
{
   bool Result = false;

   CVector3f Waypt = *(m_Waypoints[m_CurrentWaypointIndex].m_Frame.GetPosition());
   CVector3f UnitPos = GetSphere()->Center;

   Waypt.y = 0.0f;
   UnitPos.y = 0.0f;

   float Dist = DistanceSquare(&Waypt, &UnitPos);

   if (Dist < 30.0f * 30.0f)
      Result = true;

   return Result;
}

void CConvoy::NextWaypoint()
{
   m_CurrentWaypointIndex ++;

   if (m_CurrentWaypointIndex == m_LastWaypoint - 3)
   {
      if(GameMain.m_CurLevel1Event == GameMain.NONE)
         GameMain.m_CurLevel1Event = GameMain.FINAL_ATTACK;
   }

   if(m_CurrentWaypointIndex == m_LastWaypoint - 1)
   {
      if(tempbool3)
      {
         ++GameMain.m_NumConvoysHome;
         GameMain.m_CurrentConvoyCom = GameMain.COM_CONVOYHOME;
         tempbool3 = false;
      }   
   }

   if (m_CurrentWaypointIndex >= m_LastWaypoint)
   {
      m_CurrentWaypointIndex = m_LastWaypoint -1;
      m_CurConvoyMoveState = CONVOY_STOPPED;
   }
}
