/// \file audiosource.h
/// A audio source
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "sound\openal.h"
#include "sound\audiomanager.h"

#define MAX_DISTANCE 250000

class CAudio
{
    friend class CAudioManager;             
   public:
      COpenALSource* m_Channel;
      
      void UpdateSound(CVector3f pos, CVector3f vel);
      void Play3DAudio(float min, float max, unsigned int volume);
      void Play2DAudio(unsigned int volume);
	  void PlayBackgroundLoop(unsigned int volume);

      unsigned int GetVolume();

      void StopAudio();
      void AdjustVolume(unsigned int volume);
      
    private:
      unsigned int m_Priority;
      CAudio* m_pNext;
      unsigned int m_Sound;

};






















