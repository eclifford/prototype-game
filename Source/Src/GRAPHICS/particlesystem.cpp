/// \file particlesystem.cpp
/// Particle system containing particles
//////////////////////////////////////////////////////////////////////////

#include <algorithm>

#include "graphics\particle.h"
#include "graphics\texturemanager.h"
#include "math3d\Math3d.h"
#include "graphics\particlemanager.h"
#include "camera\camera.h"
#include "core\oglstatemanager.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CTextureManager g_TextureManager;
extern COpenGLStateManager * pGLSTATE;
extern COpenGLWindow *pGLWIN;
extern CBaseCamera *g_Camera;
extern CParticleManager g_ParticleManager;

/////////////////////////////////
/*/ External Globals          /*/ 
/////////////////////////////////

extern float g_time;
extern float g_fps;
extern float g_FrameTime;

/////////////////////////////////
/*/ Pre-Defined Headers       /*/ 
/////////////////////////////////

bool CompareParticleDistance(const CParticle * a, const CParticle * b);
void ParticleDistanceToCamera(CParticle *part);

//////////////////////////////////////////////////////////////////////////
/// Particle System contructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CParticleSystem::CParticleSystem()
{
	// Set up all the member variables to safe staring values
	m_Attracting = false;
	m_Stopped = false;
   m_IsBuilding = false;
   m_LevelOfDetail = 0;

	m_uTextureID = 0;
	m_uParticlesPerSec = 0;
	m_uParticlesAlive = 0;
	m_SpreadMin.x = 0;
	m_SpreadMax.x = 0;
	m_SpreadMin.y = 0;
	m_SpreadMax.y = 0;
	m_SpreadMin.z = 0;
	m_SpreadMax.z = 0;
	
	m_ColorVar.min = 0.0f;
	m_ColorVar.max = 0.0f;

	m_AlphaVar.min = 0.0f;
	m_AlphaVar.max = 0.0f;

	m_SizeVar.min = 0.0f;
	m_SizeVar.max = 0.0f;

	m_SpeedVar.min = 0.0f;
	m_SpeedVar.max = 0.0f;

	m_ParticleLifeVar.min = 0.0f;
	m_ParticleLifeVar.max = 0.0f;

	m_EndSize = 0.0f;
	m_StartAlpha = 0.0f;

	m_EndAlpha = 0.0f;
	m_Speed = 0.0f;

	m_ParticleLife = 0.0f;

	m_Angle = 0.0f;
	m_SystemAge = 0.0f;
	m_SystemDyingAge = 0.0f;
	m_LastUpdate = 0.0f,
	m_EmissionResidue = 0.0f;
	m_Attract.x = m_Attract.y = m_Attract.z = 0.0f;
   m_PrevLocation.x = m_PrevLocation.y = m_PrevLocation.z = 0.0f;
	
	m_UpdateFlag = ONLY_CREATE; 
	m_uInitialParticles = 0;
	m_uParticlesAlive = 0;
	m_SystemDelaySeconds = 0.0f;
	m_SystemAlive = false;

   LivePoolIterator = NULL;

   m_RelativeLocation = NULL;

   // Pre allocate the dead pool
   //for(int i = 0; i < DEAD_POOL_SIZE; i++)
   //{
   //   CParticle * part = new CParticle;
    //  DeadPool.push_back(part);
   //}
}

//////////////////////////////////////////////////////////////////////////
/// Particle System destructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CParticleSystem::~CParticleSystem()
{

}

void CParticleSystem::Shutdown()
{ 
   LivePoolIterator = LivePool.begin();
   while(LivePoolIterator != LivePool.end())  
   {
      CParticle * temp = (*LivePoolIterator);
      LivePoolIterator++;
      delete temp;
   }
   LivePool.clear();

   //DeadPoolIterator = DeadPool.begin();
   //while(DeadPoolIterator != DeadPool.end())  
   //{
   //   CParticle * temp = (*DeadPoolIterator);
   //   DeadPoolIterator++;
   //   delete temp;
   //}
   //DeadPool.clear();   
}

