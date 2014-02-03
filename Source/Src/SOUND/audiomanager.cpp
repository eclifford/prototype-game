/// \file audiomanager.h
/// Manager for components of FMOD 
/////////////////////////////////////////////////////////////////////////////
#include <AL\al.h>
#include <AL\alut.h>
#include <cassert>
#include <vector>

#include "camera\camera.h"
#include "sound\audiomanager.h"
#include "core\gamemain.h"
#include "sound\audio.h"

#include "utils\mmgr.h"

extern CBaseCamera* g_Camera;
extern float g_FrameTime;

#define ALC_CHECK_ERRORS(device) assert(alcGetError(device) == ALC_NO_ERROR)

/*
void SSound::SetMinMaxDistance(float Min, float Max)
{
    
}*/

void CAudioManager::PlaySource(COpenALSource* pSource)
{
    // set this source's sound to the appropriate buffer         
    if (pSource->m_Sound == -1 || pSource->m_Sound != pSource->m_pOwner->m_Sound)
    {
        pSource->m_Sound = pSource->m_pOwner->m_Sound; 
        alSourcei(pSource->m_OpenALHandle, AL_BUFFER, m_aSounds[pSource->m_Sound].m_Buffer);
        AL_CHECK_ERRORS();
    }
           

    pSource->m_pNext = 0;
    pSource->m_pPrev = 0;
    pSource->m_Playing = true;

    ++m_NumSourcesPlaying;   
    ++m_aSounds[pSource->m_Sound].m_CurrentPlaybacks;   

    pSource->SetVolume(255);

    if (m_aSounds[pSource->m_Sound].m_Loop)
    {
        alSourcei(pSource->m_OpenALHandle, AL_LOOPING, true);
        AL_CHECK_ERRORS();
    }
    else
    {
        alSourcei(pSource->m_OpenALHandle, AL_LOOPING, false);
        AL_CHECK_ERRORS();
    }

    if (m_aSounds[pSource->m_Sound].m_Is3D)
    {
        alSourcef(pSource->m_OpenALHandle, AL_ROLLOFF_FACTOR, m_RolloffFactor);
        AL_CHECK_ERRORS();
    }
    else
    {
        alSourcef(pSource->m_OpenALHandle, AL_ROLLOFF_FACTOR, 1.0f);
        AL_CHECK_ERRORS();
        
         if (g_Camera)
         {
        // set the source's position and and velocity to the same as the listener's
        // because it's 2D
        float listenervel[3];

        CVector3f camerapos;
	    g_Camera->GetPosition(&camerapos);

        listenervel[0] = (camerapos.x - m_ListenerLastPosition.x) / g_FrameTime;
        listenervel[1] = (camerapos.y - m_ListenerLastPosition.y) / g_FrameTime;
        listenervel[2] = (camerapos.z - m_ListenerLastPosition.z) / g_FrameTime;
    
        pSource->SetAttributes((float*)&camerapos, listenervel);
        }
    }

    alSourcePlay(pSource->m_OpenALHandle);
    AL_CHECK_ERRORS();

    // if there aren't any other playing sources just add it to the front of the list
    if (!m_pSourcesPlaying)
    {                    
        m_pSourcesPlaying = pSource;
        return;
    }

    // sort highest priority to the back of the list
    COpenALSource* pCurrentSource = m_pSourcesPlaying;

    while (true)
    {
        // if we found another source with a higher or equal priority
        if (pSource->m_Priority <= pCurrentSource->m_Priority)
        {
            if (pCurrentSource->m_pPrev)
            {
                pCurrentSource->m_pPrev->m_pNext = pSource;    
            }
            else // if it doesn't have a previous
            {
                // it must be the first one on the list, so make it the first one
                m_pSourcesPlaying = pSource;
            }
        
            // insert before the current source
            pSource->m_pPrev = pCurrentSource->m_pPrev;
            pSource->m_pNext = pCurrentSource;                        
        
            pCurrentSource->m_pPrev = pSource;            
        
            return;
        }
    
        // if we've reached the end of the list
        if (!pCurrentSource->m_pNext)
        {
            pSource->m_pPrev = pCurrentSource;
            pCurrentSource->m_pNext = pSource;
            return;
        }
    
        pCurrentSource = pCurrentSource->m_pNext;   
    }
}

