#pragma once

#include "graphics/interleavedvertex.h"
#include "graphics/color4f.h"
#include "math3d/primitives.h"

class CVertexT2FC4FN3FV3F : public CInterleavedVertex
{
   public:
      CVector2f m_TextureCoord;
      Color4f m_Color;
      CVector3f m_Normal;
      Point3f m_Position;       
};