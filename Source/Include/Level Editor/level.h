#pragma once

#include "graphics\renderlist.h"

bool LoadLevel(const char* pPTLFilePath, CRenderList* pRenderList,
               unsigned int* pNumObjects);

bool SaveLevel(const char* pPTLFilePath, unsigned int NumObjects);

struct RandomLevelInfo
{
   float m_TerrainSmoothness;
   float m_TerrainRoughness;

   unsigned int m_NumTrees;
   
   unsigned int m_NumMediumTanks;
   unsigned int m_NumScouts;
   unsigned int m_NumFlyers;
   unsigned int m_NumMissileTanks;
   unsigned int m_NumHeavyTanks;
   unsigned int m_NumKamikazes;

   unsigned int m_NumShieldPowerups;
   unsigned int m_NumTrackingMissilePacks;
   unsigned int m_NumClusterMissilePacks;
   unsigned int m_NumPlasmaUpgrades;
   unsigned int m_NumTrackingMissileUpgrades;
   unsigned int m_NumClusterMissileUpgrades;

   unsigned int m_NumBuildings;
   
   unsigned int m_NumFriendlyTurrets;
   unsigned int m_NumEnemyTurrets;
};

bool LoadRandomLevel(const RandomLevelInfo* pInfo = 0);

void MakeDiamondSquareTerrain(float *fa, int size, int seedValue, float heightScale,
                              float h);

void FilterHeightField(float* fpHeightData, int m_iSize, float fFilter);
