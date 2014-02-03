/// \file geometry.cpp
/// Implementation of CGeometry class
////////////////////////////////////////////////////////////////////////////

#include "graphics/geometry.h"
#include "math3d\primitives.h"
#include "graphics/ptgfile.h"
#include "graphics/texturemanager.h"

#include "utils\mmgr.h"

extern CTextureManager g_TextureManager;

void CGeometry::Enable()
{
   m_InterleavedArray.Enable();   
}

void CGeometry::LockVertices()
{
   m_InterleavedArray.Lock();   
}

void CGeometry::UnlockVertices()
{
   m_InterleavedArray.Unlock();
}

CGeometry::~CGeometry()
{
   delete [] m_pIndexArrays;
}

void CGeometry::Unload()
{
   delete [] m_pIndexArrays;
   m_pIndexArrays = 0;
   m_InterleavedArray.Unload();
}

void CGeometry::Initialize(const char* pPTGFilePath, Sphere* pSphere,
                           AABB* pAABB, int* pTextureIndex, unsigned int* pNumParts,
                           CVector3f** ppTranslations)
{
   FILE* pFile = fopen(pPTGFilePath, "rb");
  
   // check for PTG file header
   unsigned int FileHeader;
   fread(&FileHeader, sizeof(FileHeader), 1, pFile);
   if (FileHeader != PTG_FILE_HEADER)
      return; // TODO: INSERT ERROR CHECKING HERE

   // the vertices
   unsigned int NumVertices;
   fread(&NumVertices, sizeof(NumVertices), 1, pFile);
   
   CVertexT2FN3FV3F* pVertices = new CVertexT2FN3FV3F[NumVertices];
   fread(pVertices, sizeof(*pVertices) * NumVertices, 1, pFile);

   // indices and translations
   unsigned int NumGroups;
   fread(&NumGroups, sizeof(NumGroups), 1, pFile);
   *pNumParts = NumGroups;
   m_NumIndexArrays = NumGroups;
   m_pIndexArrays = new CIndexArray[NumGroups];
   *ppTranslations = new CVector3f[NumGroups];

   for (unsigned int i = 0; i < NumGroups; i++)
   {
      unsigned int NumIndices;
      fread(&NumIndices, sizeof(NumIndices), 1, pFile);
      unsigned int* pIndices = new unsigned int[NumIndices];
      fread(pIndices, sizeof(*pIndices) * NumIndices, 1, pFile);
      m_pIndexArrays[i].Initialize(pIndices, NumIndices, GL_TRIANGLES);
   
      // translations
      fread(&((*ppTranslations)[i]), sizeof(CVector3f), 1, pFile);
   }

   // bounding sphere
   fread(pSphere, sizeof(*pSphere), 1, pFile);

   // box
   fread(&pAABB->Min, sizeof(pAABB->Min), 1, pFile);
   fread(&pAABB->Max, sizeof(pAABB->Max), 1, pFile);

   // texture file path
   char pTextureFilePath[PT_FILE_PATH_LEN];
   fread(pTextureFilePath, PT_FILE_PATH_LEN, 1, pFile); 
   
   if (strcmp(pTextureFilePath, "notexture"))
      *pTextureIndex = g_TextureManager.LoadTexture(pTextureFilePath, GL_LINEAR_MIPMAP_LINEAR,
                                                    GL_LINEAR, GL_REPEAT, GL_REPEAT);
   else
      *pTextureIndex = -1;

   m_InterleavedArray.Initialize(pVertices, NumVertices, GL_T2F_N3F_V3F);
   
   fclose(pFile);
}

void CGeometry::RenderPart(unsigned int PartIndex)
{
   m_pIndexArrays[PartIndex].Render();
}