/// \file GameOptions.h
/// Interface header file
//////////////////////////////////////////////////////////////////////////

#pragma once

// #includes here

/// An header for GameOptions class

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

enum { EASY_DIFFICULTY = 0, MEDIUM_DIFFICULTY, HARD_DIFFICULTY };
enum { RES640X480 = 0, RES800X600, RES1024X768, RES1600X1200};
enum { BD16 = 0, BD32};


class CGameOptions
{
   public:
      CGameOptions();
      ~CGameOptions();
      void Init();
      const GameControl GetActionMap();

      void SetVideoMode(int Mode);
      void SetBitDepth(int Depth);
      void SetDifficulty(int Value);
      int GetVideoMode();
      int GetVideoWidth();
      int GetVideoHeight();
      int GetBitDepth();
      void  FXVolUp();
      void  FXVolDown();
      float   GetFXVol();
      void  MusicVolUp();
      void  MusicVolDown();
      float GetMusicVol();
      void  SetMusicVol(float vol);
      void  SetFXVol(float vol);
      void  InitSoundValues();
      void  AIOff();
      void  AIOn();
      bool  GetAI();
      int   GetDifficulty();

      friend void OnClickMute(CGameOptions* pOptions);
      friend void OnClickFXUP(CGameOptions* pOptions);
      friend void OnClickMusicUP(CGameOptions* pOptions);
      friend void OnClickFXDN(CGameOptions* pOptions);
      friend void OnClickMusicDN(CGameOptions* pOptions);
      friend void OnClickDifficultyEasy(CGameOptions* pOptions);
      friend void OnClickDifficultyMedium(CGameOptions* pOptions);
      friend void OnClickDifficultyHard(CGameOptions* pOptions);
      friend void OnClickActionMap(CGameOptions* pOptions);
      friend void OnClickBitDepth16(CGameOptions* pOptions);
      friend void OnClickBitDepth32(CGameOptions* pOptions);
      friend void OnClickScreenRez1024x768(CGameOptions* pOptions);
      friend void OnClickScreenRez1600x1200(CGameOptions* pOptions);
      friend void OnClickScreenRez800x600(CGameOptions* pOptions);
      friend void OnClickScreenRez640x480(CGameOptions* pOptions);
      friend void OnClickStartGame(CGameOptions* pOptions);
      friend void OnClickOptionsMenu(CGameOptions* pOptions);
      friend void OnClickQuitGame(CGameOptions* pOptions);

   protected:
   
   private:
      bool        m_AI;
      bool        m_Mute;
      float       m_FXVol;
      float       m_MusicVol;
      int         m_ScreenRez;
      int         m_BitDepth;
      int         m_Difficulty;
      GameControl m_ActionMap;
};
