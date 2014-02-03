#include <memory>



#include "gamelogic\GameOptions.h"
#include "utils\mmgr.h"

using namespace std;

CGameOptions::CGameOptions()
{
   Init();
}


CGameOptions::~CGameOptions()
{
}


void CGameOptions::Init()     
{
      bool        m_Mute;
      int         m_FXVol;
      int         m_MusicVol;
      int         m_ScreenRez;
      int         m_BitDepth;
      int         m_Difficulty;

      m_Mute         = false;
      m_FXVol        = 0;
      m_MusicVol     = 0;
      m_ScreenRez    = 0;
      m_BitDepth     = 0;
      m_Difficulty   = 0;
      m_AI           = true;

      memset(&m_ActionMap, 0, sizeof(GameControl));
}


const GameControl CGameOptions::GetActionMap()
{
   return (const GameControl) m_ActionMap;
}


void CGameOptions::SetVideoMode(int Mode)
{
   m_ScreenRez = Mode;
}

void CGameOptions::SetBitDepth(int Depth)
{
   m_BitDepth = Depth;
}

int CGameOptions::GetVideoMode()
{
   return m_ScreenRez;
}


void  CGameOptions::FXVolUp()
{
   m_FXVol ++;

   if (m_FXVol > 1.0f)
      m_FXVol = 1.0f;
}

void  CGameOptions::AIOn()
{
   m_AI = true;
}

void CGameOptions::AIOff()
{
   m_AI = false;
}

bool CGameOptions::GetAI()
{
   return m_AI;
}

void  CGameOptions::FXVolDown()
{
   m_FXVol -= .1f;

   if (m_FXVol < 0.0f)
      m_FXVol = 0.0f;
}

float   CGameOptions::GetFXVol()
{
   return m_FXVol;
}

void  CGameOptions::MusicVolUp()
{
   m_MusicVol += .1f;

   if (m_MusicVol > 1.0f)
      m_MusicVol = 1.0f;
}

void  CGameOptions::MusicVolDown()
{
   m_MusicVol --;
   if (m_MusicVol < 0)
      m_MusicVol = 0;
}

float   CGameOptions::GetMusicVol()
{
   return m_MusicVol;
}

int CGameOptions::GetDifficulty()
{
   return m_Difficulty;
}

void CGameOptions::SetDifficulty(int Value)
{
   m_Difficulty = Value;
}

void CGameOptions::InitSoundValues()
{
   m_MusicVol = 0.5f;
   m_FXVol = 0.5f;
}

int CGameOptions::GetVideoHeight()
{
   int Height = 0;

   switch(m_ScreenRez) 
   {
      case RES640X480:
         Height = 480;
   	   break;

      case RES800X600:
   	   Height = 600;
         break;

      case RES1024X768:
         Height = 768;
         break;
      case RES1600X1200:
         Height = 1200;
         break;
      default:
         break;
   };

   return Height;
}

int CGameOptions::GetBitDepth()
{
   int BitDepth = 0;

   switch(m_BitDepth)
   {
      case BD16:
         BitDepth = 16;
         break;
      case BD32:
         BitDepth = 32;
         break;
   };

   return BitDepth;
}

int CGameOptions::GetVideoWidth()
{
   int Width = 0;

   switch(m_ScreenRez) 
   {
      case RES640X480:
         Width = 640;
   	   break;

      case RES800X600:
   	   Width = 800;
         break;

      case RES1024X768:
         Width = 1024;
         break;
      case RES1600X1200:
         Width = 1600;
         break;
      default:
         break;
   };

   return Width;
}

void  CGameOptions::SetMusicVol(float vol)
{
   m_MusicVol = vol;
}

void  CGameOptions::SetFXVol(float vol)
{
   m_FXVol = vol;
}


// Friend Functions //////////////////////////////////////////////////////////////////////////


void OnClickMute(CGameOptions* pOptions)
{
   pOptions->m_Mute = !(pOptions->m_Mute);
}


void OnClickFXUP(CGameOptions* pOptions)
{
   pOptions->FXVolUp();
}


