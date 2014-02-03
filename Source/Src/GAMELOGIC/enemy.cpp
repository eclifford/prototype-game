/// \file enemy.cpp
/// This is the base class for all enemies in the game
//////////////////////////////////////////////////////////////////////////

#include "gamelogic\enemy.h"
#include "core\oglwindow.h"
#include "input\inputmanager.h"
#include "core\gamemain.h"
#include "math3d\intersection.h"
#include "graphics\drawshape.h"
#include "gamelogic\objectfactory.h"
#include "gamelogic\player.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CPlayer g_Player;
extern CGameMain GameMain;
extern CGameObject* g_CollisionAvoidanceList;
extern CObjectFactory g_ObjectFactory;

/////////////////////////////////
/*/ Global Variables          /*/ 
/////////////////////////////////

int g_AwakeEnemies = 0;

//////////////////////////////////////////////////////////////////////////
/// Constructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CEnemy::CEnemy()
{

}

//////////////////////////////////////////////////////////////////////////
/// Destructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CEnemy::~CEnemy()
{

}
//////////////////////////////////////////////////////////////////////////
/// Init State machine
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CEnemy::InitStateMachine(void)
{


}

//////////////////////////////////////////////////////////////////////////
/// Determine enemies aggro
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CEnemy::DetermineAggro(void)
{


}

int CEnemy::GetCurrentAggro(void)
{
   return int (100.0f * m_CurrentAggro);
}


void  CEnemy::DebugAIOn()
{
   m_DebugAI = true;
}

void CEnemy::Awaken()
{
   if (m_Awake || !m_Active)
      return;

   TargetEnemy();
   GameMain.AddToCollisionAvoidanceList(this);
   m_Awake = true;
   g_AwakeEnemies ++;
}

void CEnemy::Sleep()
{
   m_Awake = false;
   g_AwakeEnemies --;

   if (g_AwakeEnemies < 0)
      g_AwakeEnemies = 0;
}

void CEnemy::AIOn()
{
   m_AI = true;
}


void CEnemy::AIOff()
{
   m_AI = false;
}

void  CEnemy::DebugAIOff()
{
   m_DebugAI = false;
}



bool  CEnemy::IsDebugAI()
{
   return m_DebugAI;
}

float CEnemy::GetMainWeaponLastFired()
{
   return m_MainWeaponLastFired;
}


void CEnemy::ResetMainWeaponTimer()
{
   m_MainWeaponLastFired = g_time;
}

void CEnemy::TargetEnemy()
{
   CGameObject* pCurrentObj = GetWhoIsAroundMe2(400.0f);
   CGameObject* pFirstGuy = pCurrentObj;

   m_CurrentTarget = &g_Player;

   float Dist = DistanceSquare(&(GetSphere()->Center), &(g_Player.GetSphere()->Center));
   float TempDist;

   if (pCurrentObj != NULL)
   {
      do 
      {  
         switch(pCurrentObj->GetType())
         {
            case OBJ_BUILDING:
            case OBJ_BUILDING2:
            case OBJ_BUILDING3:
            {
               TempDist = DistanceSquare(&(GetSphere()->Center), &(pCurrentObj->GetSphere()->Center));
               if (TempDist < Dist)
               {
                  m_CurrentTarget = pCurrentObj;
                  Dist = TempDist;
               }

               break;
            };

            case OBJ_FTURRET:
            {
               TempDist = DistanceSquare(&(GetSphere()->Center), &(pCurrentObj->GetSphere()->Center));
               if (TempDist < Dist)
               {
                  m_CurrentTarget = pCurrentObj;
                  Dist = TempDist;
               }
               break;
            };

            case OBJ_CONVOY:
            {
               TempDist = DistanceSquare(&(GetSphere()->Center), &(pCurrentObj->GetSphere()->Center));
               if (TempDist < Dist)
               {
                  m_CurrentTarget = pCurrentObj;
                  Dist = TempDist;
               }
               break;
            };

            default:
               break;
         };

         pCurrentObj = pCurrentObj->m_GuyNextToMe;

      } while (pCurrentObj != NULL);
   }

   ClearWhoIsAroundMe(pFirstGuy);
}

