#pragma once

#include "graphics\interleavedarray.h"
#include "graphics\indexarray.h"
#include "math3d\objframe.h"
#include "math3d\primitives.h"
#include "graphics\color4ub.h"
#include "graphics\vertext2fc4ubv3f.h"

class CInterfaceRenderable
{
   public:
      CInterfaceRenderable();
      ~CInterfaceRenderable();
      enum EInterfaceCorner { INTERFACE_LOWLEFT, INTERFACE_LOWRIGHT, INTERFACE_UPLEFT, INTERFACE_UPRIGHT };

      void Render();
      void Initialize(const char* pTextureFilePath, const FloatRect* pOrthoRect,
                      const Point3f* pPosition, unsigned int DefaultWidth,
                      unsigned int DefaultHeight);
      void GetRect(FloatRect* pRect);
      
      void SetColor(const Color4UB* pColor);
      void SetColor(const Color4UB* pColor, EInterfaceCorner Corner);

      void CropLeft(float CropRatio);

      void CropBottom(float CropRatio);

      void CropTop(float CropRatio);
      
      void CropRight(float CropRatio);

      CObjFrame m_Frame;
      
      void Shutdown();

   private:
      unsigned int m_Texture;
      CInterleavedArray m_InterleavedArray;
      CIndexArray m_IndexArray;
      CVertexT2FC4UBV3F* m_pVertices;
      CVertexT2FC4UBV3F m_OriginalVerts[4];
};