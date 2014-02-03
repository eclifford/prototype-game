/// \file meleetank.cpp
/// Enemy melee tank unit
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\meleetank.h"
#include "gamelogic\player.h"
#include "graphics\texturefonts.h"
#include "math3d\intersection.h"
#include "gamelogic\medtank.h"
#include "core\gamemain.h"
#include "graphics\particlemanager.h"
#include "physics\physics.h"

#include "utils\mmgr.h"

extern CGameMain GameMain;
extern CPlayer g_Player;
extern float g_time;
extern CTextureFonts g_TextureFont;
extern float g_FrameTime;
extern CAudioManager g_AudioManager;
extern CParticleManager g_ParticleManager;

CMeleeTank::CMeleeTank(CMeleeTank& MeleeTank) : CEnemy(MeleeTank)
{
    m_RigidBody = new CRigidBody;
    m_Type = OBJ_KAMITANK;
    m_pExplosion = NULL;
    m_pBallEnergy = NULL;
    m_pThrusterLeft = NULL;
    m_pThrusterRight = NULL;
    m_pBallExplode = NULL;
    SetupAI();

    m_RigidBody->Initialize(this, 200.0f, m_OBB.x * 2.0f, m_OBB.y * 2.0f, m_OBB.z * 2.0f);
    m_RigidBody->SetTerrainOffset(1.0f);

    // Set up the sound
    m_pExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_LARGE], 6);
    m_pEngineSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMY_ENGINE], 0);

    m_AwakenByPlayerRange = 350.0f;
    m_AwakenByEnemyRange = 50.0f;
}

CMeleeTank::CMeleeTank()
{

}

CMeleeTank::~CMeleeTank()
{

}

void CMeleeTank::Setup()
{
   m_AITimer = g_time;
   m_AI = true;
   m_DebugAI = false;
   m_Awake = false;
   m_Active = true;
   m_CurrentAggro = 0;
   m_BaseAggro = 0;
   m_OffsetFromPlayer.x = 0.0f;
   m_OffsetFromPlayer.y = 0.0f;
   m_OffsetFromPlayer.z = 0.0f;
   m_MainWeaponLastFired = g_time;
   m_RecentHitsByPlayer = 0;
   m_DefensivenessTimer = g_time;

   switch(g_GameOptions.GetDifficulty())
   {
      case EASY_DIFFICULTY:
      {
         m_HitPoints = 40;
         m_MaxHitPoints = 40;
         m_MeleeDamage = 75;
         m_ExplosionScale = 2.0f;
      }
         break;

      case MEDIUM_DIFFICULTY:
      {
         m_HitPoints = 50;
         m_MaxHitPoints = 50;
         m_MeleeDamage = 100;
         m_ExplosionScale = 2.5f;
      }
      	break;
      case HARD_DIFFICULTY:
      {
         m_HitPoints = 70;
         m_MaxHitPoints = 70;
         m_MeleeDamage = 125;
         m_ExplosionScale = 3.0f;
      }

      default:
         break;
   };

   // Setup thruster one
   m_pBallEnergy = g_ParticleManager.ObtainSystem();
   if(m_pBallEnergy)
      g_ParticleManager.CreateSystem(m_pBallEnergy, m_SparksPosition, &m_SparksPosition, g_ParticleManager.KAMI_SPARKS, LOD_BOTH);

   // Setup thruster
   m_pThrusterLeft = g_ParticleManager.ObtainSystem();
   if(m_pThrusterLeft)
      g_ParticleManager.CreateSystem(m_pThrusterLeft, m_ThrusterLeftPos, &m_ThrusterLeftPos, g_ParticleManager.KAMI_THRUSTER, LOD_BOTH);
      
   // Setup thruster
   m_pThrusterRight = g_ParticleManager.ObtainSystem();
   if(m_pThrusterRight)
      g_ParticleManager.CreateSystem(m_pThrusterRight, m_ThrusterRightPos, &m_ThrusterRightPos, g_ParticleManager.KAMI_THRUSTER, LOD_BOTH);   

   // Movement Info
   m_MaxVelocity  = 70.0f;    // Max velocity in meters per second
   m_AccelerationTime = 1.0f; // Time it takes to reach that velocity
   m_TurnSpeed = 20.0f;       // Turn speed
   m_RigidBody->SetParameters(m_MaxVelocity, m_AccelerationTime);
}

