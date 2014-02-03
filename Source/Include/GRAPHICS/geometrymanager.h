#pragma once

// forward declarations
class CGeometry;
struct Sphere;
struct AABB;
class CVector3f;

class CGeometryManager
{
   public:
      CGeometryManager();
      ~CGeometryManager();
      void Initialize(unsigned int MaxGeometries);
      void LoadPTGFile(const char* pFilePath, Sphere* pSphere, AABB* pAABB,
                       unsigned int* pGeometryIndex,
                       unsigned int* pNumParts, CVector3f** ppTranslations,
                       int* pTextureIndex);      
      void Enable(unsigned int GeometryIndex);
      void RenderPart(unsigned int GeometryIndex, unsigned int PartIndex);
      void UnloadAll();      
      
   private:      
      CGeometry* m_pGeometries;
      unsigned int m_FreeIndex;
};