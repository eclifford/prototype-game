/// \file menumain.h
/// The inmenu loop
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "core\gamestatemanager.h"

// forward declarations
class CAudio;

class CMenuMain : CGameStateManager 
{
	public:
		CMenuMain() {m_Initialized = false; m_CurrentMenuState = MENU_RUNNING;};
		~CMenuMain() {};

      enum MenuState{MENU_RUNNING = 0, ENTER_GAME, ENTER_EDITOR, GAME_EXIT};

      unsigned int m_CurrentMenuState;
      CAudio * m_pTitleBG;

		virtual void Initialize();
		virtual void GetInput();
		virtual unsigned int Main();
		virtual void Render();
		virtual unsigned int Update();
		virtual void Shutdown();
};