int   CEnemy::InPlayersAim()
{

   int   ReturnValue;
   
   const Sphere* MySphere = m_RenderObj.GetSphere();

   Ray         PlayerShotTrajectory;
   CObjFrame   PlayerFrame;

   g_Player.FindShotPosition(&PlayerFrame);

   PlayerFrame.GetPosition(&(PlayerShotTrajectory.Origin));
   PlayerFrame.GetForward(&(PlayerShotTrajectory.Direction));

   Vec3fNormalize(&(PlayerShotTrajectory.Direction), &(PlayerShotTrajectory.Direction));   

   if (RayToSphere(&PlayerShotTrajectory, MySphere))
   {
      if((rand() % 100) > 50)
         ReturnValue = 1;
      else
         ReturnValue = -1;
   }
   else
      ReturnValue = 0;

   if (ReturnValue == 1)
      int a = 0;

   return ReturnValue;
}


CGameObject* CEnemy::GetWhoIsAroundMe(float Scale)
{
   CGameObject* firstAllyInProximity = NULL;
   CGameObject* currentCloseGuy;
   CGameObject* currentObj = g_CollisionAvoidanceList;

   if (g_CollisionAvoidanceList == NULL)
      return NULL;

   const Sphere* es = GetSphere();
   Sphere MySphere = *es;

   MySphere.Radius *= Scale;

   do 
   {
      if (currentObj == this)
      {
         currentObj = currentObj->m_CollisionAvoidanceListNext;
         continue;
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

CGameObject* CEnemy::GetWhoIsAroundMe2(float Size)
{
   CGameObject* firstAllyInProximity = NULL;
   CGameObject* currentCloseGuy;
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

CGameObject* CEnemy::GetWhoIsAroundMe()
{
   return GetWhoIsAroundMe(3.0f);
}


void CEnemy::ClearWhoIsAroundMe(CGameObject* obj)
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


void CEnemy::FindShotPosition(CObjFrame * frame)
{
}


bool CEnemy::GetStrafeDirection()
{
   return m_StrafeDirection;
}


void  CEnemy::StrafeDirectionLeft()
{
   m_StrafeDirection = false;
}


void  CEnemy::StrafeDirectionRight()
{
   m_StrafeDirection = true;
}


float CEnemy::GetAITimer()
{
   return m_AITimer;
}

void CEnemy::ResetAITimer()
{
   m_AITimer = g_time;
}

int CEnemy::GetRecentHitsByPlayer()
{
   return m_RecentHitsByPlayer;
}

bool CEnemy::IsAwake()
{
   return m_Awake;
}

void CEnemy::DropPowerup(float percent)
{
   if(rand() % 100 <= percent)
   {
      int i = rand() % 3;
      switch(i)
      {
         case 0:
         {
            CShieldPowerUp * pwrup = g_ObjectFactory.CreateShieldPowerUp();
            pwrup->Setup();
            pwrup->SetFrame(GetFrame(0), 0);
            pwrup->m_DeathAge = g_time + 10.0f;
         }
         break;

         case 1:
         {
            CMissilePack * pwrup = g_ObjectFactory.CreateMissilePack();
            pwrup->Setup();
            pwrup->SetFrame(GetFrame(0), 0);
            pwrup->m_DeathAge = g_time + 10.0f;
         }
         break;

         case 2:
         {
            CTrackingMissilePack * pwrup = g_ObjectFactory.CreateTrackingMissilePack();
            pwrup->Setup();
            pwrup->SetFrame(GetFrame(0), 0);
            pwrup->m_DeathAge = g_time + 10.0f;
         }
         break;
      }
   }
}