void CMeleeTank::SetupAI()
{

   // set up the AI
   InitStateMachine();
}

void CMeleeTank::Update(void)
{
   // Tank is dead
   if(m_HitPoints <= 0)
   {
      Die();
      return;
   }

   if (m_Awake)
   {
      m_RigidBody->StepIntegrate(g_FrameTime);
      FindSpherePositions();
      FindThrusterPositions();

      if (m_AI && g_GameOptions.GetAI())
      {
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



void CMeleeTank::Collide(const CGameObject *obj)
{
  // Object is the terrain 
   if(!obj)
   {
      // DO TERRAIN FOLLOWING
   }
   
   // Object is a game object
   else
   {
      if(obj->GetType() != OBJ_TREE && obj->GetType() != OBJ_TREE2 && obj->GetType() != OBJ_TREE3 &&
		  obj->GetType() != OBJ_ROCK1 && obj->GetType() != OBJ_ROCK2 && obj->GetType() != OBJ_ROCK3)
      {
         Die();
      }
   }    
}

void CMeleeTank::AimTowardPlayer()
{
   // aim toward player
   const Sphere* pPlayerSphere = g_Player.GetSphere();


   const CObjFrame* Frame = GetFrame(0);

   Ray         ShotTrajectory;
   Sphere      PlayerSphere = *pPlayerSphere;

   float       Scale = 1.0f;
   float       Dist = DistanceSquare(&pPlayerSphere->Center, &(GetSphere()->Center));

   if (Dist > 100.0f * 100.0f)
   {
      Scale = 5.0f;
   }
   else if (Dist < 80.0f * 80.0f)
      Scale = .5f;

   PlayerSphere.Radius *= Scale;

   Frame->GetPosition(&(ShotTrajectory.Origin));
   Frame->GetForward(&(ShotTrajectory.Direction));

   Vec3fNormalize(&(ShotTrajectory.Direction), &(ShotTrajectory.Direction));   

   if (!RayToSphere(&ShotTrajectory, &PlayerSphere))
   {

      const Sphere* pEnemySphere = GetSphere();

      CVector3f PlayerOrigin;
      CVector3f EnemyOrigin;
      CVector3f EToP;
      CVector3f EnemyLookAt;
      CVector3f Result;

      PlayerOrigin   = pPlayerSphere->Center;
      EnemyOrigin    = pEnemySphere->Center;

      PlayerOrigin.x += m_OffsetFromPlayer.x;
      PlayerOrigin.y += m_OffsetFromPlayer.y;
      PlayerOrigin.z += m_OffsetFromPlayer.z;

      // aim tank on y
      Vec3fSubtract(&EToP, &PlayerOrigin, &EnemyOrigin);
      Frame->GetForward(&EnemyLookAt);

      EnemyLookAt.y = 0.0f;
      EToP.y = 0.0f;

      Vec3fCrossProduct(&Result, &EnemyLookAt, &EToP);

      if (Result.y == 0)
      {
         // do nothing for now, good aim
      }
      else if (Result.y > 0)
         RBTurnRight(-m_TurnSpeed);
      else
         RBTurnRight(m_TurnSpeed);
   }

   // move forward

   RBMoveForward();

}


bool CMeleeTank::InDistanceFromPlayer(float Dist)
{
   bool bReturnVal = false;

   Dist = Dist * Dist;

   const Sphere* pPlayerSphere = g_Player.GetSphere();
   const Sphere* pMySphere = GetSphere();

   if (DistanceSquare(&pPlayerSphere->Center, &pMySphere->Center) < Dist)
      bReturnVal = true;

   return bReturnVal;
}


void CMeleeTank::ScanForPlayer()
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

      pt2 = pCurrentObj->GetSphere()->Center;

/*
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
*/

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


bool CMeleeTank::PlayerInFrontOfMe()
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

void CMeleeTank::Die(void)
{
   if (!m_Active)
      return;

   m_Active = false;

   // Setup enemy explosion
   //m_pExplosion = g_ParticleManager.ObtainSystem();
   //if(m_pExplosion)
   //   g_ParticleManager.CreateSystem(m_pExplosion, m_SparksPosition, NULL, g_ParticleManager.AURA, LOD_BOTH);

   // Setup enemy explosion
   m_pBallExplode = g_ParticleManager.ObtainSystem();
   if(m_pBallExplode)
      g_ParticleManager.CreateSystem(m_pBallExplode, *(GetFrame(0)->GetPosition()), NULL, g_ParticleManager.KAMI_EXPLODE, LOD_BOTH);

   // explosion damages all nearby units
   // if collision with another unit, explode, and damage them in explosion
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
         if (SphereToSphere(&MySphere, pCurrentObj->GetSphere()))
         {
            int Type = pCurrentObj->GetType();
            
            if (Type == OBJ_MEDTANK || Type == OBJ_KAMITANK || Type == OBJ_SCOUT || Type == OBJ_FTURRET || Type == OBJ_ETURRET ||
                Type == OBJ_HVYTANK || Type == OBJ_MSLETANK || Type == OBJ_PLAYER || Type == OBJ_BUILDING || Type == OBJ_BUILDING2 || Type == OBJ_BUILDING3)
            {
                pCurrentObj->TakeDamage(m_MeleeDamage, this);
            }             
         }

         pCurrentObj = pCurrentObj->m_GuyNextToMe;
      } while(pCurrentObj != NULL);
   }

   ClearWhoIsAroundMe(pFirstGuy);

   // Make explosion sound
   m_pExplosionSFX->Play3DAudio(40, 10000.0f, 40);
   CVector3f position;
	GetPosition(&position, 0);
   CVector3f velocity(0.0f, 0.0f, 0.0f);
   m_pExplosionSFX->UpdateSound(position, velocity);

   if(m_pBallEnergy)
      m_pBallEnergy->SetSystemAlive(false);
   if(m_pThrusterLeft)
      m_pThrusterLeft->SetSystemAlive(false);
   if(m_pThrusterRight)
      m_pThrusterRight->SetSystemAlive(false);

   m_pEngineSFX->StopAudio();

   Sleep();   
   RemoveFromQuadtree();
   GameMain.m_NumEnemiesAlive --;
   g_Player.m_EnemyMeleeTanksDestroyed++;
   DropPowerup(25.0f);
}

//////////////////////////////////////////////////////////////////////////
/// Find the position of the glowing sphere
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMeleeTank::FindSpherePositions()
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

   Vec3fScale(&vector, &vector, 4.5f);
   Vec3fScale(&right, &right, 0.0f);
   Vec3fScale(&left, &left, 0.0f);
   Vec3fScale(&up, &up, 0.0f);

   Vec3fAdd(&vector, &Position, &vector);
   Vec3fAdd(&m_SparksPosition, &vector, &right);
   Vec3fAdd(&m_SparksPosition, &m_SparksPosition, &up);
}

