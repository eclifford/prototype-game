#pragma once

#include "graphics/interleavedvertex.h"
#include "graphics/color4ub.h"
#include "math3d/primitives.h"

class CVertexT2FC4UBV3F : public CInterleavedVertex
{
   public:
      CVector2f m_TextureCoord;
      Color4UB m_Color;
      Point3f m_Position;       
};