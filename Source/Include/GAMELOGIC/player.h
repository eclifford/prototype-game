/// \file player.h
/// Player object class
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic\gameobject.h"

class CAudio;
class CParticleSystem;

class CPlayer : public CGameObject
{
	public:
      CPlayer();
      
		~CPlayer();
		enum MainWeaponUpgrade{MW_Level1, MW_Level2, MW_Level3};
		enum MissileUpgrade{MS_Level1, MS_Level2, MS_Level3};
      enum TrkMissileUpgrade{TMS_Level1, TMS_Level2, TMS_Level3};
		enum IonCannon{InActive = 0, Charging, Ready};
      enum TurretState{STANDBY = 0, RECOIL, RESET};
      enum PlayerStates{FINE = 0, SLIGHTLY_DAMAGED, HEAVILY_DAMAGED, DEAD};
      enum Weapons{SEC_MISSILE = 0, SEC_TRK_MISSILE, SEC_IONCANNON, NUM_SEC_WEAPONS};

      int m_CurPlayerState;
      int m_CurMainWeaponUpgrade;
      int m_CurMissileUpgrade;
      int m_CurTrkMissileUpgrade;

      // Player Movement ///////////////////////////////////////////////////////

         // Jumpjets
         float m_JumpJetPowerInSeconds;
         float m_CurrentJumpJetPower;
         float m_JumpJetEnergyPerSec;

         // Boosters
         float m_MaxBoostVelocity;
         float m_BoostAcceleration;
         float m_CurBoostVelocity;
         float m_BoostEnergyPerSec;

      //////////////////////////////////////////////////////////////////////////
     
      // WEAPON SYSTEMS ////////////////////////////////////////////////////////
      
         int m_CurSecWeapon;
         int m_CurrentMissileBay;
         int m_NumMissiles;
         int m_NumTrackingMissiles;
         int m_ClusterSize;

         // Weapon damage
         float m_MinMainWeaponDamage;
         float m_MaxMainWeaponDamage;
         float m_CurMainWeaponDamage;
         float m_MinMainWeaponRadius;
         float m_MaxMainWeaponRadius;
         float m_CurMainWeaponRadius;
         float m_MissileSpeed;
         float m_PlasmaShotSpeed;
         float m_TrkMissileSpeed; 
         float m_MissileDamage;
         float m_TrkMissileDamage;
         float m_MissileRadius;
         float m_TrkMissileRadius;
         float m_MaxEnergyUsed;
         float m_MinEnergyUsed;
         float m_CurEnergyUsed;
         float m_TrkMissileAccuracy;

         // Weapon cooldown   
         float m_MainWeaponCooldown;
         float m_MainWeaponLastFired;
         float m_TrkMissileCoolDown;
         float m_TrkMissileLastFired;
         float m_MissileCoolDown;
         float m_MissileLastFired;
         float m_IonCoolDown;
         float m_IonLastFired;
         float m_IonCannonCountDown;

         // Charge settings
         float m_MaxChargeTime;
         float m_ChargeDamageStep;
         float m_ChargeRadiusStep;
         float m_ChargeColorStep;
         float m_SpreadStep;
         float m_ChargeEnergyStep;

         // Turret
         CVector3f m_TurretPosition;
         int   m_CurTurretState;
         float m_CurElevation;
         float m_MaxTurretElevation;
         float m_MinTurretElevation;
         float m_TurretTraversalSpeed;

      //////////////////////////////////////////////////////////////////////////
      
      // Sound Effects /////////////////////////////////////////////////////////

         CAudio * m_pMainWeaponBlast;
         CAudio * m_pMissilesFired;
         CAudio * m_pPlayerExplode;
         CAudio * m_pShieldAbsorb;
         CAudio * m_pEngine;
         CAudio * m_pJumpJet;
         CAudio * m_pHullHit;
         CAudio * m_pWeaponChange;
         CAudio * m_pCollide;
         CAudio * m_pLockOn;
         CAudio * m_pIonFlare;
         CAudio * m_pChargeRampUp;
         CAudio * m_pChargeFull;

      //////////////////////////////////////////////////////////////////////////

      // Particle EFfects //////////////////////////////////////////////////////

         CParticleSystem * smoke;
         CParticleSystem * fire; 
         CParticleSystem * thruster1;
         CParticleSystem * thruster2;
         CParticleSystem * explode;
         CParticleSystem * charge;

      //////////////////////////////////////////////////////////////////////////

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;
      CObjFrame MissilePodFrame;

      CObjFrame m_ShieldPodFrame;

      // Stats
      int m_HitsWithPlasma;
      int m_HitsWithMissiles;
      int m_HitsWithTrkMissiles;
      int m_ShotsFiredPlasma;
      int m_ShotsFiredMissiles;
      int m_ShotsFiredTrkMissiles;
      int m_EnemiesDestroyed;
      int m_PlayerTotalDamage;
      int m_NumPowerUpsCollected;

      int m_EnemyMedTanksDestroyed;
      int m_EnemyMslTanksDestroyed;
      int m_EnemyHvyTanksDestroyed;
      int m_EnemyMeleeTanksDestroyed;
      int m_EnemySpheresDestroyed;
      int m_EnemyFightersDestroyed;
      int m_EnemyScoutsDestroyed;

      int m_BuildingsLeft;
      int m_ConvoysLeft;

      float m_TimeMissionStarted;
      float m_TimeMissionEnded;

      bool m_InAir;
      bool m_InBoost;

      CGameObject* m_pTarget;
      bool m_ShowShield;	
      CQuadtreeRenderObj* m_pMissilePods;
	private:
      
      float m_turretpos;   // Current position of turret


      float m_StartShieldTime;
      //float m_ShieldAlpha;
      Point3f m_TurretShieldPosition;
      Point3f m_ShieldPodsPosition;
      CQuadtreeRenderObj* m_pShieldModel;
      CQuadtreeRenderObj* m_pShieldMissilePods;

      Point3f m_MissilePodsPosition;

		float m_Shield;
		float m_Hull;

      float m_MaxShield;
      float m_MaxHull;

      float m_Energy;
      float m_MaxEnergy;

      float m_ShieldRegenPerSec;
      float m_EnergyRegenPerSec;

		float m_TurnSpeed;
      float m_TurretAngle; 
      
      CVector3f m_Burner1;
      CVector3f m_Burner2;

		void MatchCameraOrientation(void);

	public:
 		
		void SetupPlayer();
		void FireMainWeapon(bool charged);
		void FireMissiles();
		void FireIonCannon();
      void Update(void);
		void Update2(void);
		void Collide(const CGameObject *obj);
		void Die(void);
      void Reset(void);
      void FindShotPosition(CObjFrame * frame);
      void FindThrusterPositions();
      void FireMissiles(int NumMissiles);
      void FireTrackingMissiles(int NumMissiles);
      void FindMissilePosition(CObjFrame * frame, float offsetx, float offsety);

      void TakeDamage(float damage, CGameObject* guywhohitme);
      void ShieldRegen(void);
      void UpdateTurretState(void);
      void UpdateCrossHair(void);
      void GetPlayerInput(void);
      void EnergyRegen(void);
      void UpdateShield(void);
      void IonHitsShield(int damage);

      int GetIonTimer(void);
		
		float GetShield(void)
		{	return m_Shield;	}

		void SetShield(unsigned int shield)
		{	m_Shield = (float)shield;	}

      float GetMaxShield(void)
      {  return m_MaxShield;  }

		float GetHull(void)
		{	return m_Hull;	}

      float GetMaxHull(void)
      {  return m_MaxHull; }

		void SetHull(unsigned int hull)
		{	m_Hull = (float)hull;	}

      int GetNumMissiles(void)
      {	return m_NumMissiles;	}

      int GetNumTrkMissiles(void)
      {	return m_NumTrackingMissiles;	}

		void GetMinMaxElevations(float &min, float &max) const
		{
			min = m_MinTurretElevation;
			max = m_MaxTurretElevation;
		}

      float GetEnergy(void)
      {	return m_Energy;	}

      float GetMaxEnergy(void)
      {	return m_MaxEnergy;	}

      void SetEnergy(float energy)
      {	m_Energy = energy;	}

};