//////////////////////////////////////////////////////////////////////////
/// Find the position of the thrusters
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMeleeTank::FindThrusterPositions()
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

   Vec3fScale(&vector, &vector, -5.05f);
   Vec3fScale(&right, &right, 0.5f);
   Vec3fScale(&left, &left, -0.5f);
   Vec3fScale(&up, &up, 0.5f);

   Vec3fAdd(&vector, &Position, &vector);
   Vec3fAdd(&m_ThrusterLeftPos, &vector, &right);
   Vec3fAdd(&m_ThrusterRightPos, &vector, &left);
   Vec3fAdd(&m_ThrusterLeftPos, &m_ThrusterLeftPos, &up);
   Vec3fAdd(&m_ThrusterRightPos, &m_ThrusterRightPos, &up);
}

void CMeleeTank::Reset(void)
{
   
}

void CMeleeTank::TakeDamage(float damage, CGameObject * guywhohitme)
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

void CMeleeTank::InitStateMachine(void)
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
 
   m_StateMachine.AddState("PursueToOffset");
   m_StateMachine.AddState("Pursue");

   // add enter functions for each state
   m_StateMachine.AddOnEnter("PursueToOffset", MeleeTankPursueToOffsetOnEnter);
   m_StateMachine.AddOnEnter("Pursue", MeleeTankPursueOnEnter);

   // add exit functions for each state
   m_StateMachine.AddOnExit("PursueToOffset", MeleeTankPursueToOffsetOnExit);
   m_StateMachine.AddOnExit("Pursue", MeleeTankPursueOnExit);

   // add update functions for each state
   m_StateMachine.AddUpdate("PursueToOffset", MeleeTankPursueToOffsetUpdate);
   m_StateMachine.AddUpdate("Pursue", MeleeTankPursueUpdate);

   // set transitions between states
   m_StateMachine.AddTransitionCondition("PursueToOffset", "Pursue", MeleeTankPursueToOffsetToPursue);
   m_StateMachine.SetCurrentState(0);
}

