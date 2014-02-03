/// \file oglwindow.cpp
/// Sets up OpenGL in a Window
//////////////////////////////////////////////////////////////////////////

#define VC_EXTRALEAN
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "core\oglwindow.h"
#include "core\gamestatemanager.h"
#include "core\gamemain.h"
#include "level editor\editormain.h"
#include "graphics\texturemanager.h"
#include "core\startmain.h"
#include "graphics\geometrymanager.h"
#include "camera\frustum.h"
#include "gamelogic\gameoptions.h"
#include "core\oglstatemanager.h"
#include "utils\mmgr.h"


//  Initialize the pointer to null.
COpenGLWindow *COpenGLWindow::pInstance = 0;	

/////////////////////////////////
/*/ RELEASE OR DEBUG          /*/ 
/////////////////////////////////

#define DEBUG_MODE 0
#define RELEASE_MODE 1
unsigned int g_GAMEMODE = DEBUG_MODE;

#define FULLSCREEN 1

/////////////////////////////////
/*/ Global instances          /*/ 
/////////////////////////////////

COpenGLWindow		  *pGLWIN	  = COpenGLWindow::GetInstance();			
COpenGLStateManager *pGLSTATE   = COpenGLStateManager::GetInstance();	
CGameStateManager   GameStateManager;
CGeometryManager    g_GeometryManager;
CTextureManager     g_TextureManager;
CFrustum g_Frustum;
extern CGameMain GameMain;
extern CEditorMain EditorMain;
extern CStartMain StartMain;

/////////////////////////////////
/*/ External Global instances /*/ 
/////////////////////////////////

extern CGameOptions g_GameOptions;

/////////////////////////////////
/*/ Global variables          /*/ 
/////////////////////////////////

static bool g_ProgramRunning;			// Window Creation Loop, For FullScreen/Windowed Toggle																		// Between Fullscreen / Windowed Mode
static bool g_createFullScreen;		// If TRUE, Then Create Fullscreen
float g_time;	
float g_fps = 0.0f;
float g_last_time = 0.0f;
float g_FrameTime;
unsigned int g_frames = 0;	

//////////////////////////////////////////////////////////////////////////
/// Terminate the application
/// 
/// Input:     GLWindow* window - window properties
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void COpenGLWindow::TerminateApplication (GLWindow* window)	
{
	PostMessage (window->hWnd, WM_QUIT, 0, 0);	// post WM_QUIT message
	g_ProgramRunning = false;							// stop the program
}

//////////////////////////////////////////////////////////////////////////
/// Resizes an OpenGL Window / Recalculates viewport
/// 
/// Input:     int	 width  - Width of viewport
///            int   height - Height of viewport
///            float near	- distance of near planee
///            float far	- distance of far plane
///
/// Returns:   bool - True if scene successfully updated
//////////////////////////////////////////////////////////////////////////

bool COpenGLWindow::ResizeScene(int Width, int Height, float NearPlane, float FarPlane, double Fov)
{
   // Set the current viewport settings
   m_NearPlane = NearPlane;
   m_FarPlane = FarPlane;
   m_FOV = Fov;

	// Make sure we do not divide by zero
	if(Height == 0)
		Height = 1;

	// Set the viewport
	glViewport(0, 0, Width, Height);

	// Set the projection matrix
	glMatrixMode(GL_PROJECTION);

	// Clear out the matrix 
	glLoadIdentity(); 

	// Set up the camera view
	gluPerspective(Fov, (double)Width / (double)Height, NearPlane, FarPlane);

	// Initialize the frustrum
	g_Frustum.InitFrustum((float)Fov, (float)Width / (float)Height, NearPlane, FarPlane);

	// Set the model view matrix
	glMatrixMode(GL_MODELVIEW);

	// Clear out the matrix
	glLoadIdentity();

	// Viewport is set 
	return true;
}

//////////////////////////////////////////////////////////////////////////
/// Change the current windows screen resolution
/// 
/// Input:     int	width  - Width of window
///            int   height - Height of window
///            int   bitsPerPixel - Bits per pixel
///
/// Returns:   bool - True if scene successfully updated
//////////////////////////////////////////////////////////////////////////

