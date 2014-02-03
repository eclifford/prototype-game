/// \file Sphere.h
/// Enemy Sphere unit
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/enemy.h"
#include "graphics/particle.h"
#include "sound/audio.h"

extern CAudioManager g_AudioManager;

class CSphere : public CEnemy
{
   private:
      float m_ShotDamage;
      float m_LaserCoolDown;
      float m_LaserLastShot;
      float m_Velocity;
      float m_Elevation;
      CVector3f m_OffsetFromPlayer;
      CParticleSystem * explode;      
   protected:

   public:
      CSphere(CSphere& Sphere) : CEnemy(Sphere)
      {
         SetupAI();
         m_Type = OBJ_SPHERE;
         m_pExplosion = NULL;
         m_CurPosition.x = m_CurPosition.y = m_CurPosition.z = 0.0f;
         m_LastPosition.x = m_LastPosition.y = m_LastPosition.z = 0.0f;
         m_Velocity2.x = m_Velocity2.y = m_Velocity2.z = 0.0f;
         
         m_pSphereAura = new CQuadtreeRenderObj;
         m_pSphereAura->Initialize(*Sphere.m_pSphereAura);
         
         // Set up the sound
         m_pExplosionSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_EXP_MED], 2);
         m_pEngineSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_ENEMY_ENGINE], 3);
         m_pMainGunsSFX = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_PLAYER_TURRET], 2);
      
         m_AwakenByPlayerRange = 350.0f;
         m_AwakenByEnemyRange = 50.0f;
      }
      CSphere();
      ~CSphere();	

      float m_AngularVelocity;
      float m_CircleAngularVelocity;
      float m_AttackDistance;
      float m_DistanceFromPlayer;

      CParticleSystem * m_pExplosion;

      CAudio * m_pExplosionSFX;
      CAudio * m_pEngineSFX;
      CAudio * m_pMainGunsSFX;

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

      CVector3f m_Velocity2;

      float m_SphereTimer;
      bool m_ShowShield;

      CQuadtreeRenderObj* m_pSphereAura;

      void Initialize(unsigned int GeometryIndex, unsigned int NumParts,
                CVector3f* pTranslations, int TextureIndex, const Sphere* pSphere,
                AABB* pAABB, CQuadtree* pQuadtree);

      void Setup();
      void SetupAI();
      void Update(void);
      void Collide(const CGameObject *obj);
      void Die(void);
      void Reset(void);
	   void OnAwaken(void);
      void TakeDamage(float damage, CGameObject * guywhohitme);
      void AimTowardPlayer();
      bool CollisionWithPlayerSphereScaledBy(float Scale);
      bool InDistanceFromPlayer(float Dist);
      void MoveTowardPlayer();
      void InitStateMachine();
      bool AimIsGood();
      void FireMainWeapon();
      int  PlayerRelativePosition();
      bool PlayerInFrontOfMe();
      void ScanForPlayer();
      void FireLaserAtGround();
      void FireLaserAtPlayer();
      void FireLaserCrazy();
      void RotateAroundPlayer();
      void FollowTerrain(void);

      // state update
      friend void SphereMoveUpdate(void* pVoidObj);
      friend void SphereIntimidateUpdate(void* pVoidObj);
      friend void SphereAttackUpdate(void* pVoidObj);

      // state on enter
      friend void SphereMoveOnEnter(void* pVoidObj);
      friend void SphereIntimidateOnEnter(void* pVoidObj);
      friend void SphereAttackOnEnter(void* pVoidObj);

      // state on exit
      friend void SphereMoveOnExit(void* pVoidObj);
      friend void SphereIntimidateOnExit(void* pVoidObj);
      friend void SphereAttackOnExit(void* pVoidObj);

      // transition functions from state to state
      friend bool SphereMoveToIntimidate(void* pVoidObj);
      friend bool SphereIntimidateToAttack(void* pVoidObj);
      friend bool SphereIntimidateToMove(void* pVoidObj);
};