void CAudioManager::StopSource(COpenALSource* pSource)
{
    if (pSource->m_Playing)
    {
        RemoveSourceFromPlaying(pSource);        
        --(m_aSounds[pSource->m_Sound].m_CurrentPlaybacks);
        assert(m_aSounds[pSource->m_Sound].m_CurrentPlaybacks <= m_aSounds[pSource->m_Sound].m_MaxPlaybacks);
        --m_NumSourcesPlaying;
        AddToSourcePool(pSource);
    }
    
    pSource->Stop();    
    
    if (pSource == pSource->m_pOwner->m_Channel)
        pSource->m_pOwner->m_Channel = 0;
}

void CAudioManager::RemoveSourceFromPlaying(COpenALSource* pSource)
{
    assert(pSource);

    if (pSource->m_pPrev)
        pSource->m_pPrev->m_pNext = pSource->m_pNext;
    else
        m_pSourcesPlaying = pSource->m_pNext;

    if (pSource->m_pNext)
        pSource->m_pNext->m_pPrev = pSource->m_pPrev;

    pSource->m_pNext = 0;
    pSource->m_pPrev = 0;
}

void CAudioManager::AddToSourcePool(COpenALSource* pSource)
{
    assert(pSource);

    pSource->m_pNext = m_pSourcePool;
    pSource->m_pPrev = 0;

    if (m_pSourcePool)
    {
        m_pSourcePool->m_pPrev = pSource;          
    }

    m_pSourcePool = pSource;    
}

COpenALSource* CAudioManager::GetSourceFromPool()
{
    assert(m_pSourcePool);

    COpenALSource* pSource = m_pSourcePool;

    if (m_pSourcePool->m_pNext)
    {
        m_pSourcePool->m_pNext->m_pPrev = 0;   
        //m_pSourcePool = m_pSourcePool->m_pNext;
    }
    //else
        m_pSourcePool = m_pSourcePool->m_pNext;
    
    
    #ifdef _DEBUG
    // count the number of sources in the source pool for error checking
    COpenALSource* pCurrentSource = m_pSourcePool;
    unsigned int NumSourcesInSourcePool = 0;
    while (pCurrentSource)
    {
        ++NumSourcesInSourcePool;

        pCurrentSource = pCurrentSource->m_pNext;
    }

    assert(NumSourcesInSourcePool <= m_MaxSourcesPlaying - m_NumSourcesPlaying);
    #endif
    

    return pSource;
}

void CAudioManager::PlayAudio(CAudio* pAudio)
{
    assert(pAudio);

    
    // if we've reached the max playbacks for this sound then return
    if (m_aSounds[pAudio->m_Sound].m_CurrentPlaybacks ==
        m_aSounds[pAudio->m_Sound].m_MaxPlaybacks)
    {        
        pAudio->m_Channel = 0;
        return;
    }

    // if we are currently playing the maximum number of sources
    assert(m_NumSourcesPlaying <= m_MaxSourcesPlaying);
    if (m_NumSourcesPlaying == m_MaxSourcesPlaying)
    {
        // quick check for priority of lowest source
        if (pAudio->m_Priority < m_pSourcesPlaying->m_Priority)
        {
            // no free channels
            pAudio->m_Channel = 0;
            return;
        }
        else
        {
            StopSource(m_pSourcesPlaying);               
        }
    }
    

 
    // get a source from the pool
    COpenALSource* pNewSource = GetSourceFromPool();
    
    pNewSource->m_pOwner = pAudio;
    pNewSource->m_Priority = pAudio->m_Priority;
    pAudio->m_Channel = pNewSource;
    
    // play it
    PlaySource(pNewSource);        
}

void CAudioManager::StopAudio(CAudio* pAudio)
{
    StopSource(pAudio->m_Channel);
    pAudio->m_Channel = 0;
}

