/// \file oglstatemanager.cpp
/// Sets up OpenGL States
//////////////////////////////////////////////////////////////////////////

#include "core\oglstatemanager.h"

#include "utils\mmgr.h"

//  Initialize the pointer to null.
COpenGLStateManager *COpenGLStateManager::pInstance = 0;

// Bits associated with each state ON or OFF 
unsigned int ALPHA_TEST_BIT	    = 0x01 << ALPHA_TEST; 
unsigned int BLEND_BIT			= 0x01 << BLEND;
unsigned int COLOR_MATERIAL_BIT	= 0x01 << COLOR_MATERIAL;
unsigned int CULL_FACE_BIT		= 0x01 << CULL_FACE;
unsigned int DEPTH_TEST_BIT     = 0x01 << DEPTH_TEST;
unsigned int FOG_BIT			= 0x01 << FOG;
unsigned int LIGHT1_BIT			= 0x01 << LIGHT1;
unsigned int LIGHT2_BIT         = 0x01 << LIGHT2;
unsigned int LIGHT3_BIT       	= 0x01 << LIGHT3;
unsigned int LIGHT4_BIT   		= 0x01 << LIGHT4;
unsigned int LIGHT5_BIT			= 0x01 << LIGHT5;
unsigned int LIGHT6_BIT			= 0x01 << LIGHT6;
unsigned int LIGHT7_BIT         = 0x01 << LIGHT7;
unsigned int LIGHTING_BIT       = 0x01 << LIGHTING;
unsigned int POINT_SMOOTH_BIT   = 0x01 << POINT_SMOOTH;
unsigned int POLYGON_SMOOTH_BIT = 0x01 << POLYGON_SMOOTH;
unsigned int TEXTURE_2D_BIT		= 0x01 << TEXTURE_2D;
unsigned int AUTO_NORMAL_BIT	= 0x01 << AUTO_NORMAL;

// Bit lookup table
unsigned int GL_BIT_LOOKUP[LOOKUP_SIZE];

// State lookup table
int GL_STATE_LOOKUP[LOOKUP_SIZE];


//////////////////////////////////////////////////////////////////////////
/// Initalize the State Manager
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void COpenGLStateManager::Initialize()
{
	m_BitField = 0;
	InitializeStates();
	SetStateLookupTable();
	SetBitLookupTable();
}

//////////////////////////////////////////////////////////////////////////
/// Set up the State lookup table
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void COpenGLStateManager::SetStateLookupTable(void)
{
	GL_STATE_LOOKUP[0]  = GL_ALPHA_TEST;
	GL_STATE_LOOKUP[1]  = GL_BLEND;
	GL_STATE_LOOKUP[2]  = GL_COLOR_MATERIAL;
	GL_STATE_LOOKUP[3]  = GL_CULL_FACE;
	GL_STATE_LOOKUP[4]  = GL_DEPTH_TEST;
	GL_STATE_LOOKUP[5]  = GL_FOG;
	GL_STATE_LOOKUP[6]  = GL_LIGHT1;
	GL_STATE_LOOKUP[7]  = GL_LIGHT2;
	GL_STATE_LOOKUP[8]  = GL_LIGHT3;
	GL_STATE_LOOKUP[9]  = GL_LIGHT4;
	GL_STATE_LOOKUP[10] = GL_LIGHT5;
	GL_STATE_LOOKUP[11] = GL_LIGHT6;
	GL_STATE_LOOKUP[12] = GL_LIGHT7;
	GL_STATE_LOOKUP[14] = GL_LIGHTING;
	GL_STATE_LOOKUP[15] = GL_POINT_SMOOTH;
	GL_STATE_LOOKUP[16] = GL_POLYGON_SMOOTH;
	GL_STATE_LOOKUP[17] = GL_TEXTURE_2D;
	GL_STATE_LOOKUP[18] = GL_AUTO_NORMAL;
}

//////////////////////////////////////////////////////////////////////////
/// Set up the bit lookup table
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void COpenGLStateManager::SetBitLookupTable(void)
{
	GL_BIT_LOOKUP[0]  = ALPHA_TEST_BIT;
	GL_BIT_LOOKUP[1]  = BLEND_BIT;
	GL_BIT_LOOKUP[2]  = COLOR_MATERIAL_BIT;
	GL_BIT_LOOKUP[3]  = CULL_FACE_BIT;
	GL_BIT_LOOKUP[4]  = DEPTH_TEST_BIT;
	GL_BIT_LOOKUP[5]  = FOG_BIT;
	GL_BIT_LOOKUP[6]  = LIGHT1_BIT;
	GL_BIT_LOOKUP[7]  = LIGHT2_BIT;
	GL_BIT_LOOKUP[8]  = LIGHT3_BIT;
	GL_BIT_LOOKUP[9]  = LIGHT4_BIT;
	GL_BIT_LOOKUP[10] = LIGHT5_BIT;
	GL_BIT_LOOKUP[11] = LIGHT6_BIT;
	GL_BIT_LOOKUP[12] = LIGHT7_BIT;
	GL_BIT_LOOKUP[14] = LIGHTING_BIT;
	GL_BIT_LOOKUP[15] = POINT_SMOOTH_BIT;
	GL_BIT_LOOKUP[16] = POLYGON_SMOOTH_BIT;
	GL_BIT_LOOKUP[17] = TEXTURE_2D_BIT;
	GL_BIT_LOOKUP[18] = AUTO_NORMAL_BIT;
}

