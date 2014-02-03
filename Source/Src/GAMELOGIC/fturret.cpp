/// \file fturret.cpp
/// Friendly turret unit
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\fturret.h"
#include "core\gamemain.h"
#include "math3d\intersection.h"
#include "core\gamemain.h"
#include "gamelogic\objectfactory.h"
#include "graphics\terrain.h"
#include "camera\camera.h"
#include "gamelogic\player.h"

#include "utils\mmgr.h"

extern CBaseCamera* g_Camera;
extern CObjectFactory   g_ObjectFactory;
extern CGameMain GameMain;
extern float g_time;
extern float g_FrameTime;
extern CGameObject* g_CollisionAvoidanceList;
extern CPlayer g_Player;
extern CTerrain *g_pCurrLevel;
extern CAudioManager g_AudioManager;

CFriendlyTurret::CFriendlyTurret()
{

}

CFriendlyTurret::~CFriendlyTurret()
{

}

void CFriendlyTurret::Setup()
{
   m_AI = true;
   m_AttackRadius = 350.0f;
   m_Active = true;
   m_HitPoints = 700;
   m_MaxHitPoints = 700;
   m_AITimer = g_time;
   m_TurretPos = 0.0f;
   m_MainWeaponLastFired = g_time;
   m_ShotDamage = 9.0f;
   m_MainWeaponCooldown = .75f;
   m_CurTurretState = STANDBY;
   m_CurrentTarget = NULL;
   m_LastHitBy = NULL;
   m_HitCounter = 0;
	m_StaticObj = true;
   turretpos = 0.0f;   // Current position of turret
   velocity = 6.0f;    // Speed of recoil
   maxrecoil = -2.0f;  // Max/min distance of recoil
   forward = false;     // Direction moving

   // Movement Info
   float MaxVelocity  = 0.0f;    // Max velocity in meters per second
   float AccelerationTime = 0.5f;  // Time it takes to reach that velocity
   
   // Get the turrets position
   m_RenderObj.GetPosition(&m_TurretPosition, 1);
}

void CFriendlyTurret::Update(void)
{
   CGameObject::Update();
   FollowTerrain();

   if(m_HitPoints <= 0)
   {
      Die();
      return;
   }

   if (m_AI && g_GameOptions.GetAI())
   {
       if (m_CurrentTarget != NULL)
      {
         if (!m_CurrentTarget->IsActive() || 
            DistanceSquare(&m_CurrentTarget->GetSphere()->Center, &GetSphere()->Center) > m_AttackRadius * m_AttackRadius * 2.0f)
            TargetChoikin();
      }
      else
         TargetChoikin();

      if (m_CurrentTarget != NULL)
      {
         AimTowardTarget();
         if (AimIsGood())
            Fire();
      }

      UpdateTurretState();
   }
}

void CFriendlyTurret::Collide(const CGameObject *obj)
{

}

void CFriendlyTurret::Die(void)
{
   m_Active = false;
   RemoveFromQuadtree();

   // Make main weapon sound
   m_ExplosionSFX->Play3DAudio(30, 10000.0f, 150);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_ExplosionSFX->UpdateSound(position, velocity);

   // Setup enemy explosion
   m_Explode = g_ParticleManager.ObtainSystem();
   if(m_Explode)
      g_ParticleManager.CreateSystem(m_Explode, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.TURRET_EXP, LOD_BOTH);
}

void CFriendlyTurret::Reset(void)
{
   
}

