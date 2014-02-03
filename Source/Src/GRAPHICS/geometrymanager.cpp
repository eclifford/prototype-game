#include "graphics\geometrymanager.h"
#include "graphics\vertext2fn3fv3f.h"
#include "graphics\ptgfile.h"

#include "utils\mmgr.h"

CGeometryManager::CGeometryManager()
{
    m_FreeIndex = 0;
    m_pGeometries = 0;
}

CGeometryManager::~CGeometryManager()
{
   UnloadAll();
}

void CGeometryManager::UnloadAll()
{
   for (unsigned int i = 0; i < m_FreeIndex; ++i)
   {
      m_pGeometries[i].Unload();
   }

   m_FreeIndex = 0;
   
   delete [] m_pGeometries;
   m_pGeometries = 0;
}

void CGeometryManager::Initialize(unsigned int MaxGeometries)
{
   m_pGeometries = new CGeometry[MaxGeometries];
}

void CGeometryManager::LoadPTGFile(const char* pFilePath, Sphere* pSphere, AABB* pAABB,
                                   unsigned int* pGeometryIndex, unsigned int* pNumParts,
                                   CVector3f** ppTranslations, int* pTextureIndex)
{    
   m_pGeometries[m_FreeIndex].Initialize(pFilePath, pSphere, pAABB, pTextureIndex, pNumParts,
                                         ppTranslations);

   *pGeometryIndex = m_FreeIndex++;   
}

void CGeometryManager::Enable(unsigned int GeometryIndex)
{
   m_pGeometries[GeometryIndex].Enable();
}

void CGeometryManager::RenderPart(unsigned int GeometryIndex, unsigned int PartIndex)
{
   m_pGeometries[GeometryIndex].RenderPart(PartIndex);
}