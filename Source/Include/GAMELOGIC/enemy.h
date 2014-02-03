/// \file enemy.h
/// This is the baseclass for all enemies in the game
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/gameobject.h"
#include "ai/FSM.h"
#include "core/oglwindow.h"
#include "gamelogic/gameoptions.h"

extern float g_time;

class CEnemy : public CGameObject
{
	private:

   protected:
      float          m_HitPoints;
      float          m_MaxHitPoints;
      CFSM           m_StateMachine;
      float          m_CurrentAggro;         ///< Current level of aggression in enemy unit
      float          m_BaseAggro;            ///< Starting aggression level of enemy unit
      float          m_MainWeaponCooldown;   ///< Time required to cool weapon down
      float          m_MainWeaponLastFired;  ///< Point in time weapon was fired last
      int            m_RecentHitsByPlayer;   ///< Number of times unit has been hit by player
      float          m_DefensivenessTimer;   ///< For getting over the shock of being pummeled
      bool           m_DebugAI;              ///< used for debugging
      bool           m_StrafeDirection;      ///< used to remember where unit is strafing
      float          m_MaxTurnAngle;         ///< maneuverability of object
      float          m_AITimer;
      bool           m_AI;
      float          m_TargetOffset;
      bool           m_Awake;
      int            m_HitCounter;

      float m_ForwardThrust;
      float m_BackwardThrust;
      float m_LeftThrust;
      float m_RightThrust;
      float m_TurnSpeed;

	public:
      CEnemy(CEnemy& Enemy) : CGameObject(Enemy)
      {
         m_HitPoints = Enemy.m_HitPoints;
         m_MaxHitPoints = Enemy.m_MaxHitPoints;
         m_CurrentAggro = 0;
         m_BaseAggro = Enemy.m_BaseAggro;
         m_MainWeaponLastFired = Enemy.m_MainWeaponLastFired;
         m_RecentHitsByPlayer = 0;
         m_DefensivenessTimer = Enemy.m_DefensivenessTimer;
         m_DebugAI = false;
         m_AITimer = g_time;
         m_AI = true;
         m_RadarType = RADAR_ENEMY;
         m_LastHitBy = NULL;
         m_HitCounter = 0;
      }

      CGameObject * m_LastHitBy;
      CGameObject * m_CurrentTarget;

      float m_AwakenByPlayerRange;
      float m_AwakenByEnemyRange;

		CEnemy();
		~CEnemy();
      virtual void InitStateMachine(void);
      virtual void DetermineAggro(void);
      virtual void Update(void) = 0;
		virtual void Collide(const CGameObject *obj) = 0;
		virtual void Die(void) = 0;
		virtual void Reset(void) = 0;
      virtual void TakeDamage(float damage, CGameObject * guywhohitme) { }
      virtual void FindShotPosition(CObjFrame * frame);
      void ClearWhoIsAroundMe(CGameObject* obj);
      void DropPowerup(float percent);
      CGameObject* GetWhoIsAroundMe();
      CGameObject* GetWhoIsAroundMe(float Scale);
      CGameObject* GetWhoIsAroundMe2(float Size);
      void  Awaken();
      void  Sleep();
      void  AIOff();
      void  AIOn();
      float GetAITimer();
      void  ResetMainWeaponTimer();
      float GetMainWeaponLastFired();
      int   GetRecentHitsByPlayer();
      void  ResetAITimer();
      void  DebugAIOn();
      void  DebugAIOff();
      bool  IsDebugAI();
      int   GetCurrentAggro();
      int   InPlayersAim();
      bool  GetStrafeDirection();
      void  StrafeDirectionLeft();
      void  StrafeDirectionRight();
      bool  IsAwake();
      void  TargetEnemy();

      float GetAwakenPlayerRadius(void)
      {
         return m_AwakenByPlayerRange;
      }

      float GetAwakenEnemyRadius(void)
      {
         return m_AwakenByEnemyRange;
      }
};





