//////////////////////////////////////////////////////////////////////////
/// Initialize the audiomanager
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudioManager::Initialize()
{
   // reset variables
    for (unsigned int i = 0; i < NUM_SOUNDS; ++i)
    {
        samples[i] = 0;
        m_aSounds[i].Clear();
    }

    m_RolloffFactor = .3f;
    m_ListenerLastPosition.x = 0.0f;
    m_ListenerLastPosition.y = 0.0f;
    m_ListenerLastPosition.z = 0.0f;
    m_SFXVolumeRatio = 1.0f;
    m_BGVolumeRatio = 1.0f;

    m_NextFreeSound = 0;
    m_NumSourcesPlaying = 0;

    m_pAudioObjects = 0;

    m_pSourcesPlaying = 0;

    m_pOpenALDevice = 0;
    m_pOpenALContext = 0;

    // initialize OpenAL
    m_pOpenALDevice = alcOpenDevice((ALubyte*)"DirectSound3D");
    ALC_CHECK_ERRORS(m_pOpenALDevice);
    assert(m_pOpenALDevice);

    m_pOpenALContext = alcCreateContext(m_pOpenALDevice, 0);
    ALC_CHECK_ERRORS(m_pOpenALDevice);
    
    alcMakeContextCurrent(m_pOpenALContext);
    ALC_CHECK_ERRORS(m_pOpenALDevice);
    
    alcProcessContext(m_pOpenALContext);
    ALC_CHECK_ERRORS(m_pOpenALDevice);
    
        
    // figure out the max number of sources we can have playing at once
    std::vector<unsigned int> Sources;
    unsigned int Source;
    while (true)
    {
        alGenSources(1, &Source);
        int Error = alGetError();
        
        // if we've run out of sources then this is the max number 
        if (Error == AL_INVALID_VALUE)
        {
            assert(Sources.size() != 0);
            break;    
        }
        
        assert(Error == AL_INVALID_VALUE || Error == AL_NO_ERROR);

        Sources.push_back(Source);
    }
    

    m_MaxSourcesPlaying = (unsigned int)Sources.size();
    
    // create an array of sources
    m_pAllSourcesArray = new COpenALSource[m_MaxSourcesPlaying];

    // link the sources together in the source pool
    // set up the first and last source manually
    m_pAllSourcesArray[0].Clear();
    m_pAllSourcesArray[0].m_pNext = &(m_pAllSourcesArray[1]);
    m_pAllSourcesArray[0].m_OpenALHandle = Sources[0];
    m_pAllSourcesArray[m_MaxSourcesPlaying - 1].Clear();
    m_pAllSourcesArray[m_MaxSourcesPlaying - 1].m_pPrev = &(m_pAllSourcesArray[m_MaxSourcesPlaying-2]);
    m_pAllSourcesArray[m_MaxSourcesPlaying - 1].m_OpenALHandle = Sources[m_MaxSourcesPlaying - 1];
                                                                                 
    for (i = 1; i < m_MaxSourcesPlaying - 1; ++i)
    {
        m_pAllSourcesArray[i].Clear();
        m_pAllSourcesArray[i].m_pPrev = &m_pAllSourcesArray[i - 1];
        m_pAllSourcesArray[i].m_pNext = &m_pAllSourcesArray[i + 1];            
        m_pAllSourcesArray[i].m_OpenALHandle = Sources[i];
    }
    
    Sources.clear();        

    // make the source pool point to the beginning of all the linked sources
    m_pSourcePool = m_pAllSourcesArray;
}

//////////////////////////////////////////////////////////////////////////
/// Load the menu sounds
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudioManager::LoadMenuSounds()
{
   samples[SFX_TITLEBG] = LoadSound("resources/music/titlebg.wav", false, true, 1);
   samples[SFX_BUTTONCLK] = LoadSound("resources/sounds/buttonclick.wav", false, false, 1);
   samples[SFX_RADIOCLK] = LoadSound("resources/sounds/radioclick.wav", false, false, 1);
   samples[SFX_BUTTONSET] = LoadSound("resources/sounds/buttonset.wav", false, false, 1);   
}

