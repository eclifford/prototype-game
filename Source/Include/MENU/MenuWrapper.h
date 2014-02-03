/// \file MenuWrapper.h
/// InterfaceObject header file
//////////////////////////////////////////////////////////////////////////

#pragma once

// #includes here
#include "math3d\math3d.h"
#include "graphics\texturefonts.h"

// forward declarations
class CAudio;
class CGameInterfaceManager;
class CInputManager;
class CBitmapFont;

// globals here

/// An header for menu interface objects

enum { MENU_MAIN = 0, MENU_LEVEL, /*MENU_LOAD,*/ MENU_EDITOR, MENU_VIDEO, MENU_CREDITS, MENU_QUIT, MENU_SOUND, MENU_CONTROLS };

// main menu items
enum { START_GAME = 0, EDITOR, CONFIGURE_OPTIONS, CREDITS, QUIT_GAME };

// difficulty menu
enum { DIFF_EASY = 0, DIFF_MEDIUM, DIFF_HARD, DIFF_START, DIFF_BACK };

// editor
enum { EDSTART, EDBACK };

// Credits
enum { CREDBACK };

// quit menu items
enum { QUIT_YES = 0, QUIT_NO };

// Video menu items
enum { MODE640X480 = 0, MODE800X600, MODE1024X768, MODE1600X1200, V16B, V32B, VSAVE, VDEFAULT, VVIDEO, VSOUND, VCONTROL, VBACK };

// Sound menu items
enum { FVOL1, MVOL1, /*MVOL2, MVOL3, MVOL4, MVOL5, MVOL6, MVOL7, MVOL8, MVOL9, MVOL10,*/
       FPTR, MPTR, /*FVOL2, FVOL3, FVOL4, FVOL5, FVOL6, FVOL7, FVOL8, FVOL9, FVOL10,*/
       SSAVE, SDEFAULT, SVIDEO, SSOUND, SCONTROL, SBACK };

// Controls menu items
enum {   CVIDEO, CSOUND, CCONTROL, CBACK };

enum { LEVEL_START = 0, LEVEL_BACK, LEVEL_ONE, LEVEL_TWO, LEVEL_THREE, LEVEL_SKIRMISH };

enum { BRIEF_START = 0, BRIEF_BACK, BRIEF_EASY, BRIEF_MEDIUM, BRIEF_HARD,  };

enum { LOST_START, LOST_BACK };

enum { VICTORY_START, VICTORY_BACK };

enum { MAIN_MENU = 0, BRIEFING, LEVEL_VICTORY, LEVEL_LOST };

enum { BRIEFING_MENU, LOAD_MENU };

class CMenuWrapper
{
   public:
      static CMenuWrapper* GetInstance();
      ~CMenuWrapper();
      void Init();
      unsigned int Update();
      void Render();
      void GetInput();
      void Shutdown();
      void SetState(int State);
      CAudio * m_ButtonClick;
      CAudio * m_RadioClick;
      CAudio * m_ButtonSet;
    void DeleteInstance();

   protected:

   private:
      CMenuWrapper();
		static CMenuWrapper* m_pInstance; // Pointer to instance of singleton class
		CMenuWrapper(const &CMenuWrapper) {};
		CMenuWrapper &operator = (const &CMenuWrapper) {};
      void Briefing1Init();
      void Briefing2Init();
      void Briefing3Init();
      void VictoryInit();
      void LostInit();
      void MenuInit();
      unsigned int MenuUpdate();
      unsigned int Briefing1Update();
      unsigned int Briefing2Update();
      unsigned int Briefing3Update();
      unsigned int VictoryUpdate();
      unsigned int LostUpdate();
      void  MenuRender();
      void  Briefing1Render();
      void  Briefing2Render();
      void  Briefing3Render();
      void  VictoryRender();
      void  LostRender();
      void  MenuGetInput();
      void  Briefing1GetInput();
      void  Briefing2GetInput();
      void  Briefing3GetInput();
      void  VictoryGetInput();
      void  LostGetInput();
      void  RefreshItems(int FirstItem, int LastItem, int ExcludeItem);

      CGameInterfaceManager*  pIM;
      CInputManager*          m_pInput;
      int                     m_CurrentMenu;
      CTextureFonts           m_TextToScreen;
      int                     m_WrapperState;
      CVector3f               m_MousePos;
      CBitmapFont * g_pFont;
      CBitmapFont * g_pSmallFont;
      CBitmapFont * g_pLargeFont;
      int                     m_FXVolPointer;
      int                     m_MusicVolPointer;
      int                     m_FXVolBar;
      int                     m_MusicVolBar;
      CVector3f               m_MusicBarStartPos;
      CVector3f               m_FXBarStartPos;
};
