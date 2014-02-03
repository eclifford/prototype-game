#pragma once

#include "graphics\interleavedarray.h"
#include "graphics\indexarray.h"
#include "graphics\vertext2fv3f.h"
#include "graphics\color4f.h"
#include "graphics\color3ub.h"
#include "math3d\primitives.h"

class CBitmapFont
{
   public:
      void Initialize(const char* pTextureFilePath, const char* pCharWidthsFilePath,
                      unsigned int DefaultWidth, unsigned int DefaultHeight,
                      const FloatRect* pOrthoRect);
      void Shutdown();
      void Print(const char* pText, const CVector2f* pPosition,
                 const Color4f* pColor = 0, const CVector2f* pScale = 0);
      void Print(char Character, const CVector2f* pPosition,
                 const CVector2f* pScale = 0);
      void BeginRendering();
      void EndRendering();
   private:
      
      enum EFontCorner { FONT_LOWLEFT, FONT_LOWRIGHT, FONT_UPLEFT, FONT_UPRIGHT };

      

      float m_CharacterTextureWidth;
      float m_CharacterTextureHeight;

      float m_TextureHeightDivisor;

      float m_CharacterWidthDivisor;
      float m_CharacterHeightDivisor;

      float m_CharacterWidth;
      float m_CharacterHeight;

      float m_HalfWidth;
      float m_HalfHeight;

      float m_TextureWidthOffset;
      float m_TextureHeightOffset;

      float m_PixelHeightFix;



      unsigned int m_Texture;
      CInterleavedArray m_InterleavedArray;
      CIndexArray m_IndexArray;
      CVertexT2FV3F* m_pVertices;

      bool m_Filtered;

      float m_pCharWidths[256];
      float m_pTexWidths[256];
};