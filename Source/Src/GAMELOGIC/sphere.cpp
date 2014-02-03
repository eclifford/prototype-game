/// \file Sphere.cpp
/// Enemy Sphere unit
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/Sphere.h"
#include "gamelogic/medtank.h"
#include "core/oglwindow.h"
#include "input/inputmanager.h"
#include "core/gamemain.h"
#include "math3d/intersection.h"
#include "gamelogic/objectfactory.h"
#include "camera/camera.h"
#include "graphics/terrain.h"
#include "graphics/geometrymanager.h"
#include "gamelogic/laser.h"

#include "gamelogic/player.h"

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
extern CTerrain*        g_pCurrLevel;
extern CGeometryManager g_GeometryManager;

/////////////////////////////////
/*/ External Globals          /*/ 
/////////////////////////////////

extern float g_time;
extern int   g_AwakeEnemies;
extern float g_FrameTime;

CSphere::CSphere()
{

}

CSphere::~CSphere()
{
   delete m_pSphereAura;
}

void CSphere::Initialize(unsigned int GeometryIndex, unsigned int NumParts, CVector3f * pTranslations, 
                         int TextureIndex, const Sphere * pSphere, AABB* pAABB, CQuadtree * pQuadtree)
{
   CGameObject::Initialize(GeometryIndex, NumParts, pTranslations, TextureIndex, pSphere, pAABB, pQuadtree);
  
   Sphere TempSphere;
   CVector3f* pTrans;

   g_GeometryManager.LoadPTGFile("resources\\models\\sphereaura.ptg", &TempSphere,
                                 pAABB, &GeometryIndex, &NumParts, &pTrans,
                                 &TextureIndex); 

   m_pSphereAura = new CQuadtreeRenderObj;

   m_pSphereAura->Initialize(&TempSphere, GeometryIndex, NumParts, pTrans, TextureIndex);   

   delete [] pTrans;

   m_pSphereAura->m_Material.m_Diffuse.r = 0.2f;
   m_pSphereAura->m_Material.m_Diffuse.g = 0.2f;
   m_pSphereAura->m_Material.m_Diffuse.b = .8f;
   m_pSphereAura->m_Material.m_Diffuse.a = 0.3f;

   //m_pShieldModel->m_Material.m_Ambient.r = 0.0f;
   //m_pShieldModel->m_Material.m_Ambient.g = 0.0f;
   m_pSphereAura->m_Material.m_Ambient.b = .8f;
   m_pSphereAura->m_Material.m_Ambient.a = 0.3f;

   m_pSphereAura->m_Material.m_Specular.r = 0.0f;
   m_pSphereAura->m_Material.m_Specular.g = 0.0f;
   m_pSphereAura->m_Material.m_Specular.b = 0.1f;
   m_pSphereAura->m_Material.m_Specular.a = 1.0f;

   m_RenderObj.m_Material.m_Shininess = 128;
}

void CSphere::Setup()
{
   m_AITimer = g_time;
   m_AI = true;
   m_DebugAI = false;
   m_Awake = false;
   m_HitPoints = 10;
   m_MaxHitPoints = 10;
   m_CurrentAggro = 0;
   m_MainWeaponLastFired = g_time;
   m_RecentHitsByPlayer = 0;
   m_DefensivenessTimer = g_time;
   m_ShotDamage = 10.0f;
   m_MainWeaponCooldown = 0.5f;
   m_DistanceFromPlayer = 0.0f;
   m_Velocity = 25.0f;
   m_CircleAngularVelocity = PI/8.0f;
   m_AngularVelocity = PI/6.0f;

   // Movement Info
   m_Elevation = 15.0f + float (rand() % 15);
   m_AttackDistance = 60.0f + float (rand() % 30);
   m_AITimer = g_time;
   m_OffsetFromPlayer.x = 0.0f;
   m_OffsetFromPlayer.y = 0.0f;
   m_OffsetFromPlayer.z = 0.0f;

   m_LaserCoolDown = .01f;
   m_LaserLastShot = g_time;

   DebugAIOff();
   
}

void CSphere::SetupAI()
{
   InitStateMachine();
}

