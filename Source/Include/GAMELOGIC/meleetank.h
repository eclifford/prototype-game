/// \file meleetank.h
/// Enemy melee tank unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/enemy.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CMeleeTank : public CEnemy
{
	private:

   protected:

	public:

      CVector3f m_OffsetFromPlayer;

      CMeleeTank(CMeleeTank& MeleeTank);

      CParticleSystem * m_pExplosion;
      CParticleSystem * m_pBallEnergy;
      CParticleSystem * m_pThrusterLeft;
      CParticleSystem * m_pBallExplode;
      CParticleSystem * m_pThrusterRight;

      CVector3f m_SparksPosition;
      CVector3f m_ThrusterLeftPos;
      CVector3f m_ThrusterRightPos;

      CAudio * m_pExplosionSFX;
      CAudio * m_pEngineSFX;

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

	   CMeleeTank();
		~CMeleeTank();	

      float m_ExplosionScale;
      float m_MeleeDamage;
      
      void Setup();
      void SetupAI();
      void Update(void);
		void Collide(const CGameObject *obj);
		void Die(void);
		void Reset(void);
      void FindSpherePositions();
      void FindThrusterPositions();
      void InitStateMachine();
      void TakeDamage(float damage, CGameObject * guywhohitme);
      void AimTowardPlayer();
      bool PlayerInFrontOfMe();
      void ScanForPlayer();
      bool InDistanceFromPlayer(float Dist);
	   void OnAwaken(void);

      inline int GetMeleeDamage()
      {
         return (int)m_MeleeDamage;
      }

      friend bool MeleeTankPursueToOffsetToPursue(void* pVoidObj);
      friend void MeleeTankPursueUpdate(void* pVoidObj);
      friend void MeleeTankPursueToOffsetUpdate(void* pVoidObj);
      friend void MeleeTankPursueOnExit(void* pVoidObj);
      friend void MeleeTankPursueToOffsetOnExit(void* pVoidObj);
      friend void MeleeTankPursueOnEnter(void* pVoidObj);
      friend void MeleeTankPursueToOffsetOnEnter(void* pVoidObj);
};
