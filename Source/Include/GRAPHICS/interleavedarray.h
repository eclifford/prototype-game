#pragma once

#include "interleavedvertex.h"
#include "vertext2fn3fv3f.h"
#include <cstdio>

class CInterleavedArray
{
   public:
      CInterleavedArray() : m_pVertices(0) {}
      ~CInterleavedArray();
      void Enable() const;
      void Initialize(CInterleavedVertex* pVertices, unsigned int NumVertices,
                      unsigned int VertexType)
      {
         m_pVertices = pVertices;
         m_VertexType = VertexType;
         m_NumVertices = NumVertices;
      }
      unsigned int GetVertexType() const { return m_VertexType; }
      void SaveToFile(FILE* pFile);
      void Lock();
      void Unlock();
      void Unload();

   private:
      CInterleavedVertex* m_pVertices;
      unsigned int m_VertexType;
      unsigned int m_NumVertices;
};