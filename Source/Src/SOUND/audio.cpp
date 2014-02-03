///\file audio.cpp
/// An Audio File
//////////////////////////////////////////////////////////////////////////

#include <cassert>

#include "utils\mmgr.h"

#include "sound\audio.h"
#include "camera\camera.h"


/////////////////////////////////
/*/ Global Instances          /*/ 
/////////////////////////////////

extern CBaseCamera* g_Camera;
extern CAudioManager g_AudioManager;

/////////////////////////////////
/*/ Global variables          /*/ 
/////////////////////////////////

extern float g_FrameTime;

//////////////////////////////////////////////////////////////////////////
/// Update the location and velocity of a sound
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudio::UpdateSound(CVector3f pos, CVector3f vel)
{
    if (m_Channel)
    {
       m_Channel->SetAttributes((float*)&pos, (float*)&vel);
    }
}

//////////////////////////////////////////////////////////////////////////
/// Play a 3D sound
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudio::Play3DAudio(float min = 25.0f, float max = 10000.0f, unsigned int volume = 128)
{
   g_AudioManager.PlayAudio(this);
 
   if (m_Channel)
   {
        m_Channel->SetMinMaxDistances(min, max);
        m_Channel->SetVolume(unsigned int(volume * g_AudioManager.m_SFXVolumeRatio));
   } 
}

//////////////////////////////////////////////////////////////////////////
/// Play a 2D sound
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudio::Play2DAudio(unsigned int volume = 25)
{
   g_AudioManager.PlayAudio(this);
   
   if (m_Channel) 
        m_Channel->SetVolume(unsigned int(volume * g_AudioManager.m_SFXVolumeRatio));
}

//////////////////////////////////////////////////////////////////////////
/// Play a 2D background loop
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudio::PlayBackgroundLoop(unsigned int volume = 25)
{
	Play2DAudio(volume);
}

//////////////////////////////////////////////////////////////////////////
/// Stop audio playing in a channel
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudio::StopAudio()
{
    if (m_Channel)
        g_AudioManager.StopAudio(this);
}

//////////////////////////////////////////////////////////////////////////
/// Adjust the volume of a channel playing audio
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudio::AdjustVolume(unsigned int volume)
{
    if (m_Channel)
        m_Channel->SetVolume(volume);
}