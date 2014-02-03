/// \file eturret.cpp
/// Enemy turret unit
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\eturret.h"
#include "graphics\terrain.h"
#include "math3d\intersection.h"
#include "core\gamemain.h"
#include "gamelogic\objectfactory.h"
#include "graphics\terrain.h"
#include "gamelogic\player.h"

#include "utils\mmgr.h"

extern CObjectFactory   g_ObjectFactory;
extern CGameMain GameMain;
extern float g_time;
extern float g_FrameTime;
extern CGameObject* g_CollisionAvoidanceList;
extern CPlayer g_Player;
extern CTerrain *g_pCurrLevel;

CEnemyTurret::CEnemyTurret()
{

}

CEnemyTurret::~CEnemyTurret()
{

}

void CEnemyTurret::Setup()
{
   m_AI = true;
   m_Active = true;
   m_AwakenByPlayerRange = 350.0f;
   m_AwakenByEnemyRange = 50.0f;
   m_AttackRadius = 350.0f;
   m_Active = true;
   m_HitPoints = 300;
   m_MaxHitPoints = 300;
   m_AITimer = g_time;
   m_TurretPos = 0.0f;
   m_MainWeaponLastFired = g_time;
   m_ShotDamage = 8.0f;
   m_MainWeaponCooldown = 0.2f;
   m_CurrentTarget = &g_Player;
   m_LastHitBy = NULL;
   m_HitCounter = 0;
	m_StaticObj = true;
   turretpos = 0.0f;   // Current position of turret
   velocity = 6.0f;    // Speed of recoil
   maxrecoil = -2.0f;  // Max/min distance of recoil
   forward = false;     // Direction moving
   m_MaxTurnAngle = 2.09f;
   turretpos = 0.0;
   m_CurTurret = 0;
   m_Awake = false;

   // Get the turrets position
   m_RenderObj.GetPosition(&m_TurretPosition, 1);
}

void CEnemyTurret::Update(void)
{
   if (!m_Active)
      return;

   CGameObject::Update();
   FollowTerrain();

   if(m_HitPoints <= 0)
   {
      Die();
      return;
   }

   if (m_AI && g_GameOptions.GetAI())
   {
      if (m_Awake)
      {
         if (!m_CurrentTarget->IsActive() || DistanceSquare(&m_CurrentTarget->GetSphere()->Center, &GetSphere()->Center) > m_AttackRadius * m_AttackRadius * 2.0f)
            TargetEnemy();
         else
         {
            AimTowardTarget();
            if (AimIsGood())
               Fire();
         }
      }
      else
         ScanForTarget();
   }
}

void CEnemyTurret::Collide(const CGameObject *obj)
{

}

void CEnemyTurret::Die(void)
{
   m_Active = false;
   RemoveFromQuadtree();
      
   // Make main weapon sound
   m_ExplosionSFX->Play3DAudio(40, 10000.0f, 200);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_ExplosionSFX->UpdateSound(position, velocity);
   GameMain.m_NumEnemiesAlive--;

   Sleep();

   // Setup enemy explosion
   m_Explode = g_ParticleManager.ObtainSystem();
   if(m_Explode)
      g_ParticleManager.CreateSystem(m_Explode, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.TURRET_EXP, LOD_BOTH);
}

void CEnemyTurret::Reset(void)
{
   
}

void CEnemyTurret::TakeDamage(float damage, CGameObject * guywhohitme)
{
   m_HitPoints -= damage;

   if (guywhohitme != m_LastHitBy)
   {
      m_LastHitBy = guywhohitme;
      m_HitCounter = 1;
   }
   else
   {
      m_HitCounter ++;
      if (m_HitCounter > 2)
         m_CurrentTarget = guywhohitme;
   }

   // Take damage
   m_HitPoints -= damage;
}


void CEnemyTurret::FollowTerrain(void)
{
   float offset = 0.0f;

   Plane p;

	// Clamp it to ground first
	CObjFrame frame = *m_OBB.Frame;
	CVector3f pos;

	frame.GetPosition(&pos);
	g_pCurrLevel->GetPlaneAtPoint(pos.x, pos.z, &p);
	pos.y += ClampToPlane(&pos, &p, m_OBB.y + offset);
	frame.SetPosition(&pos);

	// Figure out orientation
	float hw = m_OBB.x;
	float hh = m_OBB.y;
	float hl = m_OBB.z;

	CQuaternion q;

	CVector3f front(0.0f, -hh, hl);
	frame.RotateToWorldPoint(&front, &front);
	g_pCurrLevel->GetPlaneAtPoint(front.x, front.z, &p);
	front.y += ClampToPlane(&front, &p, 0.0f);
	
	CVector3f backleft(hw, -hh, -hl);
	frame.RotateToWorldPoint(&backleft, &backleft);
	g_pCurrLevel->GetPlaneAtPoint(backleft.x, backleft.z, &p);
	backleft.y += ClampToPlane(&backleft, &p, 0.0f);

	CVector3f backright(-hw, -hh, -hl);
	frame.RotateToWorldPoint(&backright, &backright);
	g_pCurrLevel->GetPlaneAtPoint(backright.x, backright.z, &p);
	backright.y += ClampToPlane(&backright, &p, 0.0f);

	GetOrientationFromPoints(&q, &front, &backleft, &backright);

	frame.SetOrientation(&q);
	SetFrame(&frame, 0);
}

