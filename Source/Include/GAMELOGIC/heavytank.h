/// \file heavytank.h
/// Enemy heavytank unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/enemy.h"
#include "graphics/texturefonts.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CHeavyTank : public CEnemy
{
	private:

   protected:

	public:

      float m_TurretPos;
      float m_ShotDamage;
      float m_ExplosionScale;
      CVector3f m_OffsetFromTarget;
      CParticleSystem * explode;
      bool m_Suicidal;
      float m_MeleeDamage;

      // Missile Stats
      float m_MissileCoolDown;
      float m_MissileLastFired;
      float m_MissileSpeed;
      float m_MissileDamage;
      float m_MissileClusterSize;
      float m_NumMissiles;
      int m_CurrentMissileBay;
      float m_MissileRadius;

      // Audio
      CAudio * m_pMissilesFired;
      CAudio * m_pExplosionSFX;
      CAudio * m_pEngineSFX;
      CAudio * m_pMainGunsSFX;

      // Used for sound 
      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

      CHeavyTank(CHeavyTank& HeavyTank);

      // Turret information
      enum TurretState{STANDBY = 0, RECOIL, RESET};
      CVector3f m_TurretPosition;
      int m_CurTurretState;

	   CHeavyTank();
		~CHeavyTank();
      
      void Setup();
      void SetupAI();
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
	  void OnAwaken(void);
      void TakeDamage(float damage, CGameObject * guywhohitme);
      bool AimIsGood();
      bool AimIsGoodNowWithLeadTM();
      void AimTurret();
      void AimTowardTarget();
      void AimTowardTargetNowWithLeadTM();
      bool InDistanceFromTarget(float Dist);
      void FindShotPosition(CObjFrame * frame, float ForwardScale);
      void MoveTowardTarget();
      bool AvoidCollision();
      void InitStateMachine();
      void FireMainWeapon();
      int  TargetRelativePosition();
      void DetermineAggro();
      void SplashDamage();     
      void UpdateTurretState(void);
      void FireMissiles(int NumMissiles);
      void FindMissilePosition(CObjFrame * frame, float offsetx, float offsety);
      void OrientMissileBays(void);
      bool PlayerInFrontOfMe();
      void ScanForTarget();
      int  TargetRelativePositionWithLead();
      bool InFrontOfMe(CGameObject* obj);
      bool InDistanceFromPlayer(float Dist);
      void SuicideRun();

      friend void HeavyTankAimOnEnter(void* pVoidObj);
      friend void HeavyTankFireOnEnter(void* pVoidObj);
      friend void HeavyTankDocileOnEnter(void* pVoidObj);
      friend void HeavyTankPursueOnEnter(void* pVoidObj);
      friend void HeavyTankAttackOnEnter(void* pVoidObj);
      friend void HeavyTankGRRROnEnter(void* pVoidObj);

      friend void HeavyTankAimUpdate(void* pVoidObj);
      friend void HeavyTankFireUpdate(void* pVoidObj);
      friend void HeavyTankDocileUpdate(void* pVoidObj);
      friend void HeavyTankPursueUpdate(void* pVoidObj);
      friend void HeavyTankAttackUpdate(void* pVoidObj);
      friend void HeavyTankGRRRUpdate(void* pVoidObj);

      friend void HeavyTankAimOnExit(void* pVoidObj);
      friend void HeavyTankFireOnExit(void* pVoidObj);
      friend void HeavyTankDocileOnExit(void* pVoidObj);
      friend void HeavyTankPursueOnExit(void* pVoidObj);
      friend void HeavyTankAttackOnExit(void* pVoidObj);
      friend void HeavyTankGRRROnExit(void* pVoidObj);

      friend bool HeavyTankAttackToGRRR(void* pVoidObj);
      friend bool HeavyTankAttackToPursue(void* pVoidObj);
      friend bool HeavyTankPursueToGRRR(void* pVoidObj);
      friend bool HeavyTankPursueToAttack(void* pVoidObj);
      friend bool HeavyTankAimToFire(void* pVoidObj);
      friend bool HeavyTankAimToDocile(void* pVoidObj);
      friend bool HeavyTankDocileToAim(void* pVoidObj);
      friend bool HeavyTankFireToAim(void* pVoidObj);
};


















