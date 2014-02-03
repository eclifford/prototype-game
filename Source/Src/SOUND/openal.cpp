#include <al\al.h>
#include <cassert>
#include "sound\openal.h"

#include "utils\mmgr.h"

void COpenALSource::Clear()
{
    m_OpenALHandle = 0;
    m_Priority = 0;
    m_pNext = 0;
    m_pPrev = 0;
    m_pOwner = 0;
    m_Sound = -1;
    m_Playing = 0;
}

unsigned int COpenALSource::GetVolume()
{
    if (m_Playing)
    {
        float Volume;
        alGetSourcef(m_OpenALHandle, AL_GAIN, &Volume);
        AL_CHECK_ERRORS();
        // convert volume from 0.0f-1.0f to 0-255
        return unsigned int(Volume * 255);
    }
    else
        return 0;
}

void COpenALSource::SetVolume(unsigned int Volume)
{
    if (m_Playing)
    {
        // clamp volume
        if (Volume > 255)
            Volume = 255;

        // set the volume in OpenAL
        alSourcef(m_OpenALHandle, AL_GAIN, Volume / 255.f);
        AL_CHECK_ERRORS();
    }
}

void COpenALSource::SetAttributes(float* pPosition, float* pVelocity)
{
    assert(pPosition);
    assert(pVelocity);

    if (m_Playing)
    {
        // set the position and velocity in OpenAL
        alSourcefv(m_OpenALHandle, AL_POSITION, pPosition);
        AL_CHECK_ERRORS();

        alSourcefv(m_OpenALHandle, AL_VELOCITY, pVelocity);
        AL_CHECK_ERRORS();
    }
}

bool COpenALSource::IsPlaying()
{
    if (m_Playing)
    {
        int State;
        alGetSourcei(m_OpenALHandle, AL_SOURCE_STATE, &State);
        AL_CHECK_ERRORS();
        return State == AL_PLAYING;
    }
    else
        return false;
}


void COpenALSource::SetMinMaxDistances(float Min, float Max)
{
    if (m_Playing)
    {
        alSourcef(m_OpenALHandle, AL_REFERENCE_DISTANCE, Min);
        AL_CHECK_ERRORS();
        alSourcef(m_OpenALHandle, AL_MAX_DISTANCE, Max);
        AL_CHECK_ERRORS();
    }
}

void COpenALSource::Stop()
{
    if (m_Playing)
    {
        // stop the source
        alSourceStop(m_OpenALHandle);
        AL_CHECK_ERRORS();

        m_Playing = false;
    }
}