void OnClickMusicUP(CGameOptions* pOptions)
{
   pOptions->MusicVolUp();
}


void OnClickFXDN(CGameOptions* pOptions)
{
   pOptions->FXVolDown();
}


void OnClickMusicDN(CGameOptions* pOptions)
{
   pOptions->MusicVolDown();
}


void OnClickDifficultyEasy(CGameOptions* pOptions)
{
   pOptions->m_Difficulty = EASY_DIFFICULTY;
}



void OnClickDifficultyMedium(CGameOptions* pOptions)
{
   pOptions->m_Difficulty = MEDIUM_DIFFICULTY;
}



void OnClickDifficultyHard(CGameOptions* pOptions)
{
   pOptions->m_Difficulty = HARD_DIFFICULTY;
}


/*
void OnClickActionMap(CGameOptions* pOptions)
{

}
*/


void OnClickBitDepth16(CGameOptions* pOptions)
{
   pOptions->m_BitDepth = ~pOptions->m_BitDepth;
}

void OnClickBitDepth32(CGameOptions* pOptions)
{
   pOptions->m_BitDepth = ~pOptions->m_BitDepth;
}

void OnClickScreenRez640x480(CGameOptions* pOptions)
{
   pOptions->m_ScreenRez = RES640X480;
}

void OnClickScreenRez1024x768(CGameOptions* pOptions)
{
   pOptions->m_ScreenRez = RES1024X768;
}

void OnClickScreenRez1600x1200(CGameOptions* pOptions)
{
   pOptions->m_ScreenRez = RES1600X1200;
}

void OnClickScreenRez800x600(CGameOptions* pOptions)
{
   pOptions->m_ScreenRez = RES800X600;
}

void OnClickStartGame(CGameOptions* pOptions)
{
   
}

void OnClickOptionsMenu(CGameOptions* pOptions)
{

}

void OnClickQuitGame(CGameOptions* pOptions)
{

}

void OnClickUpControl(CGameOptions* pOptions)
{
   // get some keyboard input ... maybe a keyboard class or something?
}

void OnClickUpDownControl(CGameOptions* pOptions)
{
}


void OnClickUpStrafeRightControl(CGameOptions* pOptions)
{
}


void OnClickUpStrafeLeftControl(CGameOptions* pOptions)
{
}


void OnClickUpFireControl(CGameOptions* pOptions)
{
}


void OnClickUpAltFireControl(CGameOptions* pOptions)
{
}


void OnClickUpJumpJetControl(CGameOptions* pOptions)
{
}


void OnClickUpForwardControl(CGameOptions* pOptions)
{
}


void OnClickUpBackControl(CGameOptions* pOptions)
{
}


void OnClickUpTurnRightControl(CGameOptions* pOptions)
{
}


void OnClickUpTurnLeftControl(CGameOptions* pOptions)
{
}


void OnClickUpPauseControl(CGameOptions* pOptions)
{
}




/*

struct GameControl
{
   int   Up,
         Down,
         StrafeRight,
         StrafeLeft,
         Fire,
         AltFire,
         JumpJet,
         Forward,
         Back,
         TurnRight,
         TurnLeft,
         Pause;
};


enum { START_GAME = 0, CONFIGURE_OPTIONS, QUIT_GAME };

// Options menu items
enum { SOUND_OPTIONS = 0, VIDEO_OPTIONS, CONTROLS_OPTIONS, CONF_BACKTOMAIN };

// quit menu items
enum { QUIT_YES = 0, QUIT_NO };

// Video menu items
enum { MODE640X480 = 0, MODE800X600, VID_CONF_BACK };

// Sound menu items
enum { FX_VOL0 = 0, FX_VOL1, FX_VOL2, MUSIC_VOL0, MUSIC_VOL1, MUSIC_VOL2, SOUND_CONF_BACK };

// Controls menu items
enum {   CONF_INVERTMOUSE = 0, CONF_UP, CONF_DOWN, CONF_FOWARD, CONF_BACK, CONF_STRAFE_R,
         CONF_STRAFE_L, CONF_FIRE, CONF_ALT_FIRE, CONTROL_CONF_BACK };
*/