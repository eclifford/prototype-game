/// \file particlemanager.h
/// The manager of the particle systems
//////////////////////////////////////////////////////////////////////////

// Include this only once
#pragma once

#include "graphics/particle.h"
#include "gamelogic/gameobject.h"
#include "math.h"
#include <queue>
#include <list>
using namespace std;

// Max number of systems
#define MAX_SYSTEMS 500
#define PREALLOCATED_SYSTEMS 300
#define LOD_MINDISTANCE 100.0f //   10000.0f
#define LOD_MAXDISTANCE 500.0f //   250000.0f
#define MAX_PARTICLES 3000  //  size of preallocated particle pool
    
class CParticleManager
{
	private:	

	public:

		CParticleManager();
		~CParticleManager();
      
      float particle_sine[360];
      float particle_cosi[360];
		
		// Particle system effect types
		enum Effects{PLASMA = 0, SMOKEDOUT, AURA, TIMED, BURNER, SMOKE, ION_BEAM, ION_BLAST, KAMI_SPARKS, FADE, FLASH, XOR, PLASMAEXP, 
         MISSILE_TRAIL, MISSILE_EXP, ENEMY_SHOT, ENEMY_PLASMA, ENEMY_PLASMAEXP, KAMI_THRUSTER, TREE_DAMAGE, MISSILE_TRAIL_SMOKE, TREEFIRE, 
		CHARGELVL2, CHARGELVL3, ENEMYEXP_SMALL, ENEMYEXP_LARGE, TRKMISSILE_TRAIL, TRKMISSILE_TRAIL_SMOKE, KAMI_EXPLODE, CHARGELVL1, 
      DIRT, BUILDING_FALLING, BUILDING_SMOKE, BUILDING_EXPLODE, TRKMISSILE_EXP, HVYTANK_EXP, IONFLARE_SHOT, IONFLARE_GROUND, SPHERE_AURA, 
      LASERSHOT_HIT, TREE_DIE, MOTHERSHIP_THRUSTER, CONVOY_FIRE, CONVOY_SMOKE, LASER_TURRET, LASER_STREAM, 
      FIGHTER_THRUSTER, FIGHTER_EXP, FIGHTER_SMOKE, FIGHTER_FIRE, TURRET_EXP, BOSS_THRUSTER, BOSS_EXP, NUM_EFFECTS};
		
      list<CParticleSystem*> SystemList;
		list<CParticleSystem*>::iterator Iterator; 
      list<CParticleSystem*>::iterator Next;
		list<CParticleSystem*> RenderList;
      list<CParticle*> ParticlePool;
      
      list<CParticleSystem*> NewSystems;
      
      CParticleSystem* m_AllSystems[PREALLOCATED_SYSTEMS];
      
	 	enum eParticleTex{
         FLARE_TEX = 0, 
         CIRCLE_TEX,    
         SPARK_TEX, 
         CLOUD_TEX, 
         TORCH_TEX, 
         BURN_TEX, 
         BUBBLE_TEX, 
         CLOUDS_TEX,
         ALPHA_AURA_TEX,
         ALPHA_BANG_TEX,
         ALPHA_BUBBLE1_TEX,
         ALPHA_CLOUD1_TEX,
         ALPHA_DIRT_TEX,
         ALPHA_ODD1_TEX,
         ALPHA_PUFF_TEX,
         ALPHA_RING_TEX,
         ALPHA_RING2_TEX,
         ALPHA_ROCK_TEX,
         ALPHA_SMOKE_TEX,
         ALPHA_SMOKE2_TEX,
         ALPHA_SPLAT1_TEX,
         ALPHA_SPLAT2_TEX,
         ALPHA_STAR1_TEX,
         ALPHA_STAR2_TEX,
         ALPHA_STAR3_TEX,
         ALPHA_TEAR1_TEX,
         ALPHA_WHIRL_TEX,
         PLASMA_CHARGE,
         NUM_PARTICLE_TEXTURES};

		int m_uParticleTextures[100];
      int m_TotalParticlesOnScreen;

		void Initialize();
      CParticleSystem* ObtainSystem();
		void CreateSystem(CParticleSystem * system, CVector3f location, const CVector3f * relativeLocation, int effect,
                        int lod, float lodmin = LOD_MINDISTANCE, float lodmax = LOD_MAXDISTANCE);
		void LoadEffect(int effectIndex, char * fn);
		void SetSystem(CParticleSystem * system, int effectIndex);
		void RenderSystem();
		void UpdateSystem();
		void Shutdown();
		void RenderSmartInsert(CParticleSystem * systemToInsert);
      void DestroySystem(CParticleSystem * system);
      void ClearSystem(CParticleSystem * system);
      void BuildLookupTables(void);
};

