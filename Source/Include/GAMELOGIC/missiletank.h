/// \file missiletank.h
/// Enemy missile tank unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/enemy.h"
#include "graphics/texturefonts.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;


class CMissileTank : public CEnemy
{
	private:

   protected:

	public:

      CVector3f m_OffsetFromTarget;
      CParticleSystem * explode;
      CParticleSystem * m_pThrusterRight;
      CParticleSystem * m_pThrusterLeft;

      // Missile Stats
      float m_MissileBayPos;
      float m_MissileCoolDown;
      float m_MissileCoolDown2;
      float m_MissileLastFired;
      float m_MissileLastFired2;
      float m_MissileSpeed;
      float m_MissileDamage;
      float m_MissileClusterSize;
      float m_NumMissiles;
      int   m_CurrentMissileBay;
      float m_MissileRadius;

      // Audio
      CAudio * m_pMissilesFired;
      CAudio * m_pExplosionSFX;
      CAudio * m_pEngineSFX;

      CVector3f m_ThrusterLeftPos;
      CVector3f m_ThrusterRightPos;

      // Used for sound 
      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

      CMissileTank(CMissileTank& MissileTank);
         
	   CMissileTank();
	  ~CMissileTank();	
      
      void Setup();
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
      void SetupAI();
      void TakeDamage(float damage, CGameObject * guywhohitme);
      void OnAwaken(void);
      void FindMissilePosition(CObjFrame * frame, float offsetx, float offsety);
      bool PlayerInFrontOfMe();
      bool InFrontOfMe(CGameObject* obj);
      void ScanForTarget();
      int  TargetRelativePositionWithLead();
      bool AimIsGood();
      bool AimIsGoodNowWithLeadTM();
      void AimAwayFromTarget();
      void AimTowardTarget();
      void AimTowardTargetNowWithLeadTM();
      void AimMissiles();
      bool InDistanceFromPlayer(float Dist);
      void MoveTowardTarget();
      bool AvoidCollision();
      void InitStateMachine();
      int  TargetRelativePosition();
      void FindShotPosition(CObjFrame * frame, float ForwardScale);
      void FireMissiles(int NumMissiles);
      void FireTrackingMissiles(int NumMissiles);
      bool InDistanceFromTarget(float Dist);
      void FindThrusterPositions();

   // on enter functions for each state
   friend void MissileTankAttackOnEnter(void* pVoidObj);
   friend void MissileTankRetreatOnEnter(void* pVoidObj);
   friend void MissileTankPursueOnEnter(void* pVoidObj);

   // add exit functions for each state
   friend void MissileTankAttackOnExit(void* pVoidObj);
   friend void MissileTankRetreatOnExit(void* pVoidObj);
   friend void MissileTankPursueOnExit(void* pVoidObj);


   // add update functions for each state
   friend void MissileTankAttackUpdate(void* pVoidObj);
   friend void MissileTankRetreatUpdate(void* pVoidObj);
   friend void MissileTankPursueUpdate(void* pVoidObj);

   // set transitions between states
   friend bool MissileTankAttackToRetreat(void* pVoidObj);
   friend bool MissileTankAttackToPursue(void* pVoidObj);
   friend bool MissileTankPursueToRetreat(void* pVoidObj);
   friend bool MissileTankPursueToAttack(void* pVoidObj);
   friend bool MissileTankRetreatToPursue(void* pVoidObj);
   friend bool MissileTankRetreatToAttack(void* pVoidObj);
};