void CFriendlyTurret::TakeDamage(float damage, CGameObject * guywhohitme)
{
   if (guywhohitme == &g_Player)
      return;

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

void CFriendlyTurret::TargetChoikin()
{
   CGameObject* pCurrentObj = GetWhoIsAroundMe(m_AttackRadius);
   CGameObject* pFirstGuy = pCurrentObj;
   
   m_CurrentTarget = NULL;

   if (pCurrentObj == NULL)
      return;

   float Dist = (m_AttackRadius * m_AttackRadius) + 10.0f;
   float TempDist;

   do 
   {
      TempDist = DistanceSquare(&(GetSphere()->Center), &(pCurrentObj->GetSphere()->Center));

      if (TempDist < Dist && pCurrentObj->m_RadarType == RADAR_ENEMY)
      {
         m_CurrentTarget = pCurrentObj;
         Dist = TempDist;
      }

      pCurrentObj = pCurrentObj->m_GuyNextToMe;

   } while(pCurrentObj != NULL);

   ClearWhoIsAroundMe(pFirstGuy);
}

void CFriendlyTurret::Fire()
{
   // Check to see if the weapon is ready to be fired
   if(g_time > m_MainWeaponCooldown + m_MainWeaponLastFired && m_CurTurretState == STANDBY)
   {
      m_MainWeaponLastFired = g_time;
   
      m_CurTurretState = RECOIL;

      CPlasmaShot * shot1 = g_ObjectFactory.CreatePlasmaShot();
      CPlasmaShot * shot2 = g_ObjectFactory.CreatePlasmaShot();

      shot1->SetUp(FRIENDLY_SHOT, this, m_ShotDamage);
      shot2->SetUp(FRIENDLY_SHOT, this, m_ShotDamage);

      // Find the position of the shot
      CObjFrame frame1, frame2;
      FindShotPosition(&frame1, 9.0f);
      FindShotPosition(&frame2, 9.0f);

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
         NULL, g_ParticleManager.FLASH, LOD_BOTH);
      }

      // Get a particle effect for the shot if one available
      shot1->m_ParticleEffect = g_ParticleManager.ObtainSystem();

      // If obtained a particle effect then we use it
      if(shot1->m_ParticleEffect != NULL)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(shot1->m_ParticleEffect, *(shot1->GetFrame(0)->GetPosition()),
         shot1->GetFrame(0)->GetPosition(), g_ParticleManager.PLASMA, LOD_BOTH);
      }  

      CParticleSystem * effect2 = g_ParticleManager.ObtainSystem();
      if(effect2)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(effect2, *(shot2->GetFrame(0)->GetPosition()),
         NULL, g_ParticleManager.FLASH, LOD_BOTH);
      }   

      shot2->m_ParticleEffect = g_ParticleManager.ObtainSystem();
      if(shot2->m_ParticleEffect != NULL)
      {
         // Create an effect for the shot
         g_ParticleManager.CreateSystem(shot2->m_ParticleEffect, *(shot2->GetFrame(0)->GetPosition()),
         shot2->GetFrame(0)->GetPosition(), g_ParticleManager.PLASMA, LOD_BOTH);
      }  

      // Make main weapon sound
      m_MainWeaponSFX->Play3DAudio(30, 10000.0f, 150);
      m_MainWeaponSFX2->Play3DAudio(30, 10000.0f, 150);
      CVector3f position;
	   GetPosition(&position, 0);
      CVector3f velocity(0.0f, 0.0f, 0.0f);
      m_MainWeaponSFX->UpdateSound(position, velocity);
      m_MainWeaponSFX2->UpdateSound(position, velocity);
   }
}

CGameObject* CFriendlyTurret::GetWhoIsAroundMe(float Size)
{
   CGameObject* firstAllyInProximity = NULL;
   CGameObject* currentCloseGuy = NULL;
   CGameObject* currentObj = g_CollisionAvoidanceList;

   if (g_CollisionAvoidanceList == NULL)
      return NULL;

   const Sphere* es = GetSphere();
   Sphere MySphere = *es;

   MySphere.Radius = Size;

   do 
   {
      if (currentObj == this)
      {
         currentObj = currentObj->m_CollisionAvoidanceListNext;
         continue;
      }

      switch (currentObj->GetType())
      {
         case OBJ_BUILDING:
         {
            currentObj = currentObj->m_CollisionAvoidanceListNext;
            continue;
            break;
         }

         case OBJ_BUILDING2:
         {
            currentObj = currentObj->m_CollisionAvoidanceListNext;
            continue;
            break;
         }

         case OBJ_BUILDING3:
         {
            currentObj = currentObj->m_CollisionAvoidanceListNext;
            continue;
            break;
         }

         case OBJ_CONVOY:
         {
            currentObj = currentObj->m_CollisionAvoidanceListNext;
            continue;
            break;
         }

         case OBJ_FTURRET:
         {
            currentObj = currentObj->m_CollisionAvoidanceListNext;
            continue;
            break;
         }

         case OBJ_PLAYER:
         {
            currentObj = currentObj->m_CollisionAvoidanceListNext;
            continue;
            break;
         }

         case OBJ_PLASMA:
         {
            currentObj = currentObj->m_CollisionAvoidanceListNext;
            continue;
            break;
         }

         case OBJ_ROCK1:
         case OBJ_ROCK2:
         case OBJ_ROCK3:
         case OBJ_ROCK4:
         case OBJ_ROCK5:
         case OBJ_ROCK6:
         {
            currentObj = currentObj->m_CollisionAvoidanceListNext;
            continue;
            break;
         }

         default:
            break;
      }

      if (SphereToSphere(&MySphere, currentObj->GetSphere()))
      {
         if (firstAllyInProximity == NULL)
         {
            firstAllyInProximity = currentObj;
            currentCloseGuy  = currentObj;
         }
         else
         {
            currentCloseGuy->m_GuyNextToMe = currentObj;
            currentCloseGuy = currentObj;
         }
      }

   	currentObj = currentObj->m_CollisionAvoidanceListNext;

   } while(currentObj != NULL);

   return firstAllyInProximity;
}

