/// \file particle.h
/// Individual particles
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics\glinclude.h"

#include "math3d/Math3d.h"
#include "utils/utils.h"
#include "core/oglwindow.h"
#include "gamelogic/gameobject.h"

#include <list>
#include <vector>
using namespace std;

#pragma comment(lib, "winmm.lib")

/* Particle Engine constants */
#define ONLY_CREATE		  0
#define ONLY_UPDATE		  1
#define UPDATE_AND_CREATE 2

#define DEATH_AGE  -1.0f

#define ATTRACTION 0
#define STOP       1

#define GET_X 0
#define GET_Y 1
#define GET_Z 2

#define SK_OFF false
#define SK_ON  true

#define MIN_SPEED	   0.0f
#define MIN_SIZE     0.1f
#define MIN_LIFETIME 0.01f
#define MIN_SPREAD	-1000.0f
#define MIN_EMISSION 1.0f
#define MIN_GRAVITY  -10.0f
#define MIN_ALPHA	   0.0f
#define MIN_COLOR	   0.0f

#define MAX_SPEED	   300.0f
#define MAX_SIZE     100.0f
#define MAX_LIFETIME 20.0f
#define MAX_SPREAD   1000.0f
#define MAX_EMISSION 1000
#define MAX_GRAVITY  10.0f
#define MAX_ALPHA	   1.0f
#define MAX_COLOR	   1.0f

#define LOD_NONE 0
#define LOD_NEAR 1
#define LOD_FAR  2
#define LOD_BOTH 3

class CParticle;
class CParticleSystem;

struct MinMaxInfo_T
{
	float min;
	float max;
};

class CParticle
{
	private:
		CParticleSystem* m_pParent; // The parent system of this particle
		
	public:
		CVector3f m_PrevLocation;	 // The particle's last position
		CVector3f m_Location;	    // The particle's current position
		CVector3f m_Velocity;		 // The particle's current velocity		

		float m_Color[4];			    // The particle's color
		float m_ColorChange[4];		 // The colors change per second

		float m_Alpha;				    // The particle's current transparency
		float m_AlphaChange;		    // Adds/subtracts transparency over time

		float m_Size;				    // The particle's current size
		float m_SizeChange;		    // Adds/subtracts transparency over time

		float m_Age;				    // The particle's current age
		float m_DyingAge;			    // The age at which the particle DIES!

		float m_ParticleVerts[4];   // The four vertices of the particle
      float m_DistanceToCamera;

	void SetParentSystem(CParticleSystem *parent);
	void CreateParticle(CParticleSystem *parent, float alphaTime);
	bool UpdateParticle(float alphaTime, int index);
   void CalcuateDistanceToCamera(void);

	CParticle();
	~CParticle();
};

class CParticleSystem
{
	private:
		bool m_Attracting;					// Is the system attracting particle towards itself?
		bool m_Stopped;						// Have the particles stopped emitting?
		bool m_SystemAlive;					// Is the system still alive?

		unsigned int m_uTextureID;			// The particle's texture

		unsigned int m_uParticlesPerSec;	// Particles emitted per second
		unsigned int m_uParticlesAlive;	// The number of particles currently alive
		unsigned int m_uInitialParticles;// Number of particles initially emitted.

		float m_SystemAge;					// The systems current age in seconds
		float m_SystemDyingAge;
					                     	// The last time the system was updated
		float m_EmissionResidue;			// Helps emit very precise amounts of particles
		CGameObject * m_pParentObject;   // Parent object that spawned this system

	public:
      vector<CParticle*> LivePool; 
      vector<CParticle*>::iterator LivePoolIterator;

      float m_LastUpdate;	
      const CVector3f * m_RelativeLocation;

		CVector3f m_PrevLocation;			// The last known location of the system
		CVector3f m_Location;				// The current known position of the system
		CVector3f m_Velocity;				// The current known velocity of the system

		CVector3f m_StartColor;				// The starting color
		CVector3f m_EndColor;				// The ending color
		MinMaxInfo_T m_ColorVar;		   // Random Variance in color

		CVector3f m_Gravity;				   // Gravity for the X, Y, and Z axis

		MinMaxInfo_T m_SizeVar;			   // Random Variance in size
		MinMaxInfo_T m_AlphaVar;		   // Random Variance in alpha
		MinMaxInfo_T m_SpeedVar;			// Random Variance in speed
		MinMaxInfo_T m_ParticleLifeVar;	// Random Variance in particle life

      float m_MinLodDistance;
      float m_MaxLodDistance;

		float m_StartSize;					// The starting size of the particles
		float m_EndSize;					   // The particle's end size (used for a MAX boundry)
		
		float m_StartAlpha;					// The starting transparency of the particle
		float m_EndAlpha;					   // The end transparency (used for a MAX boundry)
		
		float m_Speed;						   // The system's speed
		