//////////////////////////////////////////////////////////////////////////
/// Update (move)(create) all particles inside this system
/// 
/// Input:     float time        - current game time
///            float flag        - how to update
///            float numToCreate - How many particles to create
///
/// Returns:   true if successful
//////////////////////////////////////////////////////////////////////////

bool CParticleSystem::UpdateSystem(float time, int flag, int numToCreate)
{
	int			_Loop;						
	unsigned int         _ParticlesCreated;				 
	float			_TimeCounter;					
	float			_ParticlesNeeded = float(numToCreate);	
	static bool delayed = true;

   // This system is attached to a game object
   if(m_RelativeLocation)
   {
	   SetLocation(m_RelativeLocation->x, m_RelativeLocation->y, m_RelativeLocation->z);
   }
		
	// Test to see if system is delayed
	if(time < m_SystemDelaySeconds)
	{
		SetSystemFlag(STOP, SK_ON);
		return false;
	}

	// The time elapsed since the last update
	_TimeCounter = g_FrameTime;

	// System was delayed so we need to make it seem like no time has passed since creation
	if(time >= m_SystemDelaySeconds && m_SystemDelaySeconds != 0.0f && m_Stopped == true)
	{
		_TimeCounter = 0.0f;
		SetSystemFlag(STOP, SK_OFF);				
	}

	if(flag == ONLY_UPDATE || flag == UPDATE_AND_CREATE)
	{    
		// Age this system
		m_SystemAge += _TimeCounter;

		// See if this system is dead
		if(m_SystemAge >= m_SystemDyingAge && m_SystemDyingAge != 0.0f && m_UpdateFlag != ONLY_UPDATE)
		{
			m_UpdateFlag = ONLY_UPDATE;
		}

      if(m_UpdateFlag == ONLY_UPDATE && GetParticlesAlive() <= 0 && m_SystemAge >= m_ParticleLife)
		{
		  	SetSystemAlive(false);
			return false;
		}
			
		// Set the time of the last update to now.  
		m_LastUpdate = time;

		// Clear the particle counter variable before counting.
		m_uParticlesAlive = 0;

		// Go through each particle and update them
		for(_Loop = 0; _Loop < (int)LivePool.size(); _Loop++)
		{
			if(LivePool[_Loop]->m_Age >= DEATH_AGE)
			{
				if(LivePool[_Loop]->UpdateParticle(_TimeCounter, _Loop))
					m_uParticlesAlive++;
			} 
      }   
	}

	if(flag == ONLY_CREATE || flag == UPDATE_AND_CREATE)
	{
		// Now calcuate how many particles we should create based on time
		// taking the previous frames emission residue into account.
		_ParticlesNeeded += ((m_uParticlesPerSec * _TimeCounter) + m_EmissionResidue);

      float ratio = 0.0f;

      if(m_DistanceToCamera <= m_MinLodDistance && (m_LevelOfDetail == LOD_NEAR || m_LevelOfDetail == LOD_BOTH))
      {
         _ParticlesNeeded *= ((float)m_DistanceToCamera / m_MinLodDistance); 
      }
     
      else if(m_LevelOfDetail == LOD_FAR || m_LevelOfDetail == LOD_BOTH)
      {
         if(m_DistanceToCamera >= m_MaxLodDistance)
            _ParticlesNeeded = 0;

         ratio = ((float)m_DistanceToCamera / m_MaxLodDistance);
         _ParticlesNeeded -= _ParticlesNeeded * ratio;		 
      }

		_ParticlesCreated = (unsigned int)_ParticlesNeeded;


		if(!m_Stopped)
		{
			// This will remember the difference bewteen how many we wanted to create
			m_EmissionResidue = _ParticlesNeeded - _ParticlesCreated;
		}
		else
		{
			m_EmissionResidue = _ParticlesNeeded;
			_ParticlesCreated = 0;
		}

		// Lets make sure that we actually have a particle, or two
		if(_ParticlesCreated < 1)
		{
			m_PrevLocation.x = m_Location.x;
			m_PrevLocation.y = m_Location.y;
			m_PrevLocation.z = m_Location.z;
		}
	
      while(!g_ParticleManager.ParticlePool.empty())
      {
         if(!_ParticlesCreated)
            break;

         CParticle * part = g_ParticleManager.ParticlePool.back();
         part->CreateParticle(this, _TimeCounter);
         _ParticlesCreated--;
         LivePool.push_back(part);
         g_ParticleManager.ParticlePool.pop_back();
      }

/*
      while(LivePool.size() < MAX_PARTICLES)
      {
         // Check the dead pool first
         if(!DeadPool.empty())
         {
            if(!_ParticlesCreated)
				   break;   

            CParticle * part = DeadPool.back();
            part->CreateParticle(this, _TimeCounter);
            _ParticlesCreated--;
            LivePool.push_back(part);
            DeadPool.pop_back();
         }
         
         else
         {
            if(!_ParticlesCreated)
				   break;
            
            CParticle * part = new CParticle;
            part->CreateParticle(this, _TimeCounter);
            _ParticlesCreated--;
            LivePool.push_back(part);
         }
      }
      */
	}
  
	// Success
	return true;
}

