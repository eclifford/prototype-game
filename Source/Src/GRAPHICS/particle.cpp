/// \file particle.cpp
/// Individual particles cpp
//////////////////////////////////////////////////////////////////////////

#include "graphics\particle.h"
#include "utils\utils.h"
#include "graphics\particlemanager.h"
#include "camera\camera.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CBaseCamera *g_Camera;
extern CParticleManager g_ParticleManager;

/////////////////////////////////
/*/ External Globals          /*/ 
/////////////////////////////////

extern float cos_look[360];
extern float sin_look[360];
extern float g_FrameTime;

//////////////////////////////////////////////////////////////////////////
/// Constructor for a particle
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CParticle::CParticle()
{
	// Intialize all the starting attributes
	m_DyingAge = m_Size = m_SizeChange = m_Alpha = m_AlphaChange = 0.0f;

   m_Age = -1.0f;

	// Initialize color to black totally transparent
	m_Color[0] = 0.0f;
	m_Color[1] = 0.0f;
	m_Color[2] = 0.0f;
	m_Color[3] = 0.0f;

	// Initialize ColorChange per frame to 0.0
	m_ColorChange[0] = 0.0f;
	m_ColorChange[1] = 0.0f;
	m_ColorChange[2] = 0.0f;
	m_ColorChange[3] = 0.0f;

   m_DistanceToCamera = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
/// Destructor for a particle
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CParticle::~CParticle()
{
	
}

//////////////////////////////////////////////////////////////////////////
/// Append this particle to a system / effect
/// 
/// Input:     CParticleSystem - The parent system
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticle::SetParentSystem(CParticleSystem *parent)
{
	m_pParent = parent;
}

//////////////////////////////////////////////////////////////////////////
/// Create a particle
/// 
/// Input:     *parent   - The parent system of created particle
///            alphatime - Time expired since particle creation
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CParticle::CreateParticle(CParticleSystem *parent, float alphaTime)
{
	CVector3f _TempVelocity;
	float	    _RandomYaw;
   float     _RandomPitch;
	float	    _NewSpeed;

	// The particle has just been created
	m_Age = 0.0;
	m_DyingAge = (parent->m_ParticleLife) + (RandomFloatRange(parent->m_ParticleLifeVar.max, parent->m_ParticleLifeVar.min));
	CHECK_RANGE(m_DyingAge, MIN_LIFETIME, MAX_LIFETIME);

	// Set up the particles current color with some randomness
	m_Color[0] = (parent->m_StartColor.x)+(RandomFloatRange(parent->m_ColorVar.min, parent->m_ColorVar.max));
	m_Color[1] = (parent->m_StartColor.y)+(RandomFloatRange(parent->m_ColorVar.min, parent->m_ColorVar.max));
	m_Color[2] = (parent->m_StartColor.z)+(RandomFloatRange(parent->m_ColorVar.min, parent->m_ColorVar.max));
	m_Color[3] = 1.0f;

	// Keep all colors in bounds 
	CHECK_RANGE(m_Color[0], MIN_COLOR, MAX_COLOR);
	CHECK_RANGE(m_Color[1], MIN_COLOR, MAX_COLOR);
	CHECK_RANGE(m_Color[2], MIN_COLOR, MAX_COLOR);

	// We must calculate the color changed needed per frame so that when the particle
	// dies we will have reached the particles desired end color
	m_ColorChange[0] = ((parent->m_EndColor.x) - m_Color[0]) / m_DyingAge;
	m_ColorChange[1] = ((parent->m_EndColor.y) - m_Color[1]) / m_DyingAge;
	m_ColorChange[2] = ((parent->m_EndColor.z) - m_Color[2]) / m_DyingAge;

	// Calculate the particles current alpha value
	m_Alpha = (parent->m_StartAlpha) + (RandomFloatRange(parent->m_AlphaVar.min, parent->m_AlphaVar.max));

	// Keep the alpha value in bounds
	CHECK_RANGE(m_Alpha, MIN_ALPHA, MAX_ALPHA);

	// Calculate the AlphaChange needed per frame so that when particle
	// dies we will ahve reached the desired EndAlpha
	m_AlphaChange = ((parent->m_EndAlpha) - m_Alpha) / m_DyingAge;

	// Calculate the current particles size
	m_Size = (parent->m_StartSize) + (RandomFloatRange(parent->m_SizeVar.min, parent->m_SizeVar.max));

	// Keep the size in bounds
	CHECK_RANGE(m_Size, MIN_SIZE, MAX_SIZE);

	// Calcuate the SizeChange needed per frame so that when particle 
	// dies we will have reached the desired EndSize
	m_SizeChange = ((parent->m_EndSize) - m_Size) / m_DyingAge;

	// Emit the particle from it's current location using the Velocity's found above
	m_Location.x = parent->m_Location.x;
	m_Location.y = parent->m_Location.y;
	m_Location.z = parent->m_Location.z;

   if(parent->m_IsBuilding == true)
   {
      int tempx = rand() % 3;

      if(tempx == 1)
          m_Location.x += parent->m_SpreadMin.x;
      else if(tempx == 2)
         m_Location.x += parent->m_SpreadMax.x;
      else
         m_Location.x += (RandomFloatRange((parent->m_SpreadMin.x), (parent->m_SpreadMax.x))) / (parent->m_SpreadFactor);

	   m_Location.y += (RandomFloatRange((parent->m_SpreadMin.y), (parent->m_SpreadMax.y))) / (parent->m_SpreadFactor);

      int tempz = rand() % 3;

      if(tempz == 1)
          m_Location.z += parent->m_SpreadMin.z;
      else if(tempz == 2)
         m_Location.z += parent->m_SpreadMax.z;
      else
         m_Location.z += (RandomFloatRange((parent->m_SpreadMin.z), (parent->m_SpreadMax.z))) / (parent->m_SpreadFactor);

   }
   else
   {
	   // Use our X, Y, and Z SpreadFactors to randomize somewhat our particles emittance point.
	   m_Location.x += (RandomFloatRange((parent->m_SpreadMin.x), (parent->m_SpreadMax.x))) / (parent->m_SpreadFactor);
	   m_Location.y += (RandomFloatRange((parent->m_SpreadMin.y), (parent->m_SpreadMax.y))) / (parent->m_SpreadFactor);
	   m_Location.z += (RandomFloatRange((parent->m_SpreadMin.z), (parent->m_SpreadMax.z))) / (parent->m_SpreadFactor);
   }

	// Save our location as the previous location
	parent->m_PrevLocation.x = parent->m_Location.x;
	parent->m_PrevLocation.y = parent->m_Location.y;
	parent->m_PrevLocation.z = parent->m_Location.z;

	// The emitter has a direction.  This is where we fine it:
	_RandomYaw   = (float)(RANDOM_FLOAT * PI * 2.0f);
	_RandomPitch = (float)DEG_TO_RAD(RANDOM_FLOAT * ((parent->m_Angle)));

	// The following code uses spherical coordinates to randomize the velocity vector
	// of the particle
	m_Velocity.x = (cosf(_RandomPitch)) * (parent->m_Velocity.x);
	m_Velocity.y = (sinf(_RandomPitch) * cosf(_RandomYaw)) * (parent->m_Velocity.y);
	m_Velocity.z = (sinf(_RandomPitch) * sinf(_RandomYaw)) * (parent->m_Velocity.z);
   
   //m_Velocity.x = (cos_look[(int)_RandomPitch]) * (parent->m_Velocity.x);
	//m_Velocity.y = (sin_look[(int)_RandomPitch] * cos_look[(int)_RandomYaw]) * (parent->m_Velocity.y);
	//m_Velocity.z = (sin_look[(int)_RandomPitch] * sin_look[(int)_RandomYaw]) * (parent->m_Velocity.z);

	// Velocity at this point is just a direction (normalized vector) and needs to be multiplied by the speed 
	// component to be legit.
	_NewSpeed = parent->m_Speed + RandomFloatRange(parent->m_SpeedVar.min, parent->m_SpeedVar.max);
	CHECK_RANGE(_NewSpeed, MIN_SPEED, MAX_SPEED);
	
	m_Velocity.x *= _NewSpeed;
	m_Velocity.y *= _NewSpeed;
	m_Velocity.z *= _NewSpeed;

	// Set this particle's parent
	SetParentSystem(parent);
}

//////////////////////////////////////////////////////////////////////////
/// UpdateParticle
/// 
/// Input:     alphaTime - Time expired since particle creation
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

bool CParticle::UpdateParticle(float alphaTime, int index)
{  
	static CVector3f _AttractLocation;
	static CVector3f _AttractNormal;

	// Age the particle
	m_Age += g_FrameTime;

	if(m_Age >= m_DyingAge)
	{
		// Kill the particle.
		m_Age = -1.0f;

      m_pParent->LivePoolIterator = &m_pParent->LivePool[index];
      m_pParent->LivePool.erase(m_pParent->LivePoolIterator);
      //m_pParent->DeadPool.push_back(this);
      g_ParticleManager.ParticlePool.push_back(this);
      
		return false;
	}

	// Set the particles previous location with the location that will be the old 
	// one by the time we get through this function
	m_PrevLocation.x = m_Location.x;
	m_PrevLocation.y = m_Location.y;
	m_PrevLocation.z = m_Location.z;

   // Update the particles velocity by the gravity vector time.
	m_Velocity.x += (m_pParent->m_Gravity.x * g_FrameTime);
	m_Velocity.y += (m_pParent->m_Gravity.y * g_FrameTime);
	m_Velocity.z += (m_pParent->m_Gravity.z * g_FrameTime);

	// Move the particles current location
	m_Location.x += (m_Velocity.x * g_FrameTime);
	m_Location.y += (m_Velocity.y * g_FrameTime);
	m_Location.z += (m_Velocity.z * g_FrameTime);

	// Each particle is attracted to the center of the system
	if(m_pParent->GetAttracting())
	{
		// Find out where our Parent is located so we can track it
		_AttractLocation.x = m_pParent->GetLocation(GET_X);
		_AttractLocation.y = m_pParent->GetLocation(GET_Y);
		_AttractLocation.z = m_pParent->GetLocation(GET_Z);

		// Calculate the vector bewteen the particle and the attractor
		_AttractNormal.x = _AttractLocation.x - m_Location.x;
		_AttractNormal.y = _AttractLocation.y - m_Location.y;
		_AttractNormal.z = _AttractLocation.z - m_Location.z;

		// We can turn off attraction for certain axes to create some cool effects 
		glNormal3f(_AttractNormal.x, _AttractNormal.y, _AttractNormal.z);
      //Vec3fNormalize(&_AttractNormal, &_AttractNormal);

		m_Velocity.x += (_AttractNormal.x * m_pParent->m_Attract.x * g_FrameTime);
		m_Velocity.y += (_AttractNormal.y * m_pParent->m_Attract.y * g_FrameTime);
		m_Velocity.z += (_AttractNormal.z * m_pParent->m_Attract.z * g_FrameTime);
	}


	// Adjust the current color 
	m_Color[0] += (m_ColorChange[0] * g_FrameTime);
	m_Color[1] += (m_ColorChange[1] * g_FrameTime);
	m_Color[2] += (m_ColorChange[2] * g_FrameTime);

	// Adjust the alhpa values
	m_Alpha += (m_AlphaChange * g_FrameTime);

	// Adjust the current size
	m_Size += (m_SizeChange * g_FrameTime);

	// Finally we set the color vector spot to the alpha
	m_Color[3] = m_Alpha;

	return true;
}