//////////////////////////////////////////////////////////////////////////
/// Pool OpenGL to obtain intial States
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void COpenGLStateManager::InitializeStates(void)
{
	if(glIsEnabled(GL_ALPHA_TEST))
	{
       m_BitField |= ALPHA_TEST_BIT; 
	}

	if(glIsEnabled(GL_BLEND))
	{
        m_BitField |= BLEND_BIT; 
	}

	if(glIsEnabled(GL_COLOR_MATERIAL))
	{
        m_BitField |= COLOR_MATERIAL_BIT; 
	}

	if(glIsEnabled(GL_CULL_FACE))
	{
        m_BitField |= CULL_FACE_BIT; 
	}

	if(glIsEnabled(GL_DEPTH_TEST))
	{
        m_BitField |= DEPTH_TEST_BIT; 
	}

	if(glIsEnabled(GL_FOG))
	{
        m_BitField |= FOG_BIT; 
	}

	if(glIsEnabled(GL_LIGHT1))
	{
        m_BitField |= LIGHT1_BIT; 
	}

	if(glIsEnabled(GL_LIGHT2))
	{
        m_BitField |= LIGHT2_BIT; 
	}

	if(glIsEnabled(GL_LIGHT3))
	{
        m_BitField |= LIGHT3_BIT; 
	}

	if(glIsEnabled(GL_LIGHT4))
	{
        m_BitField |= LIGHT4_BIT; 
	}
	
	if(glIsEnabled(GL_LIGHT5))
	{
        m_BitField |= LIGHT5_BIT; 
	}

	if(glIsEnabled(GL_LIGHT6))
	{
        m_BitField |= LIGHT6_BIT; 
	}

	if(glIsEnabled(GL_LIGHT7))
	{
        m_BitField |= LIGHT7_BIT; 
	}

	if(glIsEnabled(GL_LIGHTING))
	{
		m_BitField |= GL_LIGHTING_BIT;
	}

	if(glIsEnabled(GL_POINT_SMOOTH))
	{
		m_BitField |= POINT_SMOOTH_BIT;
	}

	if(glIsEnabled(GL_POLYGON_SMOOTH))
	{
		m_BitField |= POLYGON_SMOOTH_BIT;
	}

	if(glIsEnabled(GL_TEXTURE_2D))
	{
		m_BitField |= TEXTURE_2D_BIT;
	}

	if(glIsEnabled(GL_AUTO_NORMAL))
	{
		m_BitField |= AUTO_NORMAL_BIT;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Set an OpenGL state instead of calling glEnable
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void COpenGLStateManager::SetState(int lookupindex, bool turnon)
{
	// Try to set the state to ON
	if(turnon)
	{
		// State is already ON
		if(m_BitField & GL_BIT_LOOKUP[lookupindex])
			return;

		// Set the state to ON
		else
		{
			glEnable(GL_STATE_LOOKUP[lookupindex]);	
			m_BitField |= GL_BIT_LOOKUP[lookupindex];
		}
	}

	// Try to set the state to OFF
	else
	{
		// State is already OFF
		if(!m_BitField & GL_BIT_LOOKUP[lookupindex])
			return;

		// Set the state to OFF
		else
		{
			glDisable(GL_STATE_LOOKUP[lookupindex]);
			m_BitField = m_BitField & ~GL_BIT_LOOKUP[lookupindex];
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
/// Deletes an Instance of COpenGLStateManager
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void COpenGLStateManager::DeleteInstance(void)
{
	if (pInstance)
	{
		delete pInstance;
		pInstance = 0;
	}
}
 
//////////////////////////////////////////////////////////////////////////
/// Gets an Instance of CGameMain Class
/// 
/// Input:     void
///
/// Returns:   COpenGLWindow * - Instance of the class
//////////////////////////////////////////////////////////////////////////

COpenGLStateManager *COpenGLStateManager::GetInstance(void)
{
    //  Check to see if one hasn’t been made yet.
	if (pInstance == 0)
		pInstance = new COpenGLStateManager;

    //  Return the address of the instance.
	return pInstance;
}
