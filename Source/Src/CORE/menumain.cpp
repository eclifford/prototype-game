/// \file menumain.cpp
/// Menu Loop 
//////////////////////////////////////////////////////////////////////////

#include "core\menumain.h"
#include "core\oglstatemanager.h"
#include "graphics\texturemanager.h"
#include "input\inputmanager.h"
#include "menu\menuwrapper.h"
#include "graphics\interfacerenderable.h"
#include "core\gamemain.h"
#include "sound\audiomanager.h"
#include "sound\audio.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ Global Instances          /*/ 
/////////////////////////////////

extern COpenGLStateManager *pGLSTATE;
extern CTextureManager g_TextureManager;
extern CAudioManager g_AudioManager;
extern CGameMain GameMain;

//////////////////////////////////////////////////////////////////////////
/// Initialize
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMenuMain::Initialize()
{
   m_CurrentMenuState = MENU_RUNNING;
	g_TextureManager.Initialize(100);

   g_AudioManager.Initialize();
   g_AudioManager.LoadMenuSounds();

   pGLSTATE->SetState(LIGHTING, false);

   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   
   pGLSTATE->SetState(TEXTURE_2D, true);

   glMatrixMode(GL_PROJECTION);

   glPushMatrix();
   glLoadIdentity();

   pGLSTATE->SetState(DEPTH_TEST, false);

   gluOrtho2D(-1, 1, -1, 1);
   glMatrixMode(GL_MODELVIEW);

   CInputManager::GetInstance()->SetTimerInput(.2f);
   CMenuWrapper::GetInstance()->Init();

   m_pTitleBG = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_TITLEBG], 255);
   m_pTitleBG->PlayBackgroundLoop(100);
}

//////////////////////////////////////////////////////////////////////////
/// Main loop for our game
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CMenuMain::Main()
{
   switch(m_CurrentMenuState)
   {
      case MENU_RUNNING:
      {
         GetInput();
         Update();
         Render();
         break;
      }

      case ENTER_GAME:
      {
         return GAME;
         break;
      }
      
      case ENTER_EDITOR:
      {
         return EDITOR;
         break;
      }

      case GAME_EXIT:
      {
         return EXIT_PROGRAM;
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

void CMenuMain::GetInput()
{
   CMenuWrapper::GetInstance()->GetInput();
}

//////////////////////////////////////////////////////////////////////////
/// Render all objects in our world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMenuMain::Render()
{
   CMenuWrapper::GetInstance()->Render();
}

//////////////////////////////////////////////////////////////////////////
/// Update all objects in our world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CMenuMain::Update()
{
   unsigned int State = CMenuWrapper::GetInstance()->Update();
   
   FSOUND_Update(); 
   
   switch(State)
   {
      case 0:
      {
         m_CurrentMenuState = MENU_RUNNING;  
         break;
      }
      case 1:
      {
         m_CurrentMenuState = ENTER_GAME;  
         break;
      }
      case 2:
      {
         m_CurrentMenuState = ENTER_EDITOR;  
         break;
      }
      case 255:
      {
         m_CurrentMenuState = GAME_EXIT;  
         break;
      }
   }

   return 0;
}

//////////////////////////////////////////////////////////////////////////
/// Lets clean up
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CMenuMain::Shutdown()
{
   CMenuWrapper::GetInstance()->Shutdown();
   CInputManager::GetInstance()->SetTimerInput(0.0f);
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   
   pGLSTATE->SetState(LIGHTING, true);

   g_TextureManager.UnloadAll();
   g_AudioManager.Shutdown();

   glMatrixMode(GL_MODELVIEW);
}
