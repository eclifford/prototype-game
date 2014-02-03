/// \file boss.h
/// Enemy boss unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/enemy.h"
#include "graphics/particle.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CBoss : public CEnemy
{
   private:
      CFSM* m_Rise;
      bool  m_GoingDown;
      float m_ClusterMissileDamage;
      float m_ClusterMissileRadius;
      float m_HomingMissileDamage;
      float m_HomingMissileRadius;
      int   m_NumClusterMissiles;
      int   m_NumHomingMissiles;
      int   m_CurrentMissileBay;
      float m_Altitude;
      float m_StrafeDist;
      float m_StrafeDistLimit;
      float m_DesiredAltitude;
      float m_TurretPos;
      float m_ShotDamage;
      float m_PitchTurnSpeed;
      float m_YawTurnSpeed;
      float m_MaxRotateX;
      float m_ElevationSpeed;
      float m_ElevationSpeed2;
      float m_Speed;
      float m_Pitch;
      float m_RiseAltitude;
      float m_ClusterMissilesLastFired;
      float m_HomingMisslesLastFired;
      float m_HomingMissileFirePer;
      float m_ClusterMissileFirePer;
      float m_RiseAttackTime;
      float m_BarrageAttackTime;
      bool  m_StrafeRight;
      float m_StrafeTime;
      float m_StrafeTimer;

      CVector3f Velocity;
      CVector3f m_OffsetFromTarget;   
      
   protected:

   public:
      CBoss(CBoss& boss) : CEnemy(boss)
      {

      }
      CBoss();
      ~CBoss();	

      CParticleSystem * m_pExplosion;
      CParticleSystem * m_pThrusterLeft;
      CParticleSystem * m_pThrusterRight;
      CParticleSystem * m_MainWeaponFlash;

      CVector3f m_ThrusterLeftPos;
      CVector3f m_ThrusterRightPos;

      CAudio * m_pExplosionSFX;
      CAudio * m_pEngineSFX;
      CAudio * m_pMainGunsSFX;
      CAudio * m_pMissilesFired;

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

      float m_BossTimer;

      void Setup();
      void SetupAI();
      void InitStateMachine(void);
      void Update(void);
      void Collide(const CGameObject *obj);
      void Die(void);
      void Reset(void);
	   void OnAwaken(void);
      void TakeDamage(float damage, CGameObject * guywhohitme);
      void AdjustAltitude();
      void AdjustAltitude(float level);
      bool InDistanceFromPlayer(float Dist);
      void AimXYTowardPlayer();
      void AimYTowardPlayer();
      int TargetRelativePosition();
      void AdjustPitchToDefault();
      void FirePlasma();
      void FireClusterMissiles();
      void FireHomingMissiles();
      void TargetRelativePosition(float& RotY, float& RotX);
      bool InFrontOfMe(CGameObject* obj);
      bool AimIsGood();
      bool HasRisen();
      bool HasFallen();
      void FindThrusterPositions();

// sub-state2 transition
      friend bool BossGoUpToAttack(void* pVoidObj);
      friend bool BossAttackToGoDown(void* pVoidObj);

// sub-state2 update
      friend void BossGoUpUpdate(void* pVoidObj);
      friend void BossAttackUpdate(void* pVoidObj);
      friend void BossGoDownUpdate(void* pVoidObj);

// sub-state2 enter
      friend void BossGoUpOnEnter(void* pVoidObj);
      friend void BossAttackOnEnter(void* pVoidObj);
      friend void BossGoDownOnEnter(void* pVoidObj);

// sub-state2 exit
      friend void BossGoUpOnExit(void* pVoidObj);
      friend void BossAttackOnExit(void* pVoidObj);
      friend void BossGoDownOnExit(void* pVoidObj);

// enter functions for each state
      friend void BossAvoidPlayerOnEnter(void* pVoidObj);
      friend void BossRiseAttackOnEnter(void* pVoidObj);
      friend void BossBarrageAttackOnEnter(void* pVoidObj);

// exit functions for each state
      friend void BossAvoidPlayerOnExit(void* pVoidObj);
      friend void BossRiseAttackOnExit(void* pVoidObj);
      friend void BossBarrageAttackOnExit(void* pVoidObj);

// update functions for each state
      friend void BossAvoidPlayerUpdate(void* pVoidObj);
      friend void BossRiseAttackUpdate(void* pVoidObj);
      friend void BossBarrageAttackUpdate(void* pVoidObj);

// transitions between states
      friend bool BossAvoidPlayerToRiseAttack(void* pVoidObj);
      friend bool BossAvoidPlayerToBarrageAttack(void* pVoidObj);
      friend bool BossRiseAttackToAvoidPlayer(void* pVoidObj);
      friend bool BossRiseAttackToBarrageAttack(void* pVoidObj);
      friend bool BossBarrageAttackToAvoidPlayer(void* pVoidObj);

};
