/// \file projectile.h
/// The base class for all projectiles in the game
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/gameobject.h"

enum {PLAYER_SHOT = 0, ENEMY_SHOT, FRIENDLY_SHOT, ION_CANNON};

class CProjectile : public CGameObject
{
	protected:
      int   m_ShotOwner;
      float m_Damage;
      float m_DamageRadius;
      float m_CreationTime;
      float m_LifeTime;
      float m_Velocity;

	public:
      CProjectile(CProjectile& Projectile) : CGameObject(Projectile)
      {
         m_Damage = Projectile.m_Damage;  
         m_ShotOwner = Projectile.m_ShotOwner;
         m_RadarType = RADAR_PROJECTILE;
      }

      CGameObject * m_ProjectileOwner;

		CProjectile();
		~CProjectile();

      void AreaOfEffectDamage(float radius, float damage);
      void CountShotHits();

      virtual void Update() = 0;
		virtual void Collide(const CGameObject *obj) = 0;
		virtual void Die(void) = 0;
      virtual void Reset(void) = 0;

      inline int GetOwner()
      {  return m_ShotOwner; }

      inline void SetOwner(int owner)
      {  m_ShotOwner = owner; }

      inline int GetDamage()
      {  return (int)m_Damage; }

      inline void SetDamage(int damage)
      {  m_Damage = (float)damage; }
};
