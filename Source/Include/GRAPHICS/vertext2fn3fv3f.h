#pragma once

#include "graphics/interleavedvertex.h"
#include "math3d/math3d.h"

class CVertexT2FN3FV3F : public CInterleavedVertex
{
   public:
	  CVertexT2FN3FV3F() {}
      
      CVector2f m_TextureCoord;
      CVector3f m_Normal;
      CVector3f m_Vertex; 
};