//////////////////////////////////////////////////////////////////////////
/// Load the game sounds
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudioManager::LoadGameSounds()
{
   samples[SFX_LASERSHOT] = LoadSound("resources/sounds/shockrifle_shoot.wav", true, false, 10);

   samples[SFX_PLAYER_TURRET] = LoadSound("resources/sounds/cannon_blast.wav", true, false, 10);
   samples[SFX_ENEMY_TURRET] = LoadSound("resources/sounds/cannon_blast.wav", true, false, 10);
   samples[SFX_FRIENDLY_TURRET] = LoadSound("resources/sounds/cannon_blast.wav", true, false, 10);
   samples[SFX_PLAYER_ENGINE] = LoadSound("resources/sounds/Engine.wav", true, true, 1);
   samples[SFX_PLAYER_JUMPJET] = LoadSound("resources/sounds/Playerjumpjet.wav", true, true, 2);
   samples[SFX_PLAYER_SHIELDABSORB] = LoadSound("resources/sounds/shielddeflect.wav", true, false, 3);
   samples[SFX_PLAYER_HULLHIT] = LoadSound("resources/sounds/projectilehit2.wav", true, false, 3);
   samples[SFX_PLAYER_IONCANNON] = LoadSound("resources/sounds/ionblast.wav", true, false, 1);
   samples[SFX_MISSILEFIRED] = LoadSound("resources/sounds/missiledeploy.wav", true, false, 5);
   samples[SFX_ENEMYMISSILEFIRED] = LoadSound("resources/sounds/missiledeploy.wav", true, false, 15);
   samples[SFX_MISSILEINAIR] = LoadSound("resources/sounds/missileair.wav", true, true, 10);
   samples[SFX_PLASMAINAIR] = LoadSound("resources/sounds/plasmaair.wav", true, true, 10);
   samples[SFX_EXP_TINY] = LoadSound("resources/sounds/EXP_TINY.wav", true, false, 5);
   samples[SFX_EXP_SMALL] = LoadSound("resources/sounds/EXP_SMALL.wav", true, false, 5);
   samples[SFX_EXP_MED] = LoadSound("resources/sounds/EXP_MEDIUM.wav", true, false, 5);
   samples[SFX_EXP_LARGE] = LoadSound("resources/sounds/EXP_LARGE.wav", true, false, 5);
   samples[SFX_EXP_HUGE] = LoadSound("resources/sounds/EXP_HUGE.wav", true, false, 5);
   samples[SFX_ENEMY_ENGINE] = LoadSound("resources/sounds/EnemyEngine.wav", true, true, 10);
   samples[SFX_FIRE] = LoadSound("resources/sounds/fire.wav", true, true, 5);
   samples[SFX_OBJCOLLIDE] = LoadSound("resources/sounds/VehicleCollide.wav", true, false, 3);
   samples[SFX_SHLDPWRUP] = LoadSound("resources/sounds/shieldpowerup.wav", false, false, 1);
   samples[SFX_AMMOPWRUP] = LoadSound("resources/sounds/ammopowerup.wav", false, false, 1);
   samples[SFX_WPNUPGRADE] = LoadSound("resources/sounds/WeaponUpgrade.wav", false, false, 1);
   samples[SFX_WPNCHANGE] = LoadSound("resources/sounds/gun_change03.wav", false, false, 1);
   samples[SFX_LOCKON] = LoadSound("resources/sounds/lockonbeep.wav", false, false, 1);
   samples[SFX_TREEFALL] = LoadSound("resources/sounds/RanOvertrees.wav", true, false, 3);
   
   samples[SFX_MOTHERSHIP] = LoadSound("resources/sounds/Playerjumpjet.wav", true, true, 1);
   samples[SFX_CONVOYENGINE] = LoadSound("resources/sounds/CONVOY.wav", true, true, 3);
   samples[SFX_IONFLARE] = LoadSound("resources/sounds/ionflare.wav", true, false, 1);
   samples[SFX_CHARGE] = LoadSound("resources/sounds/charge.wav", true, false, 1);
   samples[SFX_CHARGE2] = LoadSound("resources/sounds/charge2.wav", true, true, 1);
   samples[SFX_LASERHIT] = LoadSound("resources/sounds/laserhit3.wav", true, false, 5);
   samples[SFX_BUILDINGCOLLAPSE1] = LoadSound("resources/sounds/BuildingCollapse.wav", true, false, 3);
   samples[SFX_BUILDINGCOLLAPSE2] = LoadSound("resources/sounds/BuildingCollapse2.wav", true, true, 3);
   samples[SFX_INCOMINGMSG] = LoadSound("resources/sounds/incomingmessage.wav", false, false, 1);
   samples[SFX_IONWAVE] = LoadSound("resources/sounds/ionwave.wav", true, false, 1);
   samples[SFX_DROPBOMB] = LoadSound("resources/sounds/dropbomb.wav", true, false, 10);
   samples[SFX_FIGHTER] = LoadSound("resources/sounds/playerjumpjet.wav", true, true, 10);

   samples[SFX_LVL1SEQUENCELOOP] = LoadSound("resources/music/lvl1sequenceloop.wav", false, true, 1);
   samples[SFX_LEVEL1_BG] = LoadSound("resources/music/level1bg.wav", false, true, 1);
}

