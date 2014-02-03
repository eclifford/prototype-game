/// \file oglwindow.h
/// OpenGL setup and intialization
//////////////////////////////////////////////////////////////////////////

#pragma once

#define VC_EXTRALEAN
#include <windows.h>
//#include "core\oglstatemanager.h"

class COpenGLWindow
{
    public:
      struct Application
      {
         HINSTANCE		hInstance;				// Application Instance
	      const char*		className;				// Application ClassName
      };

      struct GLWindowInit
      {
	      Application*	application;			// Application Structure
	      char*				title;					// Window Title
	      int				width;					// Width
	      int				height;					// Height
	      int				bitsPerPixel;			// Bits Per Pixel
	      bool				isFullScreen;			// FullScreen?
      };

      struct GLWindow
      {
	      HWND				hWnd;						// Window Handle
	      HDC				hDC;						// Device Context
	      HGLRC				hRC;						// Rendering Context
	      GLWindowInit	init;						// Window Init
	      BOOL				isVisible;				// Window Visible?
	      DWORD				lastTickCount;			// Tick Counter
      };

      // Instance of a window
      GLWindow		window;	

      // GL viewport settings
	   float m_NearPlane;
      float m_FarPlane;
      double m_FOV;
      float m_StartTime;

      int m_ClientWidth;
      int m_ClientHeight;
   
      void TerminateApplication (GLWindow* window);
      bool ResizeScene(int Width, int Height, float NearPlane, float FarPlane, double Fov);
      bool ChangeScreenResolution (int width, int height, int bitsPerPixel);
      bool CreateWindowGL (GLWindow* window, int width, int height, int bits, bool fullscreen);
      bool DestroyWindowGL (GLWindow* window);
      bool OpenGLInit(void);
      bool ClearScreen(void);

      static void DeleteInstance(void);
		static COpenGLWindow *GetInstance(void);
		~COpenGLWindow() {}; // Destructor

      inline HWND GetHWND(void)
		{
         return window.hWnd;
		}

      inline HINSTANCE GetHINSTANCE(void)
      {
         return window.init.application->hInstance;
      }
      
		inline int GetScreenWidth(void)
		{
			return window.init.width;
		}

		inline int GetScreenHeight(void)
		{
			return window.init.height;
		}

      inline int GetScreenBitDepth(void)
		{
			return window.init.bitsPerPixel;
		}

   private:
      static COpenGLWindow * pInstance; // Pointer to instance of singleton class
      COpenGLWindow() {};
		COpenGLWindow(const &COpenGLWindow) {};
		COpenGLWindow &operator = (const &COpenGLWindow);
};