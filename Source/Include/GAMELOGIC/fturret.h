/// \file fturret.h
/// Friendly turret unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/friendly.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CFriendlyTurret : public CFriendly
{
	private:

   protected:

	public:

      CGameObject*      m_LastHitBy;
      CGameObject*      m_CurrentTarget;
      int               m_HitCounter;
      float             m_TurretPos;
      CVector3f         m_TurretPosition;
      int               m_CurTurretState;
      float             m_ShotDamage;
      float             m_MainWeaponCooldown;   ///< Time required to cool weapon down
      float             m_MainWeaponLastFired;  ///< Point in time weapon was fired last
      float             m_MaxTurnAngle;         ///< maneuverability of object
      float             m_TurnSpeed;
      float             m_AITimer;
      float             turretpos;              // Current position of turret
      float             velocity;               // Speed of recoil
      float             maxrecoil;              // Max/min distance of recoil
      bool              forward;                // Direction moving
      float             m_AttackRadius;
      bool              m_AI;

      enum TurretState{STANDBY = 0, RECOIL, RESET};

      CFriendlyTurret(CFriendlyTurret& FriendlyTurret) : CFriendly(FriendlyTurret)
      {
         m_Type = OBJ_FTURRET;
			m_StaticObj = true;

         // Setup Audio
		   m_ExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_MED], 5);
         m_MainWeaponSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_FRIENDLY_TURRET], 4);
         m_MainWeaponSFX2 = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_FRIENDLY_TURRET], 4);
      }

		CFriendlyTurret();
		~CFriendlyTurret();	


      CParticleSystem * m_MainWeaponFlash;
      CParticleSystem * m_Explode;

      CAudio * m_MainWeaponSFX;
      CAudio * m_MainWeaponSFX2;
      CAudio * m_ExplosionSFX;

      void Setup();
      void Update(void);
	  void Collide(const CGameObject *obj);
	  void Die(void);
	  void Reset(void);
      void TakeDamage(float damage, CGameObject * guywhohitme);
      CGameObject* GetWhoIsAroundMe(float Size);
      void ClearWhoIsAroundMe(CGameObject* obj);
      void TargetChoikin();
      void AimTowardTarget(void);
      void Fire();
      void AimTurret();
      void FindShotPosition(CObjFrame * frame, float ForwardScale);
      void FollowTerrain(void);
      void TargetRelativePosition(float& RotY, float& RotX);
      void UpdateTurretState(void);
	  bool AimIsGood(void);
      bool InFrontOfMe(CGameObject* obj);
};