void CFriendlyTurret::ClearWhoIsAroundMe(CGameObject* obj)
{
   if (obj == NULL)
      return;

   CGameObject* oldObj;

   do 
   {
      oldObj = obj;
      obj = obj->m_GuyNextToMe;
      oldObj->m_GuyNextToMe = NULL;
   } while(obj != NULL);
}

void CFriendlyTurret::AimTowardTarget()
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
   m_RenderObj.GetFrame(2)->GetPosition(&Frame2Pos);
   
   CVector3f Zero;

   Zero.x = 0.0f;
   Zero.y = 0.0f;
   Zero.z = 0.0f;

   SetPosition(&Zero, 1);
   SetPosition(&Zero, 2);

   m_RenderObj.GetFrame(1)->RotateWorldY(RotY);
   m_RenderObj.GetFrame(2)->RotateWorldY(RotY);

   SetPosition(&Frame1Pos, 1);
   SetPosition(&Frame2Pos, 2);

//   float turnmax = PI/4.0f;

   if (m_TurretPos + RotX < -.3f || m_TurretPos + RotX > .3f)
      RotX = 0.0f;

   m_TurretPos += RotX;

   RotateLocalX(RotX, 1);
   RotateLocalX(RotX, 2);

}

void CFriendlyTurret::FindShotPosition(CObjFrame * frame, float ForwardScale)
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
   frame->MoveUp(2.0f);
}

bool CFriendlyTurret::AimIsGood()
{
   bool GotShot = false;

   const Sphere* pTargetSphere = m_CurrentTarget->GetSphere();

   Sphere TempSphere;

   TempSphere.Radius = pTargetSphere->Radius;
   TempSphere.Center = pTargetSphere->Center;

   TempSphere.Radius = m_CurrentTarget->GetOBB()->y * .5f;

   Ray         ShotTrajectory;
   CObjFrame   Frame;

   FindShotPosition(&Frame, 9.0f);

   Frame.GetPosition(&(ShotTrajectory.Origin));
   Frame.GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      GotShot = true;

   return GotShot;
}

void CFriendlyTurret::TargetRelativePosition(float& RotY, float& RotX)
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

   FindShotPosition(&Frame, 9.0f);

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

bool CFriendlyTurret::InFrontOfMe(CGameObject* obj)
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
   FindShotPosition(&Frame, 9.0f);
   Frame.GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &TargetOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

void CFriendlyTurret::FollowTerrain(void)
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

//////////////////////////////////////////////////////////////////////////
/// Update the location and status of the turret
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CFriendlyTurret::UpdateTurretState(void)
{
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
            turretpos -= (velocity * g_FrameTime * 3.5f);
         
         if (turretpos <= maxrecoil)
         {
            forward = true;
            turretpos = maxrecoil;
         }
         
         else if (turretpos >= 0.0f)
         {
            m_CurTurretState = STANDBY;
            forward = false;
            m_RenderObj.SetPosition(&m_TurretPosition, 2);
            turretpos = 0.0f;
            break;
         }

         m_RenderObj.SetPosition(&m_TurretPosition, 2);
         m_RenderObj.MoveForward(turretpos, 2);
      }
      break;
   }
}