void CEnemyTurret::AimTowardTarget()
{
   if (AimIsGood())
   {
      return;
   }

   float RotY, RotX;
  
   TargetRelativePosition(RotY, RotX);

   CVector3f Frame1Pos;
   CVector3f Frame2Pos;

   m_RenderObj.GetFrame(1)->GetPosition(&Frame1Pos);
   
   CVector3f Zero;

   Zero.x = 0.0f;
   Zero.y = 0.0f;
   Zero.z = 0.0f;

   SetPosition(&Zero, 1);

   m_RenderObj.GetFrame(1)->RotateWorldY(RotY);

   SetPosition(&Frame1Pos, 1);

//   float turnmax = PI/4.0f;

   if (m_TurretPos + RotX < -.3f || m_TurretPos + RotX > .3f)
      RotX = 0.0f;

   m_TurretPos += RotX;

   RotateLocalX(RotX, 1);
}

void CEnemyTurret::FindShotPosition(CObjFrame * frame, float ForwardScale)
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

bool CEnemyTurret::AimIsGood()
{
   bool GotShot = false;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere.Radius = pTargetSphere->Radius;
   TempSphere.Center = pTargetSphere->Center;

   TempSphere.Radius = m_CurrentTarget->GetOBB()->y * .5f;

   Ray         ShotTrajectory;
   CObjFrame   Frame;

   FindShotPosition(&Frame, 6.0f);

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      GotShot = true;

   return GotShot;
}

void CEnemyTurret::TargetRelativePosition(float& RotY, float& RotX)
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

   FindShotPosition(&Frame, 6.0f);

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
      if (PToP.z <= 0.0f || fabs(PToP.x/PToP.z) >= 0.05f)
         RotY = .05f * TargetPosition;
      else
         RotY = (float) atan2(PToP.x, PToP.z);

      // ascertain angle to tilt turret
      float Mag = Vec3fMagnitude(&PToP2);

      if (Mag != 0.0f)
      {
         float Angle = float(asin(PToP2.y/Mag));

         if (Angle > .05f)
            Angle = .05f;
         else if (Angle < -.05f)
            Angle = -.05f;

         RotX = -Angle;
      }
      else
         RotX = 0.0f;
   }
   else
      RotX = 0.0f;
}


void CEnemyTurret::ScanForTarget()
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

         case OBJ_MSLETANK:
         {
            if ( ((CEnemy*) pCurrentObj)->IsAwake() )
            {
               Awaken();
			      OnAwaken();
            }
         }
            break;

         case OBJ_ETURRET:
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

void CEnemyTurret::OnAwaken(void)
{
   // Play the engine sound	when enemy is awakened
//   m_pEngineSFX->Play3DAudio(20, 10000.0f, 120);
}

bool CEnemyTurret::PlayerInFrontOfMe()
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
   FindShotPosition(&Frame, 6.0f);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &PlayerOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}


bool CEnemyTurret::InFrontOfMe(CGameObject* obj)
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
   FindShotPosition(&Frame, 6.0f);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

bool CEnemyTurret::InDistanceFromTarget(float Dist)
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

bool CEnemyTurret::InDistanceFromPlayer(float Dist)
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



void CEnemyTurret::Fire()
{   
   // Check to see if the weapon is ready to be fired
   if(g_time > m_MainWeaponCooldown + m_MainWeaponLastFired)
   {
      m_MainWeaponLastFired = g_time;
     
      // Find the position of the shot
      CObjFrame frame;
      FindShotPosition(&frame, 6.0f);
      CObjFrame Temp = frame;

      switch(m_CurTurret)
      {
         case 0:
            {
               CLaser * shot1 = g_ObjectFactory.CreateLaser();
               shot1->SetUp(ENEMY_SHOT, 10, 5.0f, this);          
               Temp.MoveUp(3.1316f);
               shot1->SetFrame(&Temp, 0);
               
               m_MainWeaponFlash = g_ParticleManager.ObtainSystem();
               if(m_MainWeaponFlash)
                  g_ParticleManager.CreateSystem(m_MainWeaponFlash, *Temp.GetPosition(), NULL, g_ParticleManager.LASER_TURRET, LOD_BOTH);
            }
         break;

         case 1:
            {
         
               CLaser * shot2 = g_ObjectFactory.CreateLaser();
               shot2->SetUp(ENEMY_SHOT, 10, 5.0f, this);
               Temp = frame;
               Temp.MoveUp(.723f);
               Temp.MoveRight(1.471f);
               shot2->SetFrame(&Temp, 0);

               m_MainWeaponFlash = g_ParticleManager.ObtainSystem();
               if(m_MainWeaponFlash)
                  g_ParticleManager.CreateSystem(m_MainWeaponFlash, *Temp.GetPosition(), NULL, g_ParticleManager.LASER_TURRET, LOD_BOTH);
            }
            break;
            
         case 2:
            {
     
              CLaser * shot3 = g_ObjectFactory.CreateLaser();
                       shot3->SetUp(ENEMY_SHOT, 10, 5.0f, this);
              Temp = frame;
              Temp.MoveUp(.723f);
              Temp.MoveRight(-1.471f);
              shot3->SetFrame(&Temp, 0);

               m_MainWeaponFlash = g_ParticleManager.ObtainSystem();
               if(m_MainWeaponFlash)
                  g_ParticleManager.CreateSystem(m_MainWeaponFlash, *Temp.GetPosition(), NULL, g_ParticleManager.LASER_TURRET, LOD_BOTH);
            }
         break;
      }
     
      // Make main weapon sound
      m_MainWeaponSFX->Play3DAudio(20, 10000.0f, 100);
      CVector3f position;
	   GetPosition(&position, 0);
      CVector3f velocity(0.0f, 0.0f, 0.0f);
      m_MainWeaponSFX->UpdateSound(position, velocity);

      m_CurTurret++;
      if(m_CurTurret > 2)
         m_CurTurret = 0;
   }
}
