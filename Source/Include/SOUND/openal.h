#pragma once

#define AL_CHECK_ERRORS() assert(alGetError() == AL_NO_ERROR)

class CAudio;
class CAudioManager;

class COpenALSource
{
   friend class CAudioManager;
    
    public:
        void Clear();
        unsigned int GetVolume();
        void SetVolume(unsigned int Volume);
        bool IsPlaying();
        void SetAttributes(float* pPosition, float* pVelocity);
        void Stop();
        
        void SetMinMaxDistances(float Min, float Max);

        bool m_Playing;

    private:            
        
        unsigned int m_Priority;
        COpenALSource* m_pNext;
        COpenALSource* m_pPrev;

        CAudio* m_pOwner;
        int m_Sound;
        unsigned int m_OpenALHandle;
        
};