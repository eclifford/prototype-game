 /// \file particlemanager.cpp
/// Particle manager that controls the particle systems
//////////////////////////////////////////////////////////////////////////

#include "graphics/particlemanager.h"   
#include "graphics/texturemanager.h"
#include "camera/camera.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CTextureManager  g_TextureManager;
extern CParticleManager g_ParticleManager;
extern COpenGLWindow    *pGLWIN;
extern CBaseCamera      *g_Camera;

/////////////////////////////////
/*/ External Globals          /*/ 
/////////////////////////////////

extern float g_time;
extern float g_FrameTime;
float cos_look[360];
float sin_look[360];

struct Properties_T
{
	unsigned int TextureID;
	CVector3f StartColor;
	CVector3f EndColor;
	float StartAlpha;
	float EndAlpha;
	float ParticleLife;
	float SystemDyingAge;
	bool  Attracting;
	float SystemDelaySeconds;
	float Speed;
	float SpreadFactor;
	CVector3f SpreadMin;
	CVector3f SpreadMax;
	float StartSize;
	float EndSize;
	CVector3f Velocity;
	CVector3f Gravity;
	CVector3f Attract;
	float Angle;
	unsigned int ParticlesPerSec;
	unsigned int InitialParticles;
	int UpdateFlag;
	MinMaxInfo_T ColorVar;
	MinMaxInfo_T AlphaVar;
	MinMaxInfo_T ParticleLifeVar;
	MinMaxInfo_T SpeedVar;
	MinMaxInfo_T SizeVar;
	GLenum SrcBlendMode;
	GLenum DestBlendMode;
};

// Array of Effects Properties structures
Properties_T Properties[CParticleManager::NUM_EFFECTS];

//////////////////////////////////////////////////////////////////////////
/// Constructor for the particle manager
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CParticleManager::CParticleManager()
{
	Iterator = NULL; 
   m_TotalParticlesOnScreen = 0;
}

//////////////////////////////////////////////////////////////////////////
/// Destructor for the particle manager
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CParticleManager::~CParticleManager()
{

}