//////////////////////////////////////////////////////////////////////////
/// Render to the screen each alive particle in this system
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::RenderSystem(void)
{
	int		   _Loop;
	float		   _Size;
	CVector3f	_Vertex;

   // Update the distance from camera
   for(int i = 0; i < (int)LivePool.size(); i++)
   {
      ParticleDistanceToCamera(LivePool[i]);   
   }
	
   // Sort the particles for rendering (FRONT TO BACK)
   std::stable_sort(LivePool.begin(), LivePool.end(), CompareParticleDistance);

   glDepthMask(false);
   pGLSTATE->SetState(ALPHA_TEST, true);
	pGLSTATE->SetState(DEPTH_TEST, true);
	pGLSTATE->SetState(BLEND, true);
   pGLSTATE->SetState(FOG, false);
	glBlendFunc(m_SrcBlendMode, m_DstBlendMode);
	pGLSTATE->SetState(TEXTURE_2D, true);
	pGLSTATE->SetState(LIGHTING, false);
//   pGLSTATE->SetState(LIGHT1, false);

	// Get the texture for this system
	g_TextureManager.ActivateTexture(GetTextureID());
	//glPushMatrix();

		float _ViewMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, _ViewMatrix);

		CVector3f _Right; 
		_Right.x = _ViewMatrix[0];
		_Right.y = _ViewMatrix[4];
		_Right.z = _ViewMatrix[8];

		CVector3f _Up; 
		_Up.x = _ViewMatrix[1];
		_Up.y = _ViewMatrix[5];
		_Up.z = _ViewMatrix[9];

      if(!LivePool.empty())
      {
		   // For each particle we draw it as a square made up of two triangle strips
		   for(_Loop = 0; _Loop < (int)LivePool.size(); _Loop++)
		   {
            CParticle * temp = LivePool[_Loop];
            int i = 0;
			   _Size = LivePool[_Loop]->m_Size * 0.5f;
			   LivePool[_Loop]->m_Color[3] = LivePool[_Loop]->m_Alpha;

			   if(LivePool[_Loop]->m_Age >= DEATH_AGE)
			   {
				   glColor4fv(LivePool[_Loop]->m_Color);

				   glBegin(GL_TRIANGLE_STRIP);

					   _Vertex.x = LivePool[_Loop]->m_Location.x;
					   _Vertex.y = LivePool[_Loop]->m_Location.y;
					   _Vertex.z = LivePool[_Loop]->m_Location.z;

					   CVector3f _Result;

					   // TOP LEFT
					   glTexCoord2f(0.0,1.0);
					   Vec3fSubtract(&_Result, &_Up, &_Right);
					   Vec3fScale(&_Result, &_Result, _Size);
					   Vec3fAdd(&_Result, &_Vertex, &_Result);
					   glVertex3f(_Result.x, _Result.y, _Result.z);
					   
					   // BOTTOM LEFT
					   glTexCoord2f(0.0,0.0);
					   Vec3fAdd(&_Result, &_Right, &_Up);
					   Vec3fScale(&_Result, &_Result, -_Size);
					   Vec3fAdd(&_Result, &_Vertex, &_Result);
					   glVertex3f(_Result.x, _Result.y, _Result.z);

					   // BOTTOM RIGHT
					   glTexCoord2f(1.0,1.0);
					   Vec3fAdd(&_Result, &_Right, &_Up);
					   Vec3fScale(&_Result, &_Result, _Size);
					   Vec3fAdd(&_Result, &_Vertex, &_Result);
					   glVertex3f(_Result.x, _Result.y, _Result.z);

					   // TOP RIGHT
					   glTexCoord2f(1.0,0.0);
					   Vec3fSubtract(&_Result, &_Right, &_Up);
					   Vec3fScale(&_Result, &_Result, _Size);
					   Vec3fAdd(&_Result, &_Vertex, &_Result);
					   glVertex3f(_Result.x, _Result.y, _Result.z);
		   
				   glEnd();
			   }
		   }
      }

	//glPopMatrix();

	// Go back to attributes before rendering the particle system
	glDepthMask(true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   pGLSTATE->SetState(ALPHA_TEST, false);
	pGLSTATE->SetState(BLEND, false);
	pGLSTATE->SetState(LIGHTING, true);
	pGLSTATE->SetState(DEPTH_TEST, true);
//   pGLSTATE->SetState(LIGHT1, true);
   pGLSTATE->SetState(FOG, true);
}