		float m_ParticleLife;				// Life of each particle
			
		float m_Angle;						   // System's angle 

		float m_SpreadFactor;				// Used to divide spread
      int m_LevelOfDetail;
		
		CVector3f m_SpreadMin;				// Spread min			
		CVector3f m_SpreadMax;				// Spread max
		
		int   m_UpdateFlag;					// How to update this system
		float m_SystemDelaySeconds;	   // Delay of when system is spawned
		CVector3f m_Attract;
		
		GLenum m_SrcBlendMode;
		GLenum m_DstBlendMode;
		
		float m_CreationTime;
		float m_SystemDyingTime;			// Age at which the system dies

		float m_DistanceToCamera;

      bool m_IsBuilding; // hack for building effects
	
	bool UpdateSystem(float time, int flag, int numToCreate);
	void RenderSystem(void);

	unsigned int ActiveParticles(void);
	float GetLocation(int coordinate);
	void SetLocation(float x, float y, float z);
	void SetVelocity(float xv, float yv, float zv);
	void SetSize(float startsize, float endsize);
	void SetAlpha(float startalpha, float endalpha);
	void SetSystemFlag(int flag, bool state);
	void SetColor(float start_red, float start_green, float start_blue, float end_red, float end_green, float end_blue);
	void SetSpread(float Spread_factor, float Spread_MinX, float Spread_MaxX,
				   float Spread_MinY, float Spread_MaxY,
				   float Spread_MinZ, float Spread_MaxZ);
	void SetGravity(float xpull, float ypull, float zpull);
	void SetSystemLifeSpan(float systemLifeSeconds);
	void SetAttractionPower(float attractX, float attractY, float attractZ);
	void SetBlendModes(GLenum source, GLenum dst);
	void DistanceToCamera(CVector3f * cameraLoc);
   void Shutdown();
  	
	inline void SetParentObject(CGameObject * object)
	{
		m_pParentObject = object;
	}

	inline CGameObject * GetParentObject()
	{
		return m_pParentObject;
	}

	inline void SetUpdateMode(int flag)
	{
		m_UpdateFlag = flag;
	}

	inline int GetUpdateMode(void)
	{
		return m_UpdateFlag;
	}

	inline void SetSystemAge(float age)
	{
		m_SystemAge = age;
	}

	inline float GetSystemAge(void)
	{
		return m_SystemAge;
	}

	inline float GetSpeed(void)
	{
		return m_Speed;
	}

	inline void SetLife(float life)
	{
		m_ParticleLife = life;
	}

	inline float GetLife()
	{
		return m_ParticleLife;
	}


	inline void SetSpeed(float speed)
	{
		m_Speed = speed;
	}


	inline float SetAngle(float angle)
	{
		m_Angle = angle;
	}

	inline float GetAngle(void)
	{
		return m_Angle;
	}

	inline bool GetAttracting(void)
	{	
		return m_Attracting;	
	}

	inline bool GetStopped(void)
	{	
		return m_Stopped;	
	}

	inline void SetAttracting(bool status)
	{	
		m_Attracting = status;	
	}

	inline void SetStopped(bool status)
	{	
		m_Stopped = status;	
	}
	
	inline void SetSystemAlive(bool status)
	{	
		m_SystemAlive = status;	
	}
	
	inline bool GetSystemAlive()
	{	
		return m_SystemAlive;	
	}
	
	inline void SetParticlesPerSec(unsigned int numToSet)
	{
		m_uParticlesPerSec = numToSet;
	}

	inline unsigned int GetParticlesPerSec(void)
	{
		return m_uParticlesPerSec;
	}

	inline void SetInitialParticles(unsigned int numToSet)
	{
		m_uInitialParticles = numToSet;
	}

	inline unsigned int GetInitialParticles(void)
	{
		return m_uInitialParticles;
	}

	inline void SetParticlesAlive(unsigned int alive)
	{
		m_uParticlesAlive = alive;
	}

	inline unsigned int GetParticlesAlive(void)
	{
		return m_uParticlesAlive;
	}

	inline void SetCreationTime(float time)
	{
		m_CreationTime = time;
	}

	inline float GetCreationTime(void)
	{
		return m_CreationTime;
	}

	inline void SetTextureID(unsigned int index)
	{
		m_uTextureID = index;
	}

	inline unsigned int GetTextureID(void)
	{
		return m_uTextureID;
	}

	inline void SetSystemDyingAge(float age)
	{
		m_SystemDyingAge = age;
	}

	inline float GetSystemDyingAge(void)
	{
		return m_SystemDyingAge;
	}

	inline void SetSystemDelaySeconds(float set)
	{
		m_SystemDelaySeconds = set;
	}

	inline float GetSystemDelaySeconds(void)
	{
		return m_SystemDelaySeconds;
	}

	CParticleSystem();
	~CParticleSystem();
};
