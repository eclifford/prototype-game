#include "sound\fmod.h"
#include "sound\audiomanager.h"
#include <cassert>

#include "utils\mmgr.h"

extern CAudioManager g_AudioManager;

// functions to convert the previous FMOD sound system functions to the OpenAL sound system

unsigned int FSOUND_GetVolume(COpenALSource* channel)
{
    if (channel)
        return channel->GetVolume();
    else
        return 0;
}

void FSOUND_SetVolume(COpenALSource* channel, unsigned int volume)
{
    if (channel)
        channel->SetVolume(volume);
}

void FSOUND_3D_SetAttributes(COpenALSource* channel, float* pPosition, float* pVelocity)
{
    assert(pPosition);
    assert(pVelocity);

    if (channel)
        channel->SetAttributes(pPosition, pVelocity);
}


bool FSOUND_IsPlaying(COpenALSource* channel)
{
    if (channel)
    {
        return channel->m_Playing;
    }
    else
        return false;
}

void FSOUND_Update()
{
    g_AudioManager.UpdateSources();
}