bool COpenGLWindow::ChangeScreenResolution (int width, int height, int bitsPerPixel)	// Change The Screen Resolution
{
   DEVMODE DeviceSettings;
   DEVMODE dmScreenSettings;											// Device Mode
	ZeroMemory (&dmScreenSettings, sizeof (DEVMODE));			// Make Sure Memory Is Cleared	
   unsigned int HighestFrequency = 0;

   unsigned int i = 0;
   while (EnumDisplaySettings(NULL, i, &DeviceSettings))
   {
      if (DeviceSettings.dmPelsWidth == width && DeviceSettings.dmPelsHeight == height)
      {
         if (DeviceSettings.dmDisplayFrequency > HighestFrequency)
            HighestFrequency = DeviceSettings.dmDisplayFrequency;

         if (HighestFrequency >= 70)
            break;
      }

      ++i;
   }
   
   dmScreenSettings.dmDisplayFrequency = HighestFrequency; 

	dmScreenSettings.dmSize				= sizeof (DEVMODE);		// Size Of The Devmode Structure
	dmScreenSettings.dmPelsHeight		= height;					// Select Screen Height
	dmScreenSettings.dmPelsWidth		= width;						// Select Screen Width
	dmScreenSettings.dmBitsPerPel		= bitsPerPixel;			// Select Bits Per Pixel
	dmScreenSettings.dmFields			= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT |
                                      DM_DISPLAYFREQUENCY;
	
   if (ChangeDisplaySettings (&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
	{
		return FALSE;													// Display Change Failed, Return False
	}
 
   // Resize the scene
   ResizeScene(width, height, m_NearPlane, m_FarPlane, m_FOV);
   
   // Display Change Was Successful, Return True 
  	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
/// Create a opengl window
/// 
/// Input:     GLWindow *window - Window properties
///            float	width  - Width of window
///            float   height - Height of window
///            int   bits - Bits per pixel
///
/// Returns:   bool - True if scene successfully updated
//////////////////////////////////////////////////////////////////////////

bool COpenGLWindow::CreateWindowGL (GLWindow* window, int width, int height, int bits, bool fullscreen)
{
	DWORD windowStyle         = WS_POPUP;				// Define Our Window Style
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;		// Define The Window's Extended Style

   window->init.width = width;
   window->init.height = height;
   pGLWIN->window.init.isFullScreen = fullscreen;	

	PIXELFORMATDESCRIPTOR pfd =							// pfd Tells Windows How We Want Things To Be
	{
		sizeof (PIXELFORMATDESCRIPTOR),					// Size Of This Pixel Format Descriptor
		1,															// Version Number
		PFD_DRAW_TO_WINDOW |									// Format Must Support Window
		PFD_SUPPORT_OPENGL |									// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,										// Must Support Double Buffering
		PFD_TYPE_RGBA,											// Request An RGBA Format
		bits,										            // Select Our Color Depth
		0, 0, 0, 0, 0, 0,										// Color Bits Ignored
		0,													   	// No Alpha Buffer
		0,															// Shift Bit Ignored
		0,															// No Accumulation Buffer
		0, 0, 0, 0,												// Accumulation Bits Ignored
		16,														// 16Bit Z-Buffer (Depth Buffer)  
		1,															// Use Stencil Buffer
		0,															// No Auxiliary Buffer
		PFD_MAIN_PLANE,										// Main Drawing Layer
		0,															// Reserved
		0, 0, 0												   // Layer Masks Ignored
	};

	RECT windowRect = {0, 0, (long)width, (long)height};	// Define Our Window Coordinates

	GLuint PixelFormat;							// Will Hold The Selected Pixel Format
	if (fullscreen)								// Fullscreen Requested, Try Changing Video Modes
	{
		if (ChangeScreenResolution ((int)width, (int)height, bits) == FALSE)
		{
			// Fullscreen Mode Failed.  Run In Windowed Mode Instead
			MessageBox (HWND_DESKTOP, "Mode Switch Failed.\nRunning In Windowed Mode.", "Error", MB_OK | MB_ICONEXCLAMATION);
			window->init.isFullScreen = FALSE;				// Set isFullscreen To False (Windowed Mode)
		}
		else															// Otherwise (If Fullscreen Mode Was Successful)
		{
//			ShowCursor (FALSE);									// Turn Off The Cursor
			windowStyle = WS_POPUP;								// Set The WindowStyle To WS_POPUP (Popup Window)
			windowExtendedStyle |= WS_EX_TOPMOST;			// Set The Extended Window Style To WS_EX_TOPMOST
		}																// (Top Window Covering Everything Else)
	}
	else																// If Fullscreen Was Not Selected
	{
		// Adjust Window, Account For Window Borders
		AdjustWindowRectEx (&windowRect, windowStyle, 0, windowExtendedStyle);
	}

	// Create The OpenGL Window
	window->hWnd = CreateWindowEx (windowExtendedStyle,				// Extended Style
								   window->init.application->className,	// Class Name
								   window->init.title,					      // Window Title
								   windowStyle,							      // Window Style
								   0, 0,								            // Window X,Y Position
								   windowRect.right - windowRect.left,	   // Window Width
								   windowRect.bottom - windowRect.top,	   // Window Height
								   HWND_DESKTOP,						         // Desktop Is Window's Parent
								   0,									            // No Menu
								   window->init.application->hInstance,   // Pass The Window Instance
								   window);

	if (window->hWnd == 0)											// Was Window Creation A Success?
	{
		return FALSE;													// If Not Return False
	}

	window->hDC = GetDC (window->hWnd);							// Grab A Device Context For This Window
	if (window->hDC == 0)											// Did We Get A Device Context?
	{
		// Failed
		DestroyWindow (window->hWnd);								// Destroy The Window
		window->hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	PixelFormat = ChoosePixelFormat (window->hDC, &pfd);	// Find A Compatible Pixel Format
	if (PixelFormat == 0)											// Did We Find A Compatible Format?
	{
		// Failed
		ReleaseDC (window->hWnd, window->hDC);					// Release Our Device Context
		window->hDC = 0;												// Zero The Device Context
		DestroyWindow (window->hWnd);								// Destroy The Window
		window->hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	if (SetPixelFormat (window->hDC, PixelFormat, &pfd) == FALSE)	
	{
		// Failed
		ReleaseDC (window->hWnd, window->hDC);					// Release Our Device Context
		window->hDC = 0;												// Zero The Device Context
		DestroyWindow (window->hWnd);								// Destroy The Window
		window->hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	window->hRC = wglCreateContext (window->hDC);			// Try To Get A Rendering Context
	if (window->hRC == 0)											// Did We Get A Rendering Context?
	{
		// Failed
		ReleaseDC (window->hWnd, window->hDC);					// Release Our Device Context
		window->hDC = 0;												// Zero The Device Context
		DestroyWindow (window->hWnd);								// Destroy The Window
		window->hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	// Make The Rendering Context Our Current Rendering Context
	if (wglMakeCurrent (window->hDC, window->hRC) == FALSE)
	{
		// Failed
		wglDeleteContext (window->hRC);							// Delete The Rendering Context
		window->hRC = 0;												// Zero The Rendering Context
		ReleaseDC (window->hWnd, window->hDC);					// Release Our Device Context
		window->hDC = 0;												// Zero The Device Context
		DestroyWindow (window->hWnd);							   // Destroy The Window
		window->hWnd = 0;												// Zero The Window Handle
		return FALSE;													// Return False
	}

	ShowWindow (window->hWnd, SW_NORMAL);						// Make The Window Visible
	window->isVisible = TRUE;										// Set isVisible To True

	ResizeScene((int)width, (int)height, m_NearPlane, m_FarPlane, m_FOV);		
/*
   // TEMP PLACE FOR SAVING VIDEO CONFIG FILE
   FILE * stream;
   stream = fopen("config.txt", "w");

   char Width[256];
   itoa(width, Width, 10);
   char Height[256];
   itoa(height, Height, 10);
   char Bitdepth[256];
   itoa(bits, Bitdepth, 10);
   fprintf(stream, "%s\n", "// Prototype Configuration File "); 
   fprintf(stream, "%s\n\n", "/////////////////////////////////////////////////////// "); 
   
   fprintf(stream, "%s%s\n", "Width: ", Width); 
   fprintf(stream, "%s%s\n", "Height: ", Height); 
   fprintf(stream, "%s%s\n", "Bitdepth: ", Bitdepth); 
   
   fclose(stream);
*/
   // Window Creating Was A Success
   return TRUE;																														// Initialization Will Be Done In WM_CREATE
}

//////////////////////////////////////////////////////////////////////////
/// destroy a opengl window
/// 
/// Input:     GLWindow *window - Window to destroy
///
/// Returns:   bool - True if window destroyed
//////////////////////////////////////////////////////////////////////////

bool COpenGLWindow::DestroyWindowGL (GLWindow* window)			
{
	if (window->hWnd != 0)											// Does The Window Have A Handle?
	{
		if (window->hDC != 0)										// Does The Window Have A Device Context?
		{
			wglMakeCurrent (window->hDC, 0);						// Set The Current Active Rendering Context To Zero
			if (window->hRC != 0)									// Does The Window Have A Rendering Context?
			{
				wglDeleteContext (window->hRC);					// Release The Rendering Context
				window->hRC = 0;										// Zero The Rendering Context

			}
			ReleaseDC (window->hWnd, window->hDC);				// Release The Device Context
			window->hDC = 0;											// Zero The Device Context
		}
		DestroyWindow (window->hWnd);								// Destroy The Window
		window->hWnd = 0;												// Zero The Window Handle
	}

	if (window->init.isFullScreen)							   // Is Window In Fullscreen Mode
	{
		ChangeDisplaySettings (NULL,0);							// Switch Back To Desktop Resolution
//		ShowCursor (false);											// Show The Cursor
	}
  // else 
   //   ShowCursor(true);
   
	return TRUE;														// Return True
}

//////////////////////////////////////////////////////////////////////////
/// Initializes the OpenGL API
/// 
/// Input:     void
///
/// Returns:   bool - True if OpenGL is initialized successfully
//////////////////////////////////////////////////////////////////////////

bool COpenGLWindow::OpenGLInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			      // Black Background
   
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
   
   glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

   glHint(GL_FOG_HINT, GL_NICEST);

   glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   
   return true;	
}

//////////////////////////////////////////////////////////////////////////
/// Clears the screen / Readies it for redraw
/// 
/// Input:     int	 width  - Width of viewport
///            int   height - Height of viewport
///            float near	- distance of near planee
///            float far	- distance of far plane
///
/// Returns:   bool - If screen was successfully cleared
//////////////////////////////////////////////////////////////////////////

bool COpenGLWindow::ClearScreen(void)
{
   if (GameStateManager.GetCurrentState() != (CGameStateManager*)(void*)&StartMain)
   {
      if (GameStateManager.GetCurrentState() != (CGameStateManager*)(void*)&GameMain)
	      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      else
         glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
   }
	return true;
}

//////////////////////////////////////////////////////////////////////////
/// Process Window Message Callbacks
/// 
/// Input:   
///
/// Returns:  
//////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Get The Window Context
   COpenGLWindow::GLWindow* window = (COpenGLWindow::GLWindow*)(GetWindowLong (hWnd, GWL_USERDATA));

	switch (uMsg)														// Evaluate Window Message
	{
		case WM_SYSCOMMAND:											// Intercept System Commands
		{
			switch (wParam)											// Check System Calls
			{
				case SC_SCREENSAVE:									// Screensaver Trying To Start?
				case SC_MONITORPOWER:								// Monitor Trying To Enter Powersave?
				return 0;												// Prevent From Happening
			}
			break;														// Exit
		}
		return 0;														// Return

		case WM_CREATE:												// Window Creation
		{
			CREATESTRUCT* creation = (CREATESTRUCT*)(lParam);			// Store Window Structure Pointer
         window = (COpenGLWindow::GLWindow*)(creation->lpCreateParams);
			SetWindowLong (hWnd, GWL_USERDATA, (LONG)(window));
		}
		return 0;														// Return

		case WM_CLOSE:													// Closing The Window
         pGLWIN->TerminateApplication(window);
		return 0;														// Return

		case WM_SIZE:													// Size Action Has Taken Place
			switch (wParam)												// Evaluate Size Action
			{
				case SIZE_MINIMIZED:									// Was Window Minimized?
					window->isVisible = FALSE;							// Set isVisible To False
				return 0;												// Return

				case SIZE_MAXIMIZED:									// Was Window Maximized?
					window->isVisible = TRUE;							// Set isVisible To True
					pGLWIN->ResizeScene (LOWORD (lParam), HIWORD (lParam), 1.0f, 500.0f, 45.0f);		// Reshape Window - LoWord=Width, HiWord=Height
				return 0;												// Return

				case SIZE_RESTORED:										// Was Window Restored?
					window->isVisible = TRUE;							// Set isVisible To True
					pGLWIN->ResizeScene (LOWORD (lParam), HIWORD (lParam), 1.0f, 500.0f, 45.0f);		// Reshape Window - LoWord=Width, HiWord=Height
				return 0;											  	// Return
			}    
      break;
	}

	return DefWindowProc (hWnd, uMsg, wParam, lParam);		
}

//////////////////////////////////////////////////////////////////////////
/// Register a windows class
/// 
/// Input:   
///
/// Returns:  
//////////////////////////////////////////////////////////////////////////

BOOL RegisterWindowClass (COpenGLWindow::Application* application)					
{																	
	// Register A Window Class
   LOGBRUSH Brush;
   Brush.lbStyle = BS_SOLID;
   Brush.lbColor = 0x000000;
   Brush.lbHatch = 0;


	WNDCLASSEX windowClass;												         // Window Class
	ZeroMemory (&windowClass, sizeof (WNDCLASSEX));						   // Make Sure Memory Is Cleared
	windowClass.cbSize			= sizeof (WNDCLASSEX);					   // Size Of The windowClass Structure
	windowClass.style			   = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraws The Window For Any Movement / Resizing
	windowClass.lpfnWndProc		= (WNDPROC)(WindowProc);				   // WindowProc Handles Messages
	windowClass.hInstance		= application->hInstance;				   // Set The Instance
	windowClass.hbrBackground	= CreateBrushIndirect(&Brush);//(HBRUSH)(COLOR_APPWORKSPACE);			// Class Background Brush Color
	windowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	windowClass.lpszClassName	= application->className;			   	// Sets The Applications Classname

   if (RegisterClassEx (&windowClass) == 0)						
	{
		MessageBox (HWND_DESKTOP, "RegisterClassEx Failed!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;												
	}
	return TRUE;														
}

//////////////////////////////////////////////////////////////////////////
/// Entry point of every program
/// 
/// Input:   
///
/// Returns:  
//////////////////////////////////////////////////////////////////////////

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   COpenGLWindow::Application	application;				// Application Structure
	BOOL				isMessagePumpActive;						// Message Pump Active?
	MSG				msg;											// Window Message Structure

	// Fill Out Application Data
	application.className = "Sporkware";				   // Application Class Name
	application.hInstance = hInstance;						// Application Instance

   pGLWIN->m_ClientWidth =  GetSystemMetrics(SM_CXSCREEN);
   pGLWIN->m_ClientHeight =  GetSystemMetrics(SM_CYSCREEN);

   int width;
   int height;
   int bit;

   FILE * stream;
   stream = fopen("config.txt", "r");

   char FileBuffer = 0;		 // Main buffer to read characters into
	char FileBuffer2 = 0;	 // Temp buffer to store a character
   char StringBuffer[256];

   fscanf(stream, "%c", &FileBuffer);
   fclose(stream);
   if(FileBuffer != '/')
   {      
      FILE * stream;
      stream = fopen("config.txt", "w");

      fprintf(stream, "%s\n%s\n\n%s\t\t%s\n%s\t\t%s\n%s\t%s\n", "// Prototype config file", "//////////////////////////////////////////////////", "Width:", "640", "Height:", "480", "Bitdepth:", "32");
      fclose(stream);   
   }

   stream = fopen("config.txt", "r");

	while(fscanf(stream, "%c", &FileBuffer) != EOF)
	{
		if (FileBuffer == '/')
		{	
			FileBuffer2 = FileBuffer;
			fscanf(stream, "%c", &FileBuffer);

			if (FileBuffer == '/')
			{
				while (FileBuffer != '\n')
				{
					fscanf(stream, "%c", &FileBuffer);
				}
				continue;
			}
      }
		else 	
      {
         fscanf(stream, "%s", &StringBuffer);
      
         if(!(strcmp(StringBuffer, "Width:")))
         {
            fscanf(stream, "%s", &StringBuffer);
            width = atoi(StringBuffer);
            continue;
         }

         else if(!(strcmp(StringBuffer, "Height:")))
         {
            fscanf(stream, "%s", &StringBuffer);
            height = atoi(StringBuffer);
            continue;
         }

         else if(!(strcmp(StringBuffer, "Bitdepth:")))
         {
            fscanf(stream, "%s", &StringBuffer);
            bit = atoi(StringBuffer);
            continue;
         }
      }
	}

  
   switch(width)
   {
      case 640:
         g_GameOptions.SetVideoMode(RES640X480);
         break;
      case 800:
         g_GameOptions.SetVideoMode(RES800X600);
         break;
      case 1024:
         g_GameOptions.SetVideoMode(RES1024X768);
         break;
      case 1600:
         g_GameOptions.SetVideoMode(RES1600X1200);
         break;
   }

   switch(bit)
   {
      case 16:
         g_GameOptions.SetBitDepth(BD16);
         break;
      case 32:
         g_GameOptions.SetBitDepth(BD32);
         break;
   }

   fclose(stream);

	// Fill Out Window
	ZeroMemory (&pGLWIN->window, sizeof (COpenGLWindow::GLWindow));	// Make Sure Memory Is Zeroed
	pGLWIN->window.init.application		= &application;					// Window Application
	pGLWIN->window.init.title			   = "Protoype";			         // Window Title
	pGLWIN->window.init.width			   = width;								// Window Width
	pGLWIN->window.init.height			   = height;							// Window Height
	pGLWIN->window.init.bitsPerPixel	   = bit;								// Bits Per Pixel
	pGLWIN->window.init.isFullScreen	   = true;								// Fullscreen?
	
	// Register A Class For Our Window To Use
	if (RegisterWindowClass (&application) == FALSE)					// Did Registering A Class Fail?
	{
		// Failure
		MessageBox (HWND_DESKTOP, "Error Registering Window Class!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;														
	}

	g_ProgramRunning = TRUE;											// Program Looping Is Set To TRUE
	g_createFullScreen = pGLWIN->window.init.isFullScreen;	// g_createFullScreen Is Set To User Default
	
   while(g_ProgramRunning)											// Loop Until WM_QUIT Is Received
	{    
		// Create A Window
		pGLWIN->window.init.isFullScreen = g_createFullScreen;			// Set Init Param Of Window Creation To Fullscreen?
		
      if (pGLWIN->CreateWindowGL(&pGLWIN->window, pGLWIN->window.init.width, pGLWIN->window.init.height, 
                                  pGLWIN->window.init.bitsPerPixel, pGLWIN->window.init.isFullScreen) == TRUE)							// Was Window Creation Successful?
		{
			// At This Point We Should Have A Window That Is Setup To Render OpenGL
			if (pGLWIN->OpenGLInit() == FALSE)					// Call User Intialization
			{
				// Failure
				pGLWIN->TerminateApplication (&pGLWIN->window);							// Close Window, This Will Handle The Shutdown
			}
			else														// Otherwise (Start The Message Pump)
			{	// Initialize was a success
				isMessagePumpActive = TRUE;								// Set isMessagePumpActive To TRUE
				
            // Initialize COM
	         CoInitialize(NULL);

            // Seed the random time
            srand((unsigned int)time(0));

	         // Initialize Game Objects
	         pGLSTATE->Initialize();       
            pGLWIN->ClearScreen(); 
	         GameStateManager.Initialize();				    	
//            ShowCursor(false);
            pGLWIN->m_StartTime = float(timeGetTime() * 0.001);
            
            while (isMessagePumpActive == TRUE)						// While The Message Pump Is Active
				{
					// Success Creating Window.  Check For Window Messages
					if (PeekMessage (&msg, 0, 0, 0, PM_REMOVE) != 0)
					{
						// Check For WM_QUIT Message
						if (msg.message != WM_QUIT)						// Is The Message A WM_QUIT Message?
						{
                     if (GameStateManager.GetCurrentState() == (CGameStateManager*)(void*)&EditorMain)
                        TranslateMessage(&msg);
							
                     DispatchMessage (&msg);						// If Not, Dispatch The Message
						}
						else											// Otherwise (If Message Is WM_QUIT)
						{
							isMessagePumpActive = FALSE;				// Terminate The Message Pump
						}
					}
					else												// If There Are No Messages
					{
						if (pGLWIN->window.isVisible == FALSE)					// If Window Is Not Visible
						{
							WaitMessage ();								// Application Is Minimized Wait For A Message
						}
						else											// If Window Is Visible
                  {
                     // temp time
                     float temptime = float(timeGetTime() * 0.001);
                     
                     // Get the new gametime
                     g_time = temptime - pGLWIN->m_StartTime;
                     static float LastFPSUpdate = g_time;

                     g_FrameTime = temptime - g_last_time; //g_time - g_last_time;

                     // Increment the frames
                     g_frames++;

                     if (g_time > LastFPSUpdate + .25f)
                     {
			               g_fps = g_frames / (g_time - LastFPSUpdate);			               
			               
                        LastFPSUpdate = g_time;

                        g_frames    = 0;
                     }

                     g_last_time = temptime;
                                       
                     // Temp to see how many systems currently rendering
	                  //char buffer[20];
	                  //SetWindowText(pGLWIN->GetHWND(), itoa(g_fps, buffer, 10));
                     
                    	pGLWIN->ClearScreen(); 
				         GameStateManager.Main();                 
                     SwapBuffers(pGLWIN->window.hDC);				         	
						}
					}              
				}								
			}														

         
         FILE * stream;
         stream = fopen("config.txt", "w");

         char bufferwidth[20];
         int tempwidth = pGLWIN->GetScreenWidth();
         itoa(tempwidth, bufferwidth, 10);

         char bufferheight[20];
         int tempheight = pGLWIN->GetScreenHeight();
         itoa(tempheight, bufferheight, 10);

         char bufferbitdepth[20];
         int tempbitdepth = pGLWIN->GetScreenBitDepth();
         itoa(tempbitdepth, bufferbitdepth, 10);

         fprintf(stream, "%s\n%s\n\n%s\t\t%s\n%s\t\t%s\n%s\t%s\n", "// Prototype config file", "//////////////////////////////////////////////////", "Width:", bufferwidth, "Height:", bufferheight, "Bitdepth:", bufferbitdepth);
         fclose(stream);

         // Shutdown
//	      GameStateManager.Shutdown();
	      pGLWIN->DestroyWindowGL(&pGLWIN->window);				   
	      pGLWIN->DeleteInstance();
		}
		else															
		{
			// Error Creating Window
			MessageBox (HWND_DESKTOP, "Error Creating OpenGL Window", "Error", MB_OK | MB_ICONEXCLAMATION);
			g_ProgramRunning = FALSE;									
		}
	}																

	UnregisterClass (application.className, application.hInstance);		// UnRegister Window Class
	return 0;
}																	

//////////////////////////////////////////////////////////////////////////
/// Deletes an Instance of COpenGLWindow
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void COpenGLWindow::DeleteInstance(void)
{
	if (pInstance)
	{
		delete pInstance;
		pInstance = 0;
	}
}
 
//////////////////////////////////////////////////////////////////////////
/// Gets an Instance of COpenGLWindow Class
/// 
/// Input:     void
///
/// Returns:   COpenGLWindow * - Instance of the class
//////////////////////////////////////////////////////////////////////////

COpenGLWindow *COpenGLWindow::GetInstance(void)
{
   //  Check to see if one hasn’t been made yet.
	if (pInstance == 0)
		pInstance = new COpenGLWindow;

    //  Return the address of the instance.
	return pInstance;
}