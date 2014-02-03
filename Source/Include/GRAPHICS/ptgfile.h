#pragma once

#include "geometry.h"
#include "math3d\primitives.h"

#define PTG_FILE_HEADER 45
#define PT_FILE_PATH_LEN 256

void LoadPTG(const char* pFilePath, CGeometry* pGeometry, int* pTextureIndex, Sphere* pSphere);

void OBJToPTG(const char* pOBJFilePath, const char* pTextureFilePath, const char* pPTGFilePath,
              bool CenterOnOrigin, float XScale, float YScale, float ZScale);