void MeleeTankPursueToOffsetOnEnter(void* pVoidObj)
{
   CMeleeTank* pObj = (CMeleeTank*) pVoidObj;
   int Random = rand() % 3;
   
   switch (Random)
   {
      case 0:
      {
         pObj->m_OffsetFromPlayer.x = 0.0f;
         pObj->m_OffsetFromPlayer.y = 0.0f;
         pObj->m_OffsetFromPlayer.z = 0.0f;
      }
         break;

      case 1:
      {
         pObj->m_OffsetFromPlayer.x = -80.0f;
         pObj->m_OffsetFromPlayer.y = 0.0f;
         pObj->m_OffsetFromPlayer.z = 0.0f;
      }
         break;

      case 2:
      {
         pObj->m_OffsetFromPlayer.x = 80.0f;
         pObj->m_OffsetFromPlayer.y = 0.0f;
         pObj->m_OffsetFromPlayer.z = 0.0f;
      }
         break;

      default:
         break;
   };
}

void MeleeTankPursueOnEnter(void* pVoidObj)
{
   CMeleeTank* pObj = (CMeleeTank*) pVoidObj;
}

void MeleeTankPursueToOffsetOnExit(void* pVoidObj)
{
   CMeleeTank* pObj = (CMeleeTank*) pVoidObj;
}

void MeleeTankPursueOnExit(void* pVoidObj)
{
   CMeleeTank* pObj = (CMeleeTank*) pVoidObj;
}

void MeleeTankPursueToOffsetUpdate(void* pVoidObj)
{
   CMeleeTank* pObj = (CMeleeTank*) pVoidObj;

   pObj->AimTowardPlayer();
}

void MeleeTankPursueUpdate(void* pVoidObj)
{
   CMeleeTank* pObj = (CMeleeTank*) pVoidObj;

   pObj->AimTowardPlayer();
}

bool MeleeTankPursueToOffsetToPursue(void* pVoidObj)
{
   CMeleeTank* pObj = (CMeleeTank*) pVoidObj;

   if (pObj->m_OffsetFromPlayer.x == 0 && pObj->m_OffsetFromPlayer.y == 0 && pObj->m_OffsetFromPlayer.z == 0)
      return true;

   bool Fudge = false;

   // if within distance of player or within distance of offset go to puruse state
   const Sphere* pPlayerSphere = g_Player.GetSphere();
   const Sphere* pEnemySphere = pObj->GetSphere();
   CVector3f OffsetFromPlayerOrigin;

   // add offset to player's origin to get offset point
   OffsetFromPlayerOrigin   = pPlayerSphere->Center;
   OffsetFromPlayerOrigin.x += pObj->m_OffsetFromPlayer.x;
   OffsetFromPlayerOrigin.y += pObj->m_OffsetFromPlayer.y;
   OffsetFromPlayerOrigin.z += pObj->m_OffsetFromPlayer.z;

   if (DistanceSquare(&OffsetFromPlayerOrigin, &pEnemySphere->Center) < 30.0f * 30.0f)
   {
      pObj->m_OffsetFromPlayer.x = 0.0f;
      pObj->m_OffsetFromPlayer.y = 0.0f;
      pObj->m_OffsetFromPlayer.z = 0.0f;
      Fudge = true;
   }

   return Fudge;
}

void CMeleeTank::OnAwaken(void)
{
   // Play the engine sound when enemy is awakened
   GetPosition(&m_CurPosition, 0);
   m_LastPosition = m_CurPosition; 

   // Play the engine sound	when enemy is awakened
   m_pEngineSFX->Play3DAudio(20, 10000.0f, 100);
}