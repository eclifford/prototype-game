#define VC_EXTRALEAN
#include <windows.h>
#include <gl/GL.h>

#include "graphics/indexarray.h"

#include "utils\mmgr.h"

CIndexArray::CIndexArray()
{
    m_pIndices = 0;
}

CIndexArray::~CIndexArray()
{
   Unload();
}

void CIndexArray::Unload()
{
    delete [] m_pIndices;
    m_pIndices = 0;
}

void CIndexArray::Render() const
{
   glDrawElements(m_PrimitiveType, m_NumIndices, GL_UNSIGNED_INT, m_pIndices);
}

void CIndexArray::SaveToFile(FILE* pFile)
{
   fwrite(&m_NumIndices, sizeof(unsigned int), 1, pFile);
   fwrite(&m_PrimitiveType, sizeof(unsigned int), 1, pFile);
   fwrite(m_pIndices, sizeof(unsigned int) * m_NumIndices, 1, pFile);
}