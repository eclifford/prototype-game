#define VC_EXTRALEAN
#include <windows.h>
#include <gl/GL.h>

#include "graphics\interleavedarray.h"
#include "graphics\vertext2fn3fv3f.h"
#include "graphics\vertext2fc4ubv3f.h"
#include "graphics\glextensions.h"

#include "utils\mmgr.h"

extern PFNGLLOCKARRAYSEXTPROC glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT;

CInterleavedArray::~CInterleavedArray()
{   
   Unload();
}

void CInterleavedArray::Unload()
{
   delete [] m_pVertices;
   m_pVertices = 0;
}

void CInterleavedArray::Enable() const
{
   glInterleavedArrays(m_VertexType, 0, m_pVertices);
}

void CInterleavedArray::Lock()
{
   if (glLockArraysEXT)
      glLockArraysEXT(0, m_NumVertices);
}

void CInterleavedArray::Unlock()
{
   if (glUnlockArraysEXT)
      glUnlockArraysEXT();
}

void CInterleavedArray::SaveToFile(FILE* pFile)
{
   fwrite(&m_NumVertices, sizeof(m_NumVertices), 1, pFile);
   fwrite(&m_VertexType, sizeof(m_VertexType), 1, pFile);
   
   switch(m_VertexType)
   {
      case GL_T2F_C4UB_V3F:
         fwrite(m_pVertices, sizeof(CVertexT2FC4UBV3F) * m_NumVertices, 1, pFile);
         break;
      case GL_T2F_N3F_V3F:
         fwrite(m_pVertices, sizeof(CVertexT2FN3FV3F) * m_NumVertices, 1, pFile);
         break;
   }      
}