//////////////////////////////////////////////////////////////////////////
/// Get particle location specified 
/// 
/// Input:     int coord - The vertex to get
///
/// Returns:   Return the location
//////////////////////////////////////////////////////////////////////////

float CParticleSystem::GetLocation(int coord)
{	
	// Return the specified vertex
	switch(coord)
	{
		case GET_X:
			return m_Location.x;
			break;
		case GET_Y:
			return m_Location.y;
			break;
		case GET_Z:
			return m_Location.z;
			break;
	}

	// You sent in garbage you get garbage
	return 0;
}

//////////////////////////////////////////////////////////////////////////
/// Set a particles location (x, y, z)
/// 
/// Input:     float x - x location to set
///            float y - y location to set
///            float z - z location to set
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetLocation(float x, float y, float z)
{
	m_Location.x = x;
	m_Location.y = y;
	m_Location.z = z;
}

//////////////////////////////////////////////////////////////////////////
/// Set the particles velocity
/// 
/// Input:     float xv - X Velocity
///            float yv - Y Velocity
///            float zv - Z Velocity
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetVelocity(float xv, float yv, float zv)
{
	m_Velocity.x = xv;
	m_Velocity.y = yv;
	m_Velocity.z = zv;
}

//////////////////////////////////////////////////////////////////////////
/// Set the size of the particles both intial and final
/// 
/// Input:     float startSize - size of newly created particle
///            float endSize   - size of old particle
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetSize(float startSize, float endSize)
{
	m_StartSize = startSize;
	m_EndSize = endSize;
}

//////////////////////////////////////////////////////////////////////////
/// Set the alpha transparency of a particle both start and final
/// 
/// Input:     float startAlpha - newly created particles transparency
///            float endAlpha   - old particles transparency
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetAlpha(float startAlpha, float endAlpha)
{
	m_StartAlpha = startAlpha;
	m_EndAlpha = endAlpha;
}

