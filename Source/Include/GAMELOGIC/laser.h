/// \file laser.h
/// A laser projectile
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/projectile.h"
#include "graphics/particlemanager.h"
#include "sound/audio.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CParticleManager g_ParticleManager;

class CLaser : public CProjectile
{
	private:
			
	public:
      CLaser(CLaser& Laser) : CProjectile(Laser)
      {
         m_Type = OBJ_LASER;

         m_RenderObj.m_Material.m_Diffuse.r = 1.0f;
         m_RenderObj.m_Material.m_Diffuse.g = 0.0f;
         m_RenderObj.m_Material.m_Diffuse.b = 0.0f;
         //m_RenderObj.m_Material.m_Diffuse.a = 0.3f;

         m_RenderObj.m_Material.m_Ambient.r = 1.0f;
         //m_pShieldModel->m_Material.m_Ambient.g = 0.0f;
         m_RenderObj.m_Material.m_Ambient.b = 0.0f;
         m_RenderObj.m_Material.m_Ambient.a = 0.0f;

         //m_RenderObj.m_Material.m_Specular.r = 0.0f;
        // m_RenderObj.m_Material.m_Specular.g = 0.0f;
         //m_RenderObj.m_Material.m_Specular.b = 0.1f;
        // m_RenderObj.m_Material.m_Specular.a = 1.0f;

         //m_RenderObj.m_Material.m_Shininess = 128;
         m_RenderObj.SetLighting(false);
      }

      CParticleSystem *m_ParticleEffect;
      CParticleSystem *m_ExplosionEffect;

      CVector3f m_CurPosition;
      CVector3f m_LastPosition;

      CAudio * m_LaserInAir;
      CAudio * m_LaserDie;

		CLaser();
		~CLaser();

      void SetUp(int shotOwner, int damage, float damageradius, CGameObject * projectileowner);
      void Update();
		void Collide(const CGameObject *obj);
		void Die(void);
      void Reset(void);
};