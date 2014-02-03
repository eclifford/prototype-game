#pragma once
#include "math3d\math3d.h"

#define PTT_FILE_HEADER 4839495
#define PTT_FILE_HEADER_VER2 6445940

void LoadPTT(char* pFilePath);

void HeightMapToPTT(const char* pHeightMapFilePath, const char* PTTFilePath,
                    const char* pTextureFilePath, unsigned int PatchSize, float TextureTileWidth,
                    float TextureTileHeight, float XScale, float YScale, float ZScale,
                    const CVector2f* pSunDir, float ShadowFactor, float MaxBrightness,
                    float MinBrightness);
