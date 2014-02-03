/// \file geometry.h
/// CGeometry class declaration
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics\interleavedarray.h"
#include "math3d\primitives.h"
#include "graphics\indexarray.h"

/// Class used to store all game geometry information.
class CGeometry
{
   public:
      CGeometry() : m_pIndexArrays(0) { }           
      void Initialize(const char* pPTGFilePath, Sphere* pSphere, AABB* pAABB,
                      int* pTextureIndex, unsigned int* pNumParts,
                      CVector3f** ppTranslations);
      ~CGeometry();
      void Enable();
      void LockVertices();
      void UnlockVertices();
      void RenderPart(unsigned int PartIndex);
      void Unload();

   protected:
      CInterleavedArray m_InterleavedArray;
      CIndexArray* m_pIndexArrays;
      unsigned int m_NumIndexArrays;   
};