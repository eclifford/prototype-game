#pragma once

#include <cstdio>

class CIndexArray
{
   public:
	   CIndexArray();
      ~CIndexArray();
      void Unload();
      void Initialize(unsigned int* pIndices, unsigned int NumIndices,
                      unsigned int PrimitiveType)
      {
         m_pIndices = pIndices;
         m_NumIndices = NumIndices;
         m_PrimitiveType = PrimitiveType;
      }
      void Render() const;

      void SaveToFile(FILE* pFile);

      unsigned int GetNumIndices() const { return m_NumIndices; }
      const unsigned int* GetIndices() const { return m_pIndices; }

   private:
      unsigned int* m_pIndices;
      unsigned int m_NumIndices;
      int m_PrimitiveType;
};