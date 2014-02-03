#pragma once

#include "material.h"
#include "indexarray.h"

class CMesh
{
   public:
      //CMesh() { }
      //~CMesh();
      void Render();
      void Initialize(unsigned int* pIndices, unsigned int NumIndices, const int PrimitiveType,
                      CMaterial* pMaterial);
      //void SaveToFile(FILE *pFile);
      
   private:
      CIndexArray m_IndexArray;
      CMaterial m_Material;
};