/// \file gamestatemanager.h
/// Base class for the games states
//////////////////////////////////////////////////////////////////////////

#pragma once //	Include this file only once

#include "core\oglwindow.h"

class CGameStateManager
{
	private:
		CGameStateManager * m_pCurrentState;	// Current state of program

	public:
		CGameStateManager() {};
		~CGameStateManager() {};

		bool m_Initialized;
      unsigned int State;

		virtual void Initialize();
		virtual void GetInput();
		virtual unsigned int Main();
		virtual void Render();
		virtual void Shutdown();
		virtual unsigned int Update();

		enum States{NO_CHANGE = 0, MENU, GAME, EDITOR, WON, LOST, EXIT_PROGRAM};

		bool SwitchStates(CGameStateManager *stateToChangeTo);

		inline SetCurrentState(CGameStateManager *state)
		{
			m_pCurrentState = state;
		}

		inline CGameStateManager * GetCurrentState(void)
		{
			return m_pCurrentState;
		}
};