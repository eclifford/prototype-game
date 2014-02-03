#pragma once
#include "sound\openal.h"

// functions to convert the previous FMOD sound system functions to the OpenAL sound system

unsigned int FSOUND_GetVolume(COpenALSource* channel);

void FSOUND_SetVolume(COpenALSource* channel, unsigned int volume);

void FSOUND_3D_SetAttributes(COpenALSource* channel, float* pPosition, float* pVelocity);


bool FSOUND_IsPlaying(COpenALSource* channel);

void FSOUND_Update();