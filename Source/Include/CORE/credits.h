/// \file startmain.h
/// The entry game state
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "core\gamestatemanager.h"
#include "input\inputmanager.h"
#include "graphics\directshow.h"

class CCreditsMain : CGameStateManager 
{
	public:
		CCreditsMain() {};
		~CCreditsMain() {};      

		virtual void Initialize();
		virtual void GetInput();
		virtual unsigned int Main();
		virtual void Render();
		virtual unsigned int Update();
		virtual void Shutdown();

   private:
      bool m_Done;   

};
