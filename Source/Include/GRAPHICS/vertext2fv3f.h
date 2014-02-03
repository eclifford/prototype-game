#pragma once

#include "interleavedvertex.h"
#include "math3d/math3d.h"

class CVertexT2FV3F : public CInterleavedVertex
{
   public:
	   CVertexT2FV3F() {}
      CVector2f m_TextureCoord;
      CVector3f m_Position; 
};