//////////////////////////////////////////////////////////////////////////
/// Initialize the particle manager
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::Initialize()
{
   //BuildLookupTables();

	// Load the particle textures
	m_uParticleTextures[FLARE_TEX] = g_TextureManager.LoadTexture("Resources/Images/flare.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[CIRCLE_TEX] = g_TextureManager.LoadTexture("Resources/Images/circle.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);	
	m_uParticleTextures[SPARK_TEX] = g_TextureManager.LoadTexture("Resources/Images/spark.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[CLOUD_TEX] = g_TextureManager.LoadTexture("Resources/Images/cloud.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[TORCH_TEX] = g_TextureManager.LoadTexture("Resources/Images/torch.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[BURN_TEX] = g_TextureManager.LoadTexture("Resources/Images/burn.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[BUBBLE_TEX] = g_TextureManager.LoadTexture("Resources/Images/bubble.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
   m_uParticleTextures[CLOUDS_TEX] = g_TextureManager.LoadTexture("Resources/Images/clouds.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
     
   m_uParticleTextures[ALPHA_AURA_TEX] = g_TextureManager.LoadTexture("Resources/Images/aura_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_BANG_TEX] = g_TextureManager.LoadTexture("Resources/Images/bang_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_BUBBLE1_TEX] = g_TextureManager.LoadTexture("Resources/Images/bubble1_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_CLOUD1_TEX] = g_TextureManager.LoadTexture("Resources/Images/cloud1_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_DIRT_TEX] = g_TextureManager.LoadTexture("Resources/Images/dirt_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_ODD1_TEX] = g_TextureManager.LoadTexture("Resources/Images/odd1_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
   m_uParticleTextures[ALPHA_PUFF_TEX] = g_TextureManager.LoadTexture("Resources/Images/puff_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_RING_TEX] = g_TextureManager.LoadTexture("Resources/Images/ring_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);	
	m_uParticleTextures[ALPHA_RING2_TEX] = g_TextureManager.LoadTexture("Resources/Images/ring2_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);	
   m_uParticleTextures[ALPHA_ROCK_TEX] = g_TextureManager.LoadTexture("Resources/Images/rock_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
   
   m_uParticleTextures[ALPHA_SMOKE_TEX] = g_TextureManager.LoadTexture("Resources/Images/smoke_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_SMOKE2_TEX] = g_TextureManager.LoadTexture("Resources/Images/smoke2_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_SPLAT1_TEX] = g_TextureManager.LoadTexture("Resources/Images/splat1_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_SPLAT2_TEX] = g_TextureManager.LoadTexture("Resources/Images/splat2_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_STAR1_TEX] = g_TextureManager.LoadTexture("Resources/Images/star1_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_STAR2_TEX] = g_TextureManager.LoadTexture("Resources/Images/star2_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
	m_uParticleTextures[ALPHA_STAR3_TEX] = g_TextureManager.LoadTexture("Resources/Images/star3_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);	
	m_uParticleTextures[ALPHA_TEAR1_TEX] = g_TextureManager.LoadTexture("Resources/Images/tear1_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);
   m_uParticleTextures[ALPHA_WHIRL_TEX] = g_TextureManager.LoadTexture("Resources/Images/whirl_a.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP, GL_CLAMP);

	// Load the particle system effects
	LoadEffect(PLASMA, "Resources/Effects/plasmaball.fx");
   LoadEffect(BURNER, "Resources/Effects/burner.fx");
   LoadEffect(FLASH, "Resources/Effects/shot.fx");
   LoadEffect(PLASMAEXP, "Resources/Effects/plasmaballexp.fx");
   LoadEffect(AURA, "Resources/Effects/enemyexplode.fx");
   LoadEffect(TIMED, "Resources/Effects/tankfire.fx");
   LoadEffect(SMOKE, "Resources/Effects/smoke.fx");
   LoadEffect(ION_BEAM, "Resources/Effects/testion.fx");
   LoadEffect(ION_BLAST, "Resources/Effects/ionexp.fx");
   LoadEffect(KAMI_SPARKS, "Resources/Effects/kamisparks.fx");
   LoadEffect(MISSILE_TRAIL, "Resources/Effects/missiletrail.fx");
   LoadEffect(MISSILE_EXP, "Resources/Effects/missileexp.fx");
   LoadEffect(ENEMY_SHOT, "Resources/Effects/enemyshot.fx");
   LoadEffect(ENEMY_PLASMA, "Resources/Effects/enemyplasmaball.fx");
   LoadEffect(ENEMY_PLASMAEXP, "Resources/Effects/enemyplasmaballexp.fx");
   LoadEffect(KAMI_THRUSTER, "Resources/Effects/kamithruster.fx");
   LoadEffect(TREE_DAMAGE, "Resources/Effects/treedamage.fx");
   LoadEffect(CHARGELVL1, "Resources/Effects/plasmacharge.fx");
   LoadEffect(MISSILE_TRAIL_SMOKE, "Resources/Effects/missiletrailsmoke.fx");
   LoadEffect(TREEFIRE, "Resources/Effects/treefire.fx");
   LoadEffect(CHARGELVL2, "Resources/Effects/plasmachargelvl2.fx");
   LoadEffect(CHARGELVL3, "Resources/Effects/plasmachargelvl3.fx");
   LoadEffect(ENEMYEXP_SMALL, "Resources/Effects/enemyexplodesmall.fx");
   LoadEffect(ENEMYEXP_LARGE, "Resources/Effects/enemyexplodelarge.fx");
   LoadEffect(TRKMISSILE_TRAIL, "Resources/Effects/trkmissiletrail.fx");
   LoadEffect(TRKMISSILE_TRAIL_SMOKE, "Resources/Effects/trkmissilesmoke.fx");
   LoadEffect(KAMI_EXPLODE, "Resources/Effects/kamiballexplode.fx");
   LoadEffect(DIRT, "Resources/Effects/dirt2.fx");
   LoadEffect(BUILDING_FALLING, "Resources/Effects/buildingfalling.fx");
   LoadEffect(BUILDING_SMOKE, "Resources/Effects/buildingsmoke.fx");
   LoadEffect(BUILDING_EXPLODE, "Resources/EFfects/buildingexplode.fx");
   LoadEffect(TRKMISSILE_EXP, "Resources/Effects/trkmissileexp.fx");
   LoadEffect(HVYTANK_EXP, "Resources/Effects/hvytankexplode.fx");
   LoadEffect(IONFLARE_SHOT, "Resources/Effects/ionflareshot.fx");
   LoadEffect(IONFLARE_GROUND, "Resources/Effects/ionflareonground.fx");
   LoadEffect(SPHERE_AURA, "Resources/Effects/sphereaura.fx");
   LoadEffect(LASERSHOT_HIT, "Resources/Effects/lasershothit.fx");
   LoadEffect(TREE_DIE, "Resources/Effects/treedie.fx");
   LoadEffect(MOTHERSHIP_THRUSTER, "Resources/Effects/mothershipthruster.fx");
   LoadEffect(CONVOY_SMOKE, "Resources/Effects/convoysmoke.fx");
   LoadEffect(CONVOY_FIRE, "Resources/Effects/convoyfire.fx");
   LoadEffect(LASER_TURRET, "resources/effects/laserturretfire.fx");
   LoadEffect(LASER_STREAM, "resources/effects/laserstream.fx");
   LoadEffect(FIGHTER_THRUSTER, "resources/effects/fighterthruster.fx");
   LoadEffect(FIGHTER_EXP, "resources/effects/fighterexplode.fx");
   LoadEffect(FIGHTER_SMOKE, "resources/effects/fightersmoke.fx");
   LoadEffect(FIGHTER_FIRE, "resources/effects/fighterfire.fx");
   LoadEffect(TURRET_EXP, "resources/effects/turretexplode.fx");
   LoadEffect(BOSS_THRUSTER, "resources/effects/bossthruster.fx");
   LoadEffect(BOSS_EXP, "resources/effects/bossexp.fx");

	// Allocate the memory for preallocated systems
	for(int i = 0; i < PREALLOCATED_SYSTEMS; i++)
	{
		CParticleSystem * _newSystem = new CParticleSystem();

        m_AllSystems[i] = _newSystem;

		// Push this into the linked list of systems
		SystemList.push_back(_newSystem);
	}

   // Allocate the memory for particle pool
	for(int p = 0; p < MAX_PARTICLES; p++)
	{
		CParticle* _particle = new CParticle();

		// Push this into the linked list of systems
		ParticlePool.push_back(_particle);
	}
}

//////////////////////////////////////////////////////////////////////////
/// Attemp to obtain a free particle system
/// 
/// Input:     void
///
/// Returns:   Return that particle system
//////////////////////////////////////////////////////////////////////////

CParticleSystem* CParticleManager::ObtainSystem()
{
   // We only want MAX_SYSTEMS rendering at any one time
   if(g_ParticleManager.RenderList.size() >= MAX_SYSTEMS)
      return NULL;

   // Get a unused system
   if(!SystemList.empty())
   {
      CParticleSystem * system = SystemList.front();
      SystemList.pop_front();
      return system;
   }                                                                                                                     

	// Create a new system 
	CParticleSystem * _newSystem = new CParticleSystem();
    NewSystems.push_back(_newSystem);
   // Return the created system
   return _newSystem;
}

//////////////////////////////////////////////////////////////////////////
/// Create a particle system
/// 
/// Input:     
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::CreateSystem(CParticleSystem * system, 
                       CVector3f location, const CVector3f * relativeLocation, int effect, int lod, float lodmin, float lodmax)
{	
   // Clear out all the particles in this system 
   //ClearSystem(system);

   // Set the system to alive
   system->SetSystemAlive(true);

   // Set the paritcles parent game object (if it has one)
   system->m_RelativeLocation = relativeLocation;

   // Set this particles location
   system->SetLocation(location.x, location.y, location.z);

   // This system is just created
   system->SetCreationTime(g_time);
   system->SetSystemAge(0.0f);
   system->m_LevelOfDetail = lod;

   SetSystem(system, effect);	

   system->m_MinLodDistance = lodmin;
   system->m_MaxLodDistance = lodmax;

   // Insert this into the rendering list
   RenderSmartInsert(system);
}


//////////////////////////////////////////////////////////////////////////
/// Delete a particle system from the world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
/// Delete all the particle systems
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::Shutdown()
{
	for (unsigned int i = 0; i < PREALLOCATED_SYSTEMS; ++i)
	{
	    m_AllSystems[i]->Shutdown();
	    delete m_AllSystems[i];
	}
	
	for(std::list<CParticleSystem*>::iterator SystemIterator = NewSystems.begin(); SystemIterator != NewSystems.end(); ++SystemIterator)
	{
	    (*SystemIterator)->Shutdown();
		delete *SystemIterator;
	}
	
	NewSystems.clear();
	
	RenderList.clear();
	
	SystemList.clear();
	
	// delete particles
	for(std::list<CParticle*>::iterator ParticleIterator = ParticlePool.begin(); ParticleIterator != ParticlePool.end(); ++ParticleIterator)
	{
		delete *ParticleIterator;
	}
	ParticlePool.clear();
}

//////////////////////////////////////////////////////////////////////////
/// Render all the systems in world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::RenderSystem()
{
	// Iterate through all systems that are alive and render them
	Iterator = RenderList.begin();

   // We have to make sure the did not just die in Update
	while (Iterator != RenderList.end())// && (*Iterator)->GetSystemAlive())
	{
      (*Iterator)->RenderSystem();		
      Iterator++;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Remove a particle system from the render list and reset it
/// 
/// Input:     CParticleSystem *system - the particle system to remove
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::DestroySystem(CParticleSystem * system)
{
   system->SetSystemAlive(false);
   system->m_LastUpdate = 0.0f;

   while(!system->LivePool.empty())
   {
      CParticle *part = system->LivePool.back();
      ParticlePool.push_back(part);
      system->LivePool.pop_back();
   }

   system->SetParticlesAlive(0);
   RenderList.remove(system);  
}

//////////////////////////////////////////////////////////////////////////
/// Clear out a system's particles
/// 
/// Input:     CParticleSystem *system - the system to clear
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::ClearSystem(CParticleSystem * system)
{
   system->m_LastUpdate = 0.0f;

   while(!system->LivePool.empty())
   {
      CParticle *part = system->LivePool.back();
      ParticlePool.push_back(part);
      system->LivePool.pop_back();
   }

   system->SetParticlesAlive(0);
}

//////////////////////////////////////////////////////////////////////////
/// Update all the systems in the world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::UpdateSystem()
{
	// Iterate through all systems that are alive and update them
	Iterator = RenderList.begin();
   m_TotalParticlesOnScreen = 0;
	while (Iterator != RenderList.end())
	{
      // See if this system needs to be removed
      if((*Iterator)->GetSystemAlive() == false)
		{
         ClearSystem((*Iterator));
         SystemList.push_back((*Iterator));
         
         Next = Iterator;
         ++Next;
         
         //CParticleSystem * tempiterator = *Iterator;
         //++Next;
         //CParticleSystem * tempnext = *Next;
		 RenderList.erase(Iterator);
         Iterator = Next;
         continue;
		}

      // Add up the total particles on screen
      m_TotalParticlesOnScreen += (int)(*Iterator)->LivePool.size();

		// Keep track of the time lapse and frame count
		float time = g_time - (*Iterator)->m_CreationTime; // Get current time in seconds

		// Get the distance to the camera
		CVector3f camerapos;
		g_Camera->GetPosition(&camerapos);
		(*Iterator)->DistanceToCamera(&camerapos);

		// See if the system is alive
		if((*Iterator)->GetSystemAlive())
		{
			// Render system based on its rendering flag
			switch((*Iterator)->m_UpdateFlag)
			{
				case ONLY_UPDATE:
				{
					(*Iterator)->UpdateSystem(time, ONLY_UPDATE, 0);
					break;		
				}

				case ONLY_CREATE:
				{
					(*Iterator)->UpdateSystem(time, ONLY_CREATE, (*Iterator)->GetInitialParticles());
					(*Iterator)->m_UpdateFlag = ONLY_UPDATE;
					break;		
				}

				case UPDATE_AND_CREATE:
				{
					(*Iterator)->UpdateSystem(time, UPDATE_AND_CREATE, 0);
					break;		
				}
			};
		}

		// Increment the iterator
		++Iterator;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Load an effect
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::LoadEffect(int effectIndex, char * fn)
{
	// File stream
	FILE * stream = NULL;

	// Open the file
	stream = fopen(fn, "rb");

	// Error check the stream
	if (stream == NULL)
	{
		MessageBox(NULL, "Failed to load particle Effect", "ERROR", MB_OK);
	}

	// Start at the beginning
	fseek(stream, 0L, SEEK_SET);

	fread(&Properties[effectIndex].TextureID,	sizeof(unsigned int), 1, stream);
	fread(&Properties[effectIndex].StartColor,  sizeof(CVector3f), 1, stream);
	fread(&Properties[effectIndex].EndColor,	sizeof(CVector3f), 1, stream);
	fread(&Properties[effectIndex].StartAlpha,  sizeof(float), 1, stream);
	fread(&Properties[effectIndex].EndAlpha,	sizeof(float), 1, stream);
	fread(&Properties[effectIndex].ParticleLife,		sizeof(float), 1, stream);
	fread(&Properties[effectIndex].SystemDyingAge, sizeof(float), 1, stream);
	fread(&Properties[effectIndex].Attracting, sizeof(bool), 1, stream);
	fread(&Properties[effectIndex].SystemDelaySeconds, sizeof(float), 1, stream);
	fread(&Properties[effectIndex].Speed, sizeof(float), 1, stream);
	fread(&Properties[effectIndex].SpreadFactor, sizeof(float), 1, stream);
	fread(&Properties[effectIndex].SpreadMin, sizeof(CVector3f), 1, stream);
	fread(&Properties[effectIndex].SpreadMax, sizeof(CVector3f), 1, stream);
	fread(&Properties[effectIndex].StartSize, sizeof(float), 1, stream);
	fread(&Properties[effectIndex].EndSize, sizeof(float), 1, stream);
	fread(&Properties[effectIndex].Velocity, sizeof(CVector3f), 1, stream);
	fread(&Properties[effectIndex].Gravity, sizeof(CVector3f), 1, stream);
	fread(&Properties[effectIndex].Attract, sizeof(CVector3f), 1, stream);
	fread(&Properties[effectIndex].Angle, sizeof(float), 1, stream);
	fread(&Properties[effectIndex].ParticlesPerSec, sizeof(unsigned int), 1, stream);
	fread(&Properties[effectIndex].InitialParticles, sizeof(int), 1, stream);
	fread(&Properties[effectIndex].ColorVar, sizeof(MinMaxInfo_T), 1, stream);
	fread(&Properties[effectIndex].AlphaVar, sizeof(MinMaxInfo_T), 1, stream);
	fread(&Properties[effectIndex].ParticleLifeVar, sizeof(MinMaxInfo_T), 1, stream);
	fread(&Properties[effectIndex].SpeedVar, sizeof(MinMaxInfo_T), 1, stream);
	fread(&Properties[effectIndex].SizeVar, sizeof(MinMaxInfo_T), 1, stream);
	fread(&Properties[effectIndex].SrcBlendMode, sizeof(GLenum), 1, stream);
	fread(&Properties[effectIndex].DestBlendMode, sizeof(GLenum), 1, stream);
	fread(&Properties[effectIndex].UpdateFlag, sizeof(int), 1, stream);
	
	// Close the file
	fclose(stream);
}

//////////////////////////////////////////////////////////////////////////
/// Update all the systems in the world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::SetSystem(CParticleSystem * system, int effectIndex)
{
	system->SetTextureID(Properties[effectIndex].TextureID);
	system->m_StartColor = Properties[effectIndex].StartColor;
	system->m_EndColor   = Properties[effectIndex].EndColor;
	system->m_StartAlpha = Properties[effectIndex].StartAlpha;
	system->m_EndAlpha   = Properties[effectIndex].EndAlpha;
	system->m_ParticleLife = Properties[effectIndex].ParticleLife;
	system->SetSystemDyingAge(Properties[effectIndex].SystemDyingAge);
	system->SetAttracting(Properties[effectIndex].Attracting);
	system->m_SystemDelaySeconds = Properties[effectIndex].SystemDelaySeconds;
	system->m_Speed = Properties[effectIndex].Speed;

	system->SetSpread(Properties[effectIndex].SpreadFactor,
					  Properties[effectIndex].SpreadMin.x,
					  Properties[effectIndex].SpreadMax.x,
					  Properties[effectIndex].SpreadMin.y,
					  Properties[effectIndex].SpreadMax.y,
					  Properties[effectIndex].SpreadMin.z,
					  Properties[effectIndex].SpreadMax.z);

	system->m_StartSize = Properties[effectIndex].StartSize;
	system->m_EndSize = Properties[effectIndex].EndSize;
	system->m_Velocity = Properties[effectIndex].Velocity;
	system->m_Gravity = Properties[effectIndex].Gravity;
	
	system->SetAttractionPower(Properties[effectIndex].Attract.x,
							   Properties[effectIndex].Attract.y,
							   Properties[effectIndex].Attract.z);

	system->m_Angle = Properties[effectIndex].Angle;
	system->SetParticlesPerSec(Properties[effectIndex].ParticlesPerSec);
	system->SetInitialParticles(Properties[effectIndex].InitialParticles);
	system->m_ColorVar = Properties[effectIndex].ColorVar;
	system->m_AlphaVar = Properties[effectIndex].AlphaVar;
	system->m_ParticleLifeVar = Properties[effectIndex].ParticleLifeVar;
	system->m_SpeedVar = Properties[effectIndex].SpeedVar;
	system->m_SizeVar = Properties[effectIndex].SizeVar;
	system->SetBlendModes(Properties[effectIndex].SrcBlendMode, Properties[effectIndex].DestBlendMode);
	system->m_UpdateFlag = Properties[effectIndex].UpdateFlag;

}

//////////////////////////////////////////////////////////////////////////
/// Update all the systems in the world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::RenderSmartInsert(CParticleSystem * systemToInsert)
{
	
	if(RenderList.size() == 0)
	{
		RenderList.push_back(systemToInsert);
		return;
	}
	
	Iterator = RenderList.begin();

	while(Iterator != RenderList.end())
	{
		if(systemToInsert->GetTextureID() <= (*Iterator)->GetTextureID())
		{
			RenderList.insert(Iterator, systemToInsert);
			return;
		}
		++Iterator;
	}
	
	RenderList.push_back(systemToInsert);	
}

//////////////////////////////////////////////////////////////////////////
/// Build a Sine and a Cosine lookup table
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleManager::BuildLookupTables(void)
{	
   // create sin/cos lookup table
   for (int ang = 0; ang <= 360; ang++)
    {
       // convert ang to radians
       float theta = (float)ang*PI/(float)180;

       // insert next entry into table
       cos_look[ang] = cos(theta);
       sin_look[ang] = sin(theta);
    }
}




















