/// \file audiomanager.h
/// Manager for components of FMOD 
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "math3d/math3d.h"
#include "sound\openal.h"
#include "sound\fmod.h"
#include <al\alc.h>

enum SOUNDFX
{
   SFX_MENU_BG = 0,
   SFX_LEVEL1_BG,
   SFX_LEVEL2_BG,
   SFX_LEVEL3_BG,
   SFX_PLAYER_TURRET,
   SFX_PLAYER_ENGINE,
   SFX_PLAYER_JUMPJET,
   SFX_PLAYER_SHIELDABSORB,
   SFX_PLAYER_CHARGE,
   SFX_PLAYER_CHARGE2,
   SFX_PLAYER_HULLHIT,
   SFX_PLAYER_IONCANNON,
   SFX_MISSILEFIRED,
   SFX_MISSILEINAIR,
   SFX_PLASMAINAIR,
   SFX_EXP_TINY,
   SFX_EXP_SMALL,
   SFX_EXP_MED,
   SFX_EXP_LARGE,
   SFX_EXP_HUGE,
   SFX_ENEMY_ENGINE,
   SFX_FIRE,
   SFX_TREEFALL,
   SFX_OBJCOLLIDE,
   SFX_SHLDPWRUP,
   SFX_AMMOPWRUP,
   SFX_WPNUPGRADE,
   SFX_WPNCHANGE,
   SFX_LOCKON,
   SFX_LASERSHOT, 
   SFX_MOTHERSHIP,
   SFX_CONVOYENGINE,
   SFX_IONFLARE,
   SFX_TITLEBG,
   SFX_BUTTONCLK,
   SFX_RADIOCLK,
   SFX_BUTTONSET,
   SFX_CHARGE,
   SFX_CHARGE2,
   SFX_LASERAMB,
   SFX_LASERHIT, 
   SFX_ENEMY_TURRET,
   SFX_ENEMYMISSILEFIRED,
   SFX_FRIENDLY_TURRET,
   SFX_BUILDINGCOLLAPSE1,
   SFX_BUILDINGCOLLAPSE2,
   SFX_INCOMINGMSG,
   SFX_IONWAVE,
   SFX_FIGHTER,
   SFX_DROPBOMB,
   SFX_LVL1SEQUENCELOOP,

   NUM_SOUNDS
};

struct SSound
{
    void Clear()
    {
        m_Buffer = 0;
        m_MaxPlaybacks = 0;
        m_CurrentPlaybacks = 0;
        m_MinDistance = 0;
        m_MaxDistance = 0;
        m_Is3D = 0;
        m_Loop = 0;
        m_Loaded = 0;
    }

    unsigned int m_Buffer;
    unsigned int m_MaxPlaybacks;
    unsigned int m_CurrentPlaybacks;
    float m_MinDistance;
    float m_MaxDistance;
    bool m_Is3D;
    bool m_Loop;
    bool m_Loaded;
    
    void SetMinMaxDistance(float Min, float Max);
};


class CAudio;
class CAudioManager
{
   private:
      // friend class CAudio;
       

       // source lists
        COpenALSource* m_pSourcesPlaying; // playing and paused sources
        //COpenALSource* m_pSourcesQueued; // sources waiting to be played        
        //COpenALSource* m_pSourcesStopped; // stoppped sources
        COpenALSource* m_pSourcePool; // pool of unused sources

        COpenALSource* m_pAllSourcesArray; // array of all sources

        unsigned int m_MaxSourcesPlaying; // max number of sources available to be played
        unsigned int m_NumSourcesPlaying;

        float m_RolloffFactor;

      CAudio* m_pAudioObjects; // audio objects to clean up during shutdown

      void AddToSourcePool(COpenALSource* pSource);
      COpenALSource* GetSourceFromPool();
      void RemoveSourceFromPlaying(COpenALSource* pSource);

      void StopSource(COpenALSource* pSource);
      void PlaySource(COpenALSource* pSource);
      SSound m_aSounds[NUM_SOUNDS];
      CVector3f m_ListenerLastPosition;

      ALCcontext* m_pOpenALContext;
      ALCdevice* m_pOpenALDevice;

   public:
      CAudioManager() { }
      ~CAudioManager() { }
      

      void PlayAudio(CAudio* pAudio);
      void StopAudio(CAudio* pAudio);

      unsigned int samples[NUM_SOUNDS];

      
      unsigned int m_NextFreeSound;

	   float m_SFXVolumeRatio;
	   float m_BGVolumeRatio;

      void Initialize();
      void LoadGameSounds();
      void LoadMenuSounds();
      CAudio* CreateSound(unsigned int sound, unsigned int priority, bool IsPaused = true);
      unsigned int LoadSound(const char * fn, bool is3D, bool loop, unsigned int maxplaybacks);
	   void SetAudioRatios(float sfxratio, float bgratio);
      //void SetGlobalVolume(int volume);
      //void SetProperties();
      void Shutdown();
      void Update();

      void UpdateSources();
};

