/// \file gamestatemanager.cpp
/// Manages all game states
//////////////////////////////////////////////////////////////////////////

#include "core\gamestatemanager.h"
#include "core\oglstatemanager.h"
#include "graphics\texturemanager.h"
#include "core\gamemain.h"
#include "core\menumain.h"
#include "core\startmain.h"
#include "level editor\editormain.h"
#include "input\inputmanager.h"
#include "core\oglwindow.h"
#include "menu\menuwrapper.h"
#include "graphics\fader.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ Global Instances          /*/ 
/////////////////////////////////

CGameMain GameMain;
CMenuMain MenuMain;
CStartMain StartMain;
CEditorMain EditorMain;
CFader g_Fader;

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern COpenGLStateManager *pGLSTATE;
extern COpenGLWindow		*pGLWIN;


//////////////////////////////////////////////////////////////////////////
/// Initialize
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameStateManager::Initialize()
{
   // initialize direct input
   CInputManager::GetInstance()->Init();

   // Set the cursor to false
   ShowCursor(false);

	// Set current state to menumain
	SwitchStates((CGameStateManager*)&StartMain);

	g_Fader.Initialize();
}

//////////////////////////////////////////////////////////////////////////
/// Main loop for our game
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CGameStateManager::Main()
{
   // Get the current state of the program
	State = m_pCurrentState->Main();

   // Get input for the gamestatemanager 
   GetInput();
 
   switch(State)
   {
      case NO_CHANGE:
      {

      }
      break;

      case GAME:
      {
         SwitchStates((CGameStateManager*)&GameMain);  
         return 1;
      }
      break;

      case MENU:
      {
         SwitchStates((CGameStateManager*)&MenuMain);  
         return 2;
      }
      break;

      case EDITOR:
      {
         pGLWIN->DestroyWindowGL(&pGLWIN->window);
         pGLWIN->CreateWindowGL(&pGLWIN->window, pGLWIN->m_ClientWidth, pGLWIN->m_ClientHeight, 32, false); 
         pGLSTATE->Initialize();
         pGLWIN->OpenGLInit();
         SwitchStates((CGameStateManager*)&EditorMain);
         return 3;
      }
      break;

      case WON:
      {
         CMenuWrapper::GetInstance()->SetState(LEVEL_VICTORY);
         SwitchStates((CGameStateManager*)&MenuMain);
         return 4;
      }
      break;
    
      case LOST:
      {
         CMenuWrapper::GetInstance()->SetState(LEVEL_LOST);
         SwitchStates((CGameStateManager*)&MenuMain);
         return 5;
      }
      break;

      case EXIT_PROGRAM:
      {
         m_pCurrentState->Shutdown();
         pGLSTATE->DeleteInstance();
         CMenuWrapper::GetInstance()->DeleteInstance();
         Shutdown();      
		   pGLWIN->TerminateApplication(&pGLWIN->window);
         return 255;
      }
      break;
   }
   return 0;
}

//////////////////////////////////////////////////////////////////////////
/// Get Input from our Input Manager
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameStateManager::GetInput()
{
   /*
	if (GetAsyncKeyState(VK_F1))
	{
		GameMain.m_CurrentLevel = GameMain.TestLevel;
	}

	else if (GetAsyncKeyState(VK_F2))
	{
		SwitchStates(&GameMain);
	}

   else if (GetAsyncKeyState(VK_F3))
   {
      SwitchStates(&EditorMain);
   }

	else if (GetAsyncKeyState(0x38))
	{
		GameMain.m_CurrentLevel = GameMain.Level1;
	}

   else if (GetAsyncKeyState(0x39))
   {
      GameMain.m_CurrentLevel = GameMain.Level2;
   }
   else if (GetAsyncKeyState(0x30))
   {
      GameMain.m_CurrentLevel = GameMain.Level3;
   }*/
}

//////////////////////////////////////////////////////////////////////////
/// Render all objects in our world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameStateManager::Render()
{
	m_pCurrentState->Render();
}

//////////////////////////////////////////////////////////////////////////
/// Update all objects in our world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CGameStateManager::Update()
{	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
/// Lets clean up
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameStateManager::Shutdown()
{
   // shutdown direct input
   CInputManager::GetInstance()->Shutdown();
}

//////////////////////////////////////////////////////////////////////////
/// Switch game states
/// 
/// Input:     CGameStateManager *stateToChangeTo
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

bool CGameStateManager::SwitchStates(CGameStateManager *stateToChangeTo)
{
	// We are already in this state
	if(stateToChangeTo == m_pCurrentState)
		return false;

	// Shut down the old state
	if(m_pCurrentState)
		m_pCurrentState->Shutdown();

	// Initialize the new state
	stateToChangeTo->Initialize();
	
	// Set the current state
	SetCurrentState(stateToChangeTo);

	// Return true
	return true;
}