//////////////////////////////////////////////////////////////////////////
/// Load a sound
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CAudioManager::LoadSound(const char * fn, bool is3D, bool loop, unsigned int maxplaybacks)
{
    assert(m_NextFreeSound < NUM_SOUNDS);

    // load the sound into the next free sound slot
    assert(fn);

    ALenum Format;
    ALvoid* Data;
    ALsizei Size, Freq;    
    ALboolean Loop;    
    
    alutLoadWAVFile((ALbyte *)fn, &Format, &Data, &Size, &Freq, &Loop);
    AL_CHECK_ERRORS();
    
    assert(Data);

    // load the sound into this buffer
    alGenBuffers(1, &m_aSounds[m_NextFreeSound].m_Buffer);
    AL_CHECK_ERRORS();        
    alBufferData(m_aSounds[m_NextFreeSound].m_Buffer, Format, Data, Size, Freq);
    AL_CHECK_ERRORS();
   
    // unload the wav file
    alutUnloadWAV(Format, Data, Size, Freq);

    AL_CHECK_ERRORS();
    
    m_aSounds[m_NextFreeSound].m_Is3D = is3D;
    m_aSounds[m_NextFreeSound].m_Loop = loop;
    m_aSounds[m_NextFreeSound].m_MaxPlaybacks = maxplaybacks;
    m_aSounds[m_NextFreeSound].m_CurrentPlaybacks = 0;
    m_aSounds[m_NextFreeSound].m_Loaded = true;
    
    return m_NextFreeSound++;        
}

//////////////////////////////////////////////////////////////////////////
/// Create a sound
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CAudio* CAudioManager::CreateSound(unsigned int Sound, unsigned int Priority, bool IsPaused)
{
   CAudio* pAudio = new CAudio;

   pAudio->m_Sound = Sound;

   pAudio->m_Priority = Priority;
   pAudio->m_Channel = 0;

   if (m_pAudioObjects)
   {
        pAudio->m_pNext = m_pAudioObjects;
        m_pAudioObjects = pAudio;  
   }
   else
   {
        m_pAudioObjects = pAudio;
        m_pAudioObjects->m_pNext = 0;     
   }
   
   return pAudio;
}