//////////////////////////////////////////////////////////////////////////
/// Set any special properties that this system may have
/// 
/// Input:     int flag   - special property of this system
///            bool state - set it on or off
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetSystemFlag(int flag, bool state)
{
	switch(flag)
	{
		case ATTRACTION:
			m_Attracting = state;
			break;
		case STOP:
			m_Stopped = state;
			break;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Set a particle systems start and end colors
/// 
/// Input:     float startRed   - inital red value
///            float startGreen - intial green value
///            float startBlue  - intial blue value
/// 		   float endRed     - final red value
///            float endGreen   - final green value
///            float endBlue    - final blue value
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetColor(float startRed, float startGreen, float startBlue,
							   float endRed, float endGreen, float endBlue)
{
	m_StartColor.x = startRed;
	m_StartColor.y = startGreen;
	m_StartColor.z = startBlue;

	m_EndColor.x = endRed;
	m_EndColor.y = endGreen;
	m_EndColor.z = endBlue;
}

//////////////////////////////////////////////////////////////////////////
/// Set any special properties that this system may have
/// 
/// Input:     int flag   - special property of this system
///            bool state - set it on or off
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetSpread(float spreadFactor, float spreadMinX, float spreadMaxX,
				   float spreadMinY, float spreadMaxY,
				   float spreadMinZ, float spreadMaxZ)
{
	m_SpreadMin.x = spreadMinX;
	m_SpreadMax.x = spreadMaxX;
	m_SpreadMin.y = spreadMinY;
	m_SpreadMax.y = spreadMaxY;
	m_SpreadMin.z = spreadMinZ;
	m_SpreadMax.z = spreadMaxZ;

	if(spreadFactor == 0)
		spreadFactor = 1;

	m_SpreadFactor = spreadFactor;
}

//////////////////////////////////////////////////////////////////////////
/// Set any outside forces (gravity) that act on particles in that system
/// 
/// Input:     float xPull - Pull in the X direction
///            float yPull - Pull in the Y direction
///            float zPull - Pull in the Z direction
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetGravity(float xPull, float yPull, float zPull)
{
	m_Gravity.x = xPull;
	m_Gravity.y = yPull;
	m_Gravity.z = zPull;
}

//////////////////////////////////////////////////////////////////////////
/// Set the time in seconds that a system stays alive
/// 
/// Input:     systemLifeSeconds - Time in seconds a system stays alive
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetSystemLifeSpan(float systemLifeSeconds)
{
	m_SystemDyingTime = systemLifeSeconds;
}

//////////////////////////////////////////////////////////////////////////
/// Set the attraction power of each particle
/// 
/// Input:     attractX - Level of attraction in X axis
///            attractY - Level of attraction in Y axis
///            attractZ - Level of attraction in Z axis
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetAttractionPower(float attractX, float attractY, float attractZ)
{
	m_Attract.x = attractX;
	m_Attract.y = attractY;
	m_Attract.z = attractZ;
}

//////////////////////////////////////////////////////////////////////////
/// Set the OpenGL blend modes for this particle system
/// 
/// Input:     GLenum source - Source blend mode
///            GLenum dst - Destination blend mode
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::SetBlendModes(GLenum source, GLenum dst)
{
	m_SrcBlendMode = source;
	m_DstBlendMode = dst;
}

//////////////////////////////////////////////////////////////////////////
/// Distance from the particle emitter to the camera
/// 
/// Input:     CVector3f cameraLoc - Location of the camera
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticleSystem::DistanceToCamera(CVector3f * cameraLoc)
{
	float temp = ((m_Location.x - cameraLoc->x) * (m_Location.x - cameraLoc->x)) +
	((m_Location.y - cameraLoc->y) * (m_Location.y - cameraLoc->y)) +
	((m_Location.z - cameraLoc->z) * (m_Location.z - cameraLoc->z));

	m_DistanceToCamera = sqrt(temp);
}

//////////////////////////////////////////////////////////////////////////
/// Compare the distance to camera of two particles
/// 
/// Input:     CParticle a, b - The two particles to check
///
/// Returns:   true if particle a is farther away than particle b
//////////////////////////////////////////////////////////////////////////

bool CompareParticleDistance(const CParticle *a, const CParticle *b)
{ 
   if (a->m_DistanceToCamera < b->m_DistanceToCamera)
      return true;
   else 
      return false;
}

//////////////////////////////////////////////////////////////////////////
/// Obtain the distance from camera to a particle
/// 
/// Input:     CParticle *part - the particle to measure
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void ParticleDistanceToCamera(CParticle *part)
{
   CVector3f cameraLoc; 
   g_Camera->GetPosition(&cameraLoc);

   float temp = ((part->m_Location.x - cameraLoc.x) * (part->m_Location.x - cameraLoc.x)) +
	((part->m_Location.y - cameraLoc.y) * (part->m_Location.y - cameraLoc.y)) +
	((part->m_Location.z - cameraLoc.z) * (part->m_Location.z - cameraLoc.z));
	
   part->m_DistanceToCamera = temp;
}



