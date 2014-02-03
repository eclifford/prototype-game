/// \file medtank.h
/// Enemy medtank unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include "gamelogic/enemy.h"
#include "graphics/texturefonts.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

using namespace std;
extern CAudioManager    g_AudioManager;


extern CGameOptions g_GameOptions;

class CMedTank : public CEnemy
{
	private:

   protected:

	public:
      float m_TurretPos;
      float m_StrafeTimer;
      float m_ShotDamage;
      CVector3f m_OffsetFromTarget;
      CParticleSystem * explode;      
      CMedTank(CMedTank& MedTank);

      // Audio
      CAudio * m_pExplosionSFX;
      CAudio * m_pEngineSFX;
      CAudio * m_pMainGunsSFX;

      // Used for sound 
      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

      // Turret information
      enum TurretState{STANDBY = 0, RECOIL, RESET};
      CVector3f m_TurretPosition;
      int m_CurTurretState;

	   CMedTank();
		~CMedTank();
      void InitStateMachine(void);
      void DetermineAggro(void);
      void Print(string txt);
      void TextLoc(CVector2f vec);
      void Setup();
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
      void SetupAI();
      void FindShotPosition(CObjFrame * frame);
      void FireMainWeapon();
      void TakeDamage(float damage, CGameObject * guywhohitme);
      void AimTowardTarget();
      void TurnToAvoid(CGameObject* obj);
      bool AvoidCollision();
      void UpdateTurretState(void);
      void ScanForPlayer();
      bool PlayerInFrontOfMe();
      bool InDistanceFromTarget(float Dist);
      bool InDistanceFromPlayer(float Dist);
   	void OnAwaken(void);
      bool InFrontOfMe(CGameObject* obj);
      bool TargetInFrontOfMe();

      friend bool MedTankFightToRetreat(void* pVoidObj);
      friend bool MedTankFightToPursue(void* pVoidObj);
      friend bool MedTankRetreatToFight(void* pVoidObj);
      friend bool MedTankRetreatToPursue(void* pVoidObj);
      friend bool MedTankPursueToRetreat(void* pVoidObj);
      friend bool MedTankPursueToFight(void* pVoidObj);
      friend void MedTankFightOnEnter(void* pVoidObj);
      friend void MedTankRetreatOnEnter(void* pVoidObj);
      friend void MedTankPursueOnEnter(void* pVoidObj);
      friend void MedTankFightOnExit(void* pVoidObj);
      friend void MedTankRetreatOnExit(void* pVoidObj);
      friend void MedTankPursueOnExit(void* pVoidObj);
      friend void MedTankFightUpdate(void* pVoidObj);
      friend void MedTankRetreatUpdate(void* pVoidObj);
      friend void MedTankPursueUpdate(void* pVoidObj);
      friend bool MedTankStillToStrafe(void* pVoidObj);
      friend bool MedTankStrafeToStill(void* pVoidObj);
      friend void MedTankStillOnEnter(void* pVoidObj);
      friend void MedTankStrafeOnEnter(void* pVoidObj);
      friend void MedTankStillOnExit(void* pVoidObj);
      friend void MedTankStrafeOnExit(void* pVoidObj);
      friend void MedTankStillUpdate(void* pVoidObj);
      friend void MedTankStrafeUpdate(void* pVoidObj);
      friend bool MedTankAimToFire(void* pVoidObj);
      friend bool MedTankFireToAim(void* pVoidObj);
      friend bool MedTankAimToDocile(void* pVoidObj);
      friend bool MedTankDocileToAim(void* pVoidObj);
      friend void MedTankAimUpdate(void* pVoidObj);
      friend void MedTankAimOnEnter(void* pVoidObj);
      friend void MedTankAimOnExit(void* pVoidObj);
      friend void MedTankFireUpdate(void* pVoidObj);
      friend void MedTankFireOnExit(void* pVoidObj);
      friend void MedTankFireOnEnter(void* pVoidObj);
      friend void MedTankDocileOnEnter(void* pVoidObj);
      friend void MedTankDocileUpdate(void* pVoidObj);
      friend void MedTankDocileOnExit(void* pVoidObj);

};