//////////////////////////////////////////////////////////////////////////
/// Update the audiomanager and the listener
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudioManager::Update()
{
    // Setup the OpenAL Listener
   float listenervel[3];

   CVector3f camerapos;
	g_Camera->GetPosition(&camerapos);

   listenervel[0] = (camerapos.x - m_ListenerLastPosition.x) / g_FrameTime;
   listenervel[1] = (camerapos.y - m_ListenerLastPosition.y) / g_FrameTime;
   listenervel[2] = (camerapos.z - m_ListenerLastPosition.z) / g_FrameTime;

   m_ListenerLastPosition = camerapos;

   CVector3f cameraLookAt;
   g_Camera->GetLookAt(&cameraLookAt);

   CVector3f cameraUp;
   g_Camera->GetUp(&cameraUp);

   alListenerfv(AL_POSITION, (float*)&camerapos);
    AL_CHECK_ERRORS();
    //alListenerfv(AL_VELOCITY, listenervel);
    //AL_CHECK_ERRORS();
    
    float Orientation[6];
    Orientation[0] = cameraLookAt.x;
    Orientation[1] = cameraLookAt.y;
    Orientation[2] = cameraLookAt.z;

    Orientation[3] = cameraUp.x;
    Orientation[4] = cameraUp.y;
    Orientation[5] = cameraUp.z;

    alListenerfv(AL_ORIENTATION, Orientation);
    AL_CHECK_ERRORS();
   
   // Update all sources
    UpdateSources(); 
}

void CAudioManager::UpdateSources()
{
    // go through all our playing sources and check for ones that aren't playing anymore
    COpenALSource* pCurrentSource = m_pSourcesPlaying;
    COpenALSource* pNextSource;
    while (pCurrentSource)
    {
        pNextSource = pCurrentSource->m_pNext;

        if (!pCurrentSource->IsPlaying())
        {
            assert(!m_aSounds[pCurrentSource->m_Sound].m_Loop);
            StopSource(pCurrentSource);
        }
        else if (g_Camera && !m_aSounds[pCurrentSource->m_Sound].m_Is3D)
        {
            // set the source's position and and velocity to the same as the listener's
            float listenervel[3];

            CVector3f camerapos;
	        g_Camera->GetPosition(&camerapos);

            listenervel[0] = (camerapos.x - m_ListenerLastPosition.x) / g_FrameTime;
            listenervel[1] = (camerapos.y - m_ListenerLastPosition.y) / g_FrameTime;
            listenervel[2] = (camerapos.z - m_ListenerLastPosition.z) / g_FrameTime;
    
            pCurrentSource->SetAttributes((float*)&camerapos, listenervel);
        }

        pCurrentSource = pNextSource;
    }
}

void CAudioManager::SetAudioRatios(float sfxratio, float bgratio)
{
	m_SFXVolumeRatio = sfxratio;
	m_BGVolumeRatio = bgratio;
}

//////////////////////////////////////////////////////////////////////////
/// Shutdown the audiomanager and clean up the memory
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CAudioManager::Shutdown()
{
   // delete all audio objects
   CAudio* pCurrentAudio = m_pAudioObjects;
   CAudio* pNextAudio;

   while (pCurrentAudio)
   {
        pNextAudio = pCurrentAudio->m_pNext;
        delete pCurrentAudio;
        pCurrentAudio = pNextAudio;
   }
   
    // stop all sources
   COpenALSource* pCurrentSource = m_pSourcesPlaying;   
   while (pCurrentSource)
   {
        pCurrentSource->Stop();
        pCurrentSource = pCurrentSource->m_pNext;
   }

   // delete all sources
   for (unsigned int i = 0; i < m_MaxSourcesPlaying; ++i)
   {
        alDeleteSources(1, &m_pAllSourcesArray[i].m_OpenALHandle);
        AL_CHECK_ERRORS();
   }

   delete [] m_pAllSourcesArray;

   // delete all sound files loaded
    for (i = 0; i < NUM_SOUNDS; ++i)
    {
        if (m_aSounds[i].m_Loaded)
        {
            alDeleteBuffers(1, &m_aSounds[i].m_Buffer);
            //AL_CHECK_ERRORS();
        
            // DEBUG
            unsigned int Error = alGetError();
            if (Error != AL_NO_ERROR)
            {
                bool debug = true;
            }
        }
    }
    
    // shutdown OpenAL
    alcSuspendContext(m_pOpenALContext);
    ALC_CHECK_ERRORS(m_pOpenALDevice);

    alcMakeContextCurrent(0);
    ALC_CHECK_ERRORS(m_pOpenALDevice);
    
    alcDestroyContext(m_pOpenALContext);
    ALC_CHECK_ERRORS(m_pOpenALDevice);
    
    alcCloseDevice(m_pOpenALDevice);    
}