void CSphere::Update(void)
{
   // Tank is dead
   if(m_HitPoints <= 0)
   {
      Die();
      return;
   }

   if (m_Awake)
   {
      // Update the engine sound
      GetPosition(&m_CurPosition, 0);
      CVector3f Velocity;
      Velocity.x = (m_CurPosition.x - m_LastPosition.x) / g_FrameTime;
      Velocity.y = (m_CurPosition.y - m_LastPosition.y) / g_FrameTime;
      Velocity.z = (m_CurPosition.z - m_LastPosition.z) / g_FrameTime;
      m_pEngineSFX->UpdateSound(m_CurPosition, Velocity);
      m_LastPosition = m_CurPosition;
   
      //float mag = Vec3fMagnitude(&Velocity);
      //m_pEngineSFX->AdjustVolume(mag * 4);

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
   }
   else
   {
      ScanForPlayer();
      Plane p;
      CVector3f MyPos;

      GetPosition(&MyPos, 0);

      if (g_pCurrLevel->GetPlaneAtPoint(MyPos.x, MyPos.z, &p))
      {
         float t = ClampToPlane(&MyPos, &p, m_Elevation);
         MyPos.y += t;
         SetPosition(&MyPos, 0);
      }

   }

   /*
   static float transchange = 1.0f;
   static float transchange2 = 1.0f;

   if(m_pSphereAura->m_Material.m_Diffuse.a >= 1.0f)
   {
      transchange *= -1.0f;
   }

   if(m_pSphereAura->m_Material.m_Diffuse.a <= 0.0f)
   {
      transchange *= -1.0f;
   }

   if(m_pSphereAura->m_Material.m_Ambient.a >= 1.0f)
   {
      transchange2 *= -1.0f;
   }
   
   if(m_pSphereAura->m_Material.m_Ambient.a <= 0.0f)
      transchange2 *= -1.0f;

   // update shield fade
   m_pSphereAura->m_Material.m_Diffuse.a += (transchange * g_FrameTime);
   m_pSphereAura->m_Material.m_Ambient.a += (transchange2 * g_FrameTime);

   if(m_pSphereAura->m_Material.m_Diffuse.a > 1.0f)
      m_pSphereAura->m_Material.m_Diffuse.a = 1.0f;
   if(m_pSphereAura->m_Material.m_Diffuse.a < 0.0f)
      m_pSphereAura->m_Material.m_Diffuse.a = 0.0f;
   if(m_pSphereAura->m_Material.m_Ambient.a > 1.0f)
      m_pSphereAura->m_Material.m_Ambient.a = 1.0f;
   if(m_pSphereAura->m_Material.m_Ambient.a < 0.0f)
      m_pSphereAura->m_Material.m_Ambient.a = 0.0f;
   
   //m_pSphereAura->m_Material.m_Ambient.a += (transchange2 * g_FrameTime);
   //m_pSphereAura->m_Material.m_Ambient.a = (ShieldDisplayTime +
   //                                         (m_StartShieldTime - g_time)) * .3f;

  */


   m_pSphereAura->SetFrame(m_RenderObj.GetFrame(0), 0);
   m_pSphereAura->Update();
   CGameObject::Update();
}

void CSphere::Collide(const CGameObject *obj)
{

}

void CSphere::Die(void)
{
   // Make explosion sound
   m_pExplosionSFX->Play3DAudio(40, 10000.0f, 120);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_pExplosionSFX->UpdateSound(position, velocity);

   m_pEngineSFX->StopAudio();

   // Setup enemy explosion
   explode = g_ParticleManager.ObtainSystem();
   if(explode)
      g_ParticleManager.CreateSystem(explode, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.ENEMYEXP_SMALL, LOD_BOTH);
   
   
   Sleep();
   m_Active = false;
   RemoveFromQuadtree();
   GameMain.m_NumEnemiesAlive --;
   g_Player.m_EnemySpheresDestroyed++;

   m_pSphereAura->RemoveFromQuadtree();
   DropPowerup(25.0f);
}

void CSphere::Reset(void)
{
   
}

void CSphere::TakeDamage(float damage, CGameObject * guywhohitme)
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
}

int CSphere::PlayerRelativePosition()
{
   int PlayerPosition = 0;

   // aim toward player
   const Sphere* pPlayerSphere = g_Player.GetSphere();
   Sphere PlayerSphere = *pPlayerSphere;
     
   float       Scale = 1.0f;
   float       Dist = DistanceSquare(&pPlayerSphere->Center,  &(GetSphere()->Center));

   if (Dist > 100.0f * 100.0f)
   {
      Scale = Dist/(100.0f * 100.0f);
   }
   else if (Dist < (80.0f * 80.0f))
      Scale = .5f;

   PlayerSphere.Radius *= Scale;

   const Sphere* pEnemySphere = GetSphere();

   CVector3f PlayerOrigin;
   CVector3f EnemyOrigin;
   CVector3f EToP;
   CVector3f EnemyLookAt;
   CVector3f Result;

   PlayerOrigin   = pPlayerSphere->Center;
   EnemyOrigin    = pEnemySphere->Center;

   PlayerOrigin.x += m_OffsetFromPlayer.x;

   const CObjFrame* Frame = GetFrame(0);

   // aim tank on y
   Vec3fSubtract(&EToP, &PlayerOrigin, &EnemyOrigin);
   Frame->GetForward(&EnemyLookAt);

   EnemyLookAt.y = 0.0f;
   EToP.y = 0.0f;

   Vec3fCrossProduct(&Result, &EnemyLookAt, &EToP);

   if (Result.y > 0.0f)
      PlayerPosition = 1;
   else if (Result.y < 0.0f)
      PlayerPosition = -1;

   return PlayerPosition;
}


void CSphere::AimTowardPlayer()
{
   if (AimIsGood())
      return;

   int pos = PlayerRelativePosition();

   if (pos > 0)
      RotateLocalY(g_FrameTime * m_AngularVelocity,0);
   else if (pos < 0)
      RotateLocalY(-g_FrameTime * m_AngularVelocity,0);
}

void CSphere::RotateAroundPlayer()
{
   CVector3f PlayerPos;

   g_Player.GetPosition(&PlayerPos,0);

   SetPosition(&PlayerPos,0);

   RotateLocalY(g_FrameTime * m_CircleAngularVelocity,0);
   MoveForward(-m_DistanceFromPlayer,0);
}


bool CSphere::PlayerInFrontOfMe()
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

   const CObjFrame* Frame = GetFrame(0);

   Frame->GetForward(&EnemyLookAt);

   // aim turret on x axis
   Vec3fSubtract(&EToP, &PlayerOrigin, &EnemyOrigin);

   float Dot = Vec3fDotProduct(&EToP, &EnemyLookAt);

   if (Dot > 0.0f)
      Fudge = true;

   return Fudge;
}

void CSphere::ScanForPlayer()
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
      if (pCurrentObj->GetType() == OBJ_PLAYER)
      {
         Awaken();
         OnAwaken();
         pCurrentObj = pCurrentObj->m_GuyNextToMe;
         continue;
      }

      pt2 = pCurrentObj->GetSphere()->Center;

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

      pCurrentObj = pCurrentObj->m_GuyNextToMe;
   } while(pCurrentObj != NULL);

   ClearWhoIsAroundMe(pFirstGuy);

}


bool CSphere::AimIsGood()
{
   bool GotShot = false;

   const Sphere* pPlayerSphere = g_Player.GetSphere();

   Sphere TempSphere;

   TempSphere.Radius = pPlayerSphere->Radius * .5f;
   TempSphere.Center = pPlayerSphere->Center;
  
   Ray         ShotTrajectory;
   const CObjFrame* Frame = GetFrame(0);

   Frame->GetPosition(&(ShotTrajectory.Origin));
   Frame->GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (RayToSphere(&ShotTrajectory, &TempSphere))
      GotShot = true;

   return GotShot;
}

void CSphere::FireLaserAtGround()
{
   if (g_time - m_LaserLastShot > m_LaserCoolDown)
   {
      m_LaserLastShot = g_time;
      const CObjFrame* frame = GetFrame(0);

      CObjFrame Temp = *frame;

      Temp.RotateLocalX(.3f);

      CLaser * shot = g_ObjectFactory.CreateLaser();
      shot->SetUp(ENEMY_SHOT, 5, 5.0f, this);
      shot->SetFrame(&Temp,0);
   }
}

void CSphere::FireLaserAtPlayer()
{
   if (g_time - m_LaserLastShot > m_LaserCoolDown)
   {
      m_LaserLastShot = g_time;
      const CObjFrame* frame = GetFrame(0);
      CVector3f Forward;
      CObjFrame Frame = *frame;

      Frame.GetForward(&Forward);

      CVector3f PtoP;
      CVector3f PlayerPos;
      CVector3f MyPos;

      MyPos = GetSphere()->Center;
      PlayerPos = g_Player.GetSphere()->Center;

      Vec3fSubtract(&PtoP, &PlayerPos, &MyPos);

      Vec3fNormalize(&PtoP, &PtoP);

      CQuaternion Q;

      QuatRotationArc(&Q, &Forward, &PtoP);

      CQuaternion Q2;

      Frame.GetOrientation(&Q2);

      QuatCrossProduct(&Q2, &Q, &Q2);

      Frame.SetOrientation(&Q2);

      Frame.MoveForward(GetSphere()->Radius);

      CLaser * shot = g_ObjectFactory.CreateLaser();
      shot->SetUp(ENEMY_SHOT, 5, 5.0f, this);
      shot->SetFrame(&Frame,0);
   }
}

void CSphere::FireLaserCrazy()
{
   if (g_time - m_LaserLastShot > m_LaserCoolDown)
   {
      m_LaserLastShot = g_time;
      const CObjFrame* frame = GetFrame(0);

      CObjFrame Temp = *frame;

      float fx, fy;
      int x, y;

      x = rand() % 100;
      y = rand() % 100;

      fx = float(x)/100.0f;
      fy = float(y)/100.0f;

      Temp.RotateLocalY(fy);
      Temp.RotateLocalX(fx);

      CLaser * shot = g_ObjectFactory.CreateLaser();
      shot->SetUp(ENEMY_SHOT, 5, 5.0f, this);
      shot->SetFrame(&Temp,0);
   }
}


void CSphere::InitStateMachine(void)
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
 
   m_StateMachine.AddState("Move");
   m_StateMachine.AddState("Intimidate");
   m_StateMachine.AddState("Attack");

   // add enter functions for each state
   m_StateMachine.AddOnEnter("Move", SphereMoveOnEnter);
   m_StateMachine.AddOnEnter("Intimidate", SphereIntimidateOnEnter);
   m_StateMachine.AddOnEnter("Attack", SphereAttackOnEnter);

   // add exit functions for each state
   m_StateMachine.AddOnExit("Move", SphereMoveOnExit);
   m_StateMachine.AddOnExit("Intimidate", SphereIntimidateOnExit);
   m_StateMachine.AddOnExit("Attack", SphereAttackOnExit);

   // add update functions for each state
   m_StateMachine.AddUpdate("Move", SphereMoveUpdate);
   m_StateMachine.AddUpdate("Intimidate", SphereIntimidateUpdate);
   m_StateMachine.AddUpdate("Attack", SphereAttackUpdate);

   // set transitions between states
   m_StateMachine.AddTransitionCondition("Move", "Intimidate", SphereMoveToIntimidate);
   m_StateMachine.AddTransitionCondition("Intimidate", "Attack", SphereIntimidateToAttack);
   m_StateMachine.AddTransitionCondition("Intimidate", "Move", SphereIntimidateToMove);

   m_StateMachine.SetCurrentState(0);
}


void CSphere::MoveTowardPlayer()
{
   AimTowardPlayer();
   MoveForward(m_Velocity * g_FrameTime,0);
}


bool CSphere::InDistanceFromPlayer(float Dist)
{
   bool bReturnVal = false;

   const Sphere* pPlayerSphere = g_Player.GetSphere();
   const Sphere* pMySphere = GetSphere();

   if (DistanceSquare(&pPlayerSphere->Center, &pMySphere->Center) < Dist * Dist)
      bReturnVal = true;

   return bReturnVal;
}


bool SphereMoveToIntimidate(void* pVoidObj)
{
   CSphere* pObj = (CSphere*) pVoidObj;
   bool Fudge = false;

   if (pObj->InDistanceFromPlayer(160.0f))
      Fudge = true;

   return Fudge;
}

bool SphereIntimidateToAttack(void* pVoidObj)
{
   CSphere* pObj = (CSphere*) pVoidObj;
   bool Fudge = false;

   if (pObj->InDistanceFromPlayer(pObj->m_AttackDistance))
      Fudge = true;

   return Fudge;
}

bool SphereIntimidateToMove(void* pVoidObj)
{
   CSphere* pObj = (CSphere*) pVoidObj;
   bool Fudge = false;

   if (!pObj->InDistanceFromPlayer(160.0f))
      Fudge = true;

   return Fudge;
}


// state update
void SphereMoveUpdate(void* pVoidObj)
{
   CSphere* pObj = (CSphere*) pVoidObj;
   pObj->MoveTowardPlayer();
   Plane p;
   CVector3f MyPos;

   pObj->GetPosition(&MyPos, 0);

   if (g_pCurrLevel->GetPlaneAtPoint(MyPos.x, MyPos.z, &p))
   {
      float t = ClampToPlane(&MyPos, &p, pObj->m_Elevation);
      MyPos.y += t;
      pObj->SetPosition(&MyPos, 0);
   }
}

void SphereIntimidateUpdate(void* pVoidObj)
{
   CSphere* pObj = (CSphere*) pVoidObj;
   pObj->MoveTowardPlayer();

   Plane p;
   CVector3f MyPos;

   pObj->GetPosition(&MyPos, 0);

   if (g_pCurrLevel->GetPlaneAtPoint(MyPos.x, MyPos.z, &p))
   {
      float t = ClampToPlane(&MyPos, &p, pObj->m_Elevation);
      MyPos.y += t;
      pObj->SetPosition(&MyPos, 0);
   }
   pObj->FireLaserAtGround();

}

void SphereAttackUpdate(void* pVoidObj)
{
   CSphere* pObj = (CSphere*) pVoidObj;
   pObj->RotateAroundPlayer();

   Plane p;
   CVector3f MyPos;

   pObj->GetPosition(&MyPos, 0);

   if (g_pCurrLevel->GetPlaneAtPoint(MyPos.x, MyPos.z, &p))
   {
      float t = ClampToPlane(&MyPos, &p, pObj->m_Elevation);
      MyPos.y += t;
      pObj->SetPosition(&MyPos, 0);
   }
   pObj->FireLaserAtPlayer();
}


// state on enter
void SphereMoveOnEnter(void* pVoidObj)
{

}

void SphereIntimidateOnEnter(void* pVoidObj)
{

}

void SphereAttackOnEnter(void* pVoidObj)
{
   CSphere* pObj = (CSphere*) pVoidObj;

   pObj->m_DistanceFromPlayer = Distance(&(pObj->GetSphere()->Center), &(g_Player.GetSphere()->Center));

   if (rand() % 10 < 5)
      pObj->m_AngularVelocity *= -1.0f;

   CVector3f Forward;
   CVector3f MyPos;
   CVector3f PlayerPos;
   CVector3f PtoP;

   MyPos = pObj->GetSphere()->Center;
   PlayerPos = g_Player.GetSphere()->Center;

   const CObjFrame* frame = pObj->GetFrame(0);

   frame->GetForward(&Forward);

   Vec3fSubtract(&PtoP, &PlayerPos, &MyPos);

   PtoP.y = 0.0f;
   Forward.y = 0.0f;

   Vec3fNormalize(&Forward, &Forward);
   Vec3fNormalize(&PtoP, &PtoP);

   CVector3f Result;

   Vec3fCrossProduct(&Result, &Forward, &PtoP);

   float sign = 1.0f;

   if (Result.y < 0)
      sign *= -1.0f;

   float Dot;

   Dot = Vec3fDotProduct(&Forward, &PtoP);

   float angle = (float) acos(Dot);

   pObj->RotateLocalY(sign * angle, 0);

}


// state on exit
void SphereMoveOnExit(void* pVoidObj)
{

}

void SphereIntimidateOnExit(void* pVoidObj)
{

}

void SphereAttackOnExit(void* pVoidObj)
{

}

void CSphere::OnAwaken(void)
{
   // Play the engine sound	when enemy is awakened
   m_pEngineSFX->Play3DAudio(20, 10000.0f, 120);
}

void CSphere::FollowTerrain(void)
{
   float offset = 1.0f;

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