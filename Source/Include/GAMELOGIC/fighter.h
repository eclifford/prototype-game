/// \file fighter.h
/// Enemy figther unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/enemy.h"
#include "graphics/texturefonts.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CFighter : public CEnemy
{
	private:

   protected:

	public:
      CFighter(CFighter& Fighter) : CEnemy(Fighter)
      {
         m_Type = OBJ_FIGHTER;
         m_RadarType = RADAR_ENEMY;
         
         m_AwakenByPlayerRange = 350.0f;
         m_AwakenByEnemyRange = 50.0f;
         SetupAI();

         // Set up the sound
         m_ExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_LARGE], 6);
         m_EngineSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMY_ENGINE], 0);

			m_Dying = false;
			m_DieNow = false;
			m_DyingTime = 0.0f;
      }

		CFighter();
		~CFighter();	

      float m_UpDownSpeed;
      float m_DesiredAltitude;
      float m_DiveTime;
      float m_RiseTime;
      float m_Pitch;
      float m_Roll;
      float m_MaxPitch;
      float m_MaxRoll;
      float m_MaxPitchAngle;
      float m_MaxYawAngle;
      float m_MaxRollAngle;
      float m_ShotDamage;
      float m_WanderToPursueDist;
      float m_DiveAtTargetDist;
      float m_BeginRiseDist;
      float m_Speed;
      float m_BombCoolDown;
      float m_BombLastDropped;

		bool	m_Dying;
		bool	m_DieNow;
		float m_SpinDir;
		float m_DyingTime;

      CVector3f m_OffsetFromPlayer;
      CParticleSystem * m_Explosion;
      CParticleSystem * m_Smoke;
      CParticleSystem * m_OnFire;
      CParticleSystem * m_pThruster1;
      CParticleSystem * m_pThruster2;

      CAudio * m_ExplosionSFX;
      CAudio * m_EngineSFX;

      // Used for sound 
      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

      CVector3f m_ThrusterLeftPos;
      CVector3f m_ThrusterRightPos;

      CVector3f m_WingPos;

      void Setup();
      void SetupAI();
		void UpdateDying(void);
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);

      void TakeDamage(float damage, CGameObject * guywhohitme);
      bool InDistanceFromPlayer(float Dist);
      void FindShotPosition(CObjFrame * frame, float ForwardScale);
      void MoveTowardPlayer();
      void InitStateMachine();
      void BombsAway();
      int  PlayerRelativePosition();
      void SplashDamage();
      void TargetRelativePosition(float& RotY, float& RotX);
      bool InFrontOfMe(CGameObject* obj);
      void AimYawTowardTarget();
      bool AimIsGood();
      void FlyTowardTarget();
      bool AvoidCollision();
      void AimAwayFromTarget();
      void AdjustAltitude();
      void FindThrusterPositions();
      void FindWingPosition();

   // enter functions for each state
      friend void FighterBombTargetOnEnter(void* pVoidObj);
      friend void FighterWanderOnEnter(void* pVoidObj);
      friend void FighterMoveTowardTargetOnEnter(void* pVoidObj);

   // exit functions for each state
      friend void FighterBombTargetOnExit(void* pVoidObj);
      friend void FighterWanderOnExit(void* pVoidObj);
      friend void FighterMoveTowardTargetOnExit(void* pVoidObj);

   // update functions for each state
      friend void FighterBombTargetUpdate(void* pVoidObj);
      friend void FighterWanderUpdate(void* pVoidObj);
      friend void FighterMoveTowardTargetUpdate(void* pVoidObj);

   // transitions between states
      friend bool FighterWanderToMoveTowardTarget(void* pVoidObj);
      friend bool FighterMoveTowardTargetToBombTarget(void* pVoidObj);
      friend bool FighterBombTargetToWander(void* pVoidObj);
};