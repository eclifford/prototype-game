/// \file eturret.h
/// Enemy turret unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/enemy.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CEnemyTurret : public CEnemy
{
	private:

   protected:

	public:

      enum TurretState{STANDBY = 0, RECOIL, RESET};

      CGameObject*      m_LastHitBy;
      int               m_HitCounter;
      float             m_TurretPos;
      CVector3f         m_TurretPosition;
      int               m_CurTurretState;
      float             m_ShotDamage;
      float             m_MainWeaponCooldown;   ///< Time required to cool weapon down
      float             m_MainWeaponLastFired;  ///< Point in time weapon was fired last
      float             m_MaxTurnAngle;         ///< maneuverability of object
      float             m_AITimer;
      float             turretpos;              // Current position of turret
      float             velocity;               // Speed of recoil
      float             maxrecoil;              // Max/min distance of recoil
      bool              forward;                // Direction moving
      float             m_AttackRadius;
      float             m_TurretRotateBy;

      CEnemyTurret(CEnemyTurret& EnemyTurret) : CEnemy(EnemyTurret)
      {
         m_Type = OBJ_ETURRET;

          // Setup Audio
		   m_ExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_MED], 5);
         m_MainWeaponSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LASERSHOT], 4);
      }

		CEnemyTurret();
		~CEnemyTurret();	

      int m_CurTurret;

      CParticleSystem * m_MainWeaponFlash;
      CParticleSystem * m_Explode;
      CAudio * m_MainWeaponSFX;
      CAudio * m_ExplosionSFX;
      
      void Setup();
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);

      void TakeDamage(float damage, CGameObject * guywhohitme);
      void FollowTerrain(void);
      void AimTowardTarget(void);
      void Fire();
      void AimTurret();
      void FindShotPosition(CObjFrame * frame, float ForwardScale);
      void TargetRelativePosition(float& RotY, float& RotX);
		bool AimIsGood(void);
      void ScanForTarget();
      bool InFrontOfMe(CGameObject* obj);
      bool PlayerInFrontOfMe();
      void OnAwaken();
      bool InDistanceFromPlayer(float Dist);
      bool InDistanceFromTarget(float Dist);
};
