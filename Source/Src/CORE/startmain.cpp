/// \file startmain.cpp
/// Intro game state
//////////////////////////////////////////////////////////////////////////

#include "core\startmain.h"
#include "core\oglstatemanager.h"
#include "graphics\texturemanager.h"
#include "input\inputmanager.h"
#include "graphics\interfacerenderable.h"
#include "gamelogic\gameoptions.h"
#include "graphics\directshow.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ Global Instances          /*/ 
/////////////////////////////////

extern COpenGLStateManager *pGLSTATE;
extern CTextureManager g_TextureManager;
extern CGameOptions g_GameOptions;
extern COpenGLWindow* pGLWIN;
//CInterfaceRenderable test;

CStartMain::~CStartMain()
{
    delete m_pDirectShow;
    m_pDirectShow = 0;
}

//////////////////////////////////////////////////////////////////////////
/// Initialize
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CStartMain::Initialize()
{
    m_pDirectShow = new CDirectShow;
   
   m_StartMainState = INTRO_RUNNING;
	
   /*
   g_TextureManager.Initialize(10);

   pGLSTATE->SetState(LIGHTING, false);
   
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   pGLSTATE->SetState(TEXTURE_2D, true);
   glMatrixMode(GL_PROJECTION);

   glPushMatrix();
   glLoadIdentity();

   pGLSTATE->SetState(DEPTH_TEST, false);

   gluOrtho2D(-1, 1, -1, 1);
   glMatrixMode(GL_MODELVIEW);

   FloatRect OrthoRect;
   OrthoRect.Max.x = 1.0f;
   OrthoRect.Max.y = 1.0f;
   OrthoRect.Min.x = -1.0f;
   OrthoRect.Min.y = -1.0f;
  
   switch(g_GameOptions.GetVideoMode())
   {
      case RES1600X1200:
      {
         int width = 1600;
         int height = 1200;
         Point3f Position(0.0f, 0.0f, 0.0f);
         test.Initialize("resources\\images\\sporkware.jpg", &OrthoRect, &Position, width,
                                    height);
      }
      break;

      case RES1024X768:
      {
         int width = 1024;
         int height = 768;
         Point3f Position(0.0f, 0.0f, 0.0f);
         test.Initialize("resources\\images\\sporkware1024.jpg", &OrthoRect, &Position, width,
                                    height);
      }
      break;

      case RES800X600:
      {
         int width = 800;
         int height = 600;
         Point3f Position(0.0f, 0.0f, 0.0f);
         test.Initialize("resources\\images\\sporkware800.jpg", &OrthoRect, &Position, width,
                                    height);
      }
      break;

      case RES640X480:
      {
         int width = 640;
         int height = 480;
         Point3f Position(0.0f, 0.0f, 0.0f);
         test.Initialize("resources\\images\\sporkware640.jpg", &OrthoRect, &Position, width,
                                    height);
      }
      break;
   }
   */

  // CInputManager::GetInstance()->SetTimerInput(.2f);
   //pGLWIN->ChangeScreenResolution(640, 480, 32);
   m_pDirectShow->Init(pGLWIN->GetHWND(), 1);
	
   m_pDirectShow->Load("resources\\movies\\sporkware1.avi", &MovieIndex);

   RECT ScreenRect = {-3, -20, pGLWIN->GetScreenWidth() + 3, pGLWIN->GetScreenHeight() + 10};
   
   m_pDirectShow->Play(MovieIndex, &ScreenRect);
	
   glClear(GL_COLOR_BUFFER_BIT);
}

//////////////////////////////////////////////////////////////////////////
/// Main loop for our game
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CStartMain::Main()
{
   switch(m_StartMainState)
   {
      case INTRO_RUNNING:
      {
         GetInput();
         Update();
         Render();
         break;
      }

      case ENTER_MENU:
      {
         return MENU;
         break;
      }
   }

   return NO_CHANGE;
}

//////////////////////////////////////////////////////////////////////////
/// Get Input from our Input Manager
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CStartMain::GetInput()
{
   CInputManager::GetInstance()->PollKeyboard();

   if (CInputManager::GetInstance()->GetKey(DIK_ESCAPE) ||
       CInputManager::GetInstance()->GetKey(DIK_RETURN))
       m_StartMainState = ENTER_MENU;
}

//////////////////////////////////////////////////////////////////////////
/// Render all objects in our world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CStartMain::Render()
{
   /*
   if (m_StartMainState == ENTER_MENU)
   {
      
   }*/   
}

//////////////////////////////////////////////////////////////////////////
/// Update all objects in our world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CStartMain::Update()
{
   m_pDirectShow->HandleEvent();
   
   if (!m_pDirectShow->IsPlaying(MovieIndex))
      m_StartMainState = ENTER_MENU;      

   return 0;
}

//////////////////////////////////////////////////////////////////////////
/// Lets clean up
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CStartMain::Shutdown()
{
   /*
   glClear(GL_COLOR_BUFFER_BIT);
   test.Render();
   glFlush();
   */
   //CInputManager::GetInstance()->SetTimerInput(0.0f);
   glMatrixMode(GL_PROJECTION);
   
   pGLSTATE->SetState(LIGHTING, true);

	//g_TextureManager.UnloadAll();

   m_pDirectShow->DestroyObjects();
}
