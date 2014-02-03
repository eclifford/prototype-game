/// \file scout.h
/// Enemy scout unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/enemy.h"
#include "graphics/particle.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CScout : public CEnemy
{
   private:
      float m_TurretPos;
      float m_ShotDamage;
      CVector3f Velocity;
      CVector3f m_OffsetFromTarget;
      CParticleSystem * explode;      
   protected:

   public:
      CScout(CScout& scout);
      
      CScout();
      ~CScout();	

      // Turret information
      enum TurretState{STANDBY = 0, RECOIL, RESET};
      CVector3f m_TurretPosition;
      int m_CurTurretState;

      CParticleSystem * m_pExplosion;
      CParticleSystem * m_pThrusterLeft;
      CParticleSystem * m_pThrusterRight;
      CParticleSystem * m_MainWeaponFlash;

      CVector3f m_ThrusterLeftPos;
      CVector3f m_ThrusterRightPos;

      CAudio * m_pExplosionSFX;
      CAudio * m_pEngineSFX;
      CAudio * m_pMainGunsSFX;

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

      float m_ScoutTimer;

      void Setup();
      void SetupAI();
      void Update(void);
      void Collide(const CGameObject *obj);
      void Die(void);
      void Reset(void);
	   void OnAwaken(void);
      void TakeDamage(float damage, CGameObject * guywhohitme);
      void AimTowardTarget();
      bool CollisionWithPlayerSphereScaledBy(float Scale);
      bool InDistanceFromTarget(float Dist);
      bool InDistanceFromPlayer(float Dist);
      void FindShotPosition(CObjFrame * frame);
      void MoveTowardTarget();
      bool AvoidCollision();
      void InitStateMachine();
      bool AimIsGood();
      void FireMainWeapon();
      int  TargetRelativePosition();
      void AimTurret();
      void FindThrusterPositions();
      void UpdateTurretState(void);
      bool PlayerInFrontOfMe();
      void ScanForTarget();
      bool InFrontOfMe(CGameObject* obj);

      // state update
      friend void ScoutAimUpdate(void* pVoidObj);
      friend void ScoutFireUpdate(void* pVoidObj);
      friend void ScoutMoveForwardUpdate(void* pVoidObj);

      // state on enter
      friend void ScoutAimOnEnter(void* pVoidObj);
      friend void ScoutFireOnEnter(void* pVoidObj);
      friend void ScoutMoveForwardOnEnter(void* pVoidObj);

      // state on exit
      friend void ScoutAimOnExit(void* pVoidObj);
      friend void ScoutFireOnExit(void* pVoidObj);
      friend void ScoutMoveForwardOnExit(void* pVoidObj);

      // transition functions from state to state
      friend bool ScoutAimToMoveForward(void* pVoidObj);
      friend bool ScoutAimToFire(void* pVoidObj);
      friend bool ScoutFireToMoveForward(void* pVoidObj);
      friend bool ScoutMoveForwardToAim(void* pVoidObj);
};
