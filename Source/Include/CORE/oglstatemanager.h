/// \file oglwindow.h
/// OpenGL setup and intialization
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")

//////////////////////////////////////////////////////////////////////////
// GL BOOLEAN STATE VALUES
//////////////////////////////////////////////////////////////////////////

// Size of the lookup table
#define LOOKUP_SIZE 19

// Indices of lookup table
#define ALPHA_TEST		 0
#define BLEND		     1
#define COLOR_MATERIAL   2
#define CULL_FACE        3
#define DEPTH_TEST       4
#define FOG              5
#define LIGHT1           6 
#define LIGHT2           7
#define LIGHT3       	 8
#define LIGHT4   		 9
#define LIGHT5			 10
#define LIGHT6			 11
#define LIGHT7           12
#define LIGHTING         14
#define POINT_SMOOTH     15
#define POLYGON_SMOOTH   16
#define TEXTURE_2D		 17
#define AUTO_NORMAL		 18

class COpenGLStateManager
{
	public:

		void Initialize();
		static void DeleteInstance(void);
		static COpenGLStateManager *GetInstance(void);
		~COpenGLStateManager() {}; // Destructor

		//////////////////////////////////////////////////////////////////////////
		/// Set up the State lookup table
		/// 
		/// Input:     void
		///
		/// Returns:   void
		//////////////////////////////////////////////////////////////////////////

		void InitializeStates(void);
		
		//////////////////////////////////////////////////////////////////////////
		/// Set up the bit lookup table
		/// 
		/// Input:     void
		///
		/// Returns:   void
		//////////////////////////////////////////////////////////////////////////

		void SetStateLookupTable(void);

		//////////////////////////////////////////////////////////////////////////
		/// To obtain intial states from OpenGL
		/// 
		/// Input:     void
		///
		/// Returns:   void
		//////////////////////////////////////////////////////////////////////////

		void SetBitLookupTable(void);

		//////////////////////////////////////////////////////////////////////////
		/// Set an OpenGL state instead of calling glEnable
		/// 
		/// Input:     void
		///
		/// Returns:   void
		//////////////////////////////////////////////////////////////////////////

		void SetState(int index, bool turnon);

	private:
		static COpenGLStateManager * pInstance; // Pointer to instance of singleton class
		COpenGLStateManager() {};
		COpenGLStateManager(const &COpenGLStateManager) {};
		COpenGLStateManager &operator = (const &COpenGLStateManager);

		unsigned int m_BitField;	// Bitfield of states
};
