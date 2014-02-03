/// \file startmain.h
/// The entry game state
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "core\gamestatemanager.h"

// forward declaration
class CDirectShow;

class CStartMain : CGameStateManager 
{
	public:
		CStartMain() {m_pDirectShow = 0; m_Initialized = false; m_StartMainState = INTRO_RUNNING;};
		~CStartMain();

      enum StartState{INTRO_RUNNING = 0, ENTER_MENU};

      unsigned int m_StartMainState;

		virtual void Initialize();
		virtual void GetInput();
		virtual unsigned int Main();
		virtual void Render();
		virtual unsigned int Update();
		virtual void Shutdown();

      CDirectShow* m_pDirectShow;
      int MovieIndex;
};
