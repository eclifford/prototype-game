#include <math.h>


#include "core\oglwindow.h"
#include "core\oglstatemanager.h"
#include "graphics\texturemanager.h"
#include "graphics\bitmapfont.h"

#include "utils\mmgr.h"

extern COpenGLWindow* pGLWIN;
extern CTextureManager g_TextureManager;
extern COpenGLStateManager* pGLSTATE;

void CBitmapFont::Initialize(const char* pTextureFilePath,
                             const char* pCharWidthsFilePath, unsigned int DefaultWidth,
                             unsigned int DefaultHeight, const FloatRect* pOrthoRect)
{
   unsigned int OldWidth;
   unsigned int OldHeight;
   unsigned int NewWidth;
   unsigned int NewHeight;

   float WidthSizeRatio = (float)pGLWIN->GetScreenWidth() / DefaultWidth; 
   float HeightSizeRatio = (float)pGLWIN->GetScreenHeight() / DefaultHeight;    

   // load in the texture
   
   if (DefaultWidth != (unsigned int)pGLWIN->GetScreenWidth() || DefaultHeight != (unsigned int)pGLWIN->GetScreenHeight())
   {
      m_Texture = g_TextureManager.LoadTexture(pTextureFilePath, GL_LINEAR,
                                               GL_LINEAR, GL_CLAMP, GL_CLAMP, &OldWidth,
                                               &OldHeight, &NewWidth, &NewHeight);
      
      m_PixelHeightFix = (1.0f / NewHeight * .5f);
   }
   else
   {
      m_Texture = g_TextureManager.LoadTexture(pTextureFilePath, GL_NEAREST,
                                               GL_NEAREST, GL_CLAMP, GL_CLAMP, &OldWidth,
                                               &OldHeight, &NewWidth, &NewHeight);
      m_PixelHeightFix = 0.0f;
   }

   // TEMP
   NewWidth = OldWidth;
   NewHeight = OldHeight;

   // load in the font widths
   FILE* pFile = fopen(pCharWidthsFilePath, "r");
   unsigned char pWidths[256];

   fread(pWidths, 256, 1, pFile);


   fclose(pFile);

   float OrthoRectWidth = pOrthoRect->Max.x - pOrthoRect->Min.x;
   unsigned int ScreenWidth = pGLWIN->GetScreenWidth();
   
   float TextureWidthRatio = (float)OldWidth / (float)NewWidth;

   for (unsigned int i = 0; i < 256; ++i)
   {
      m_pCharWidths[i] = (float)pWidths[i] * (OrthoRectWidth / (float)ScreenWidth) *
                         TextureWidthRatio * WidthSizeRatio;
      m_pTexWidths[i] = ((float)pWidths[i] / (float)OldWidth) * TextureWidthRatio * .5f;
   }

   m_TextureWidthOffset = (float)OldWidth / (float)NewWidth;
   m_TextureHeightOffset = (float)OldHeight / (float)NewHeight;

   m_Filtered = false;
   //g_TextureManager.SetFilters(m_Texture, GL_NEAREST, GL_NEAREST);
   // start creating the vertices
   m_pVertices = new CVertexT2FV3F[4];
   
   // calculate the Half width and Half height  
   float CharacterWidth = (float)OldWidth / 16.0f;
   float CharacterHeight = (float)OldHeight / 16.0f;
   
   m_HalfWidth = .5f * (float)CharacterWidth * OrthoRectWidth / ScreenWidth;
   m_HalfHeight = .5f * (float)CharacterHeight * 
                  ((pOrthoRect->Max.y - pOrthoRect->Min.y)) / pGLWIN->GetScreenHeight();

   m_CharacterWidth = m_HalfWidth * 2.0f;
   m_CharacterHeight = m_HalfHeight * 2.0f * HeightSizeRatio;

   m_CharacterTextureWidth = (CharacterWidth / (float)OldWidth) * TextureWidthRatio;
   m_CharacterTextureHeight = (CharacterHeight / (float)OldHeight) * ((float)OldHeight /
                              (float)NewHeight);

   // lower left vertex   

   //m_pVertices[FONT_LOWLEFT].m_Position.x = -m_HalfWidth;
   //m_pVertices[FONT_LOWLEFT].m_Position.y = -m_HalfHeight;
   m_pVertices[FONT_LOWLEFT].m_Position.z = 0;

   //m_pVertices[FONT_LOWLEFT].m_TextureCoord.x = 0;
   //m_pVertices[FONT_LOWLEFT].m_TextureCoord.y = 0;

   // lower right vertex   

   //m_pVertices[FONT_LOWRIGHT].m_Position.x = m_HalfWidth;
   //m_pVertices[FONT_LOWRIGHT].m_Position.y = -m_HalfHeight;
   m_pVertices[FONT_LOWRIGHT].m_Position.z = 0;

   //m_pVertices[FONT_LOWRIGHT].m_TextureCoord.x = OldToNewWidthRatio;
   //m_pVertices[FONT_LOWRIGHT].m_TextureCoord.y = 0;

   // upper left vertex   

   //m_pVertices[FONT_UPLEFT].m_Position.x = -m_HalfWidth;
   //m_pVertices[FONT_UPLEFT].m_Position.y = m_HalfHeight;
   m_pVertices[FONT_UPLEFT].m_Position.z = 0;

   //m_pVertices[FONT_UPLEFT].m_TextureCoord.x = 0;
   //m_pVertices[FONT_UPLEFT].m_TextureCoord.y = OldToNewHeightRatio;

   // upper right vertex   

   //m_pVertices[FONT_UPRIGHT].m_Position.x = m_HalfWidth;
   //m_pVertices[FONT_UPRIGHT].m_Position.y = m_HalfHeight;
   m_pVertices[FONT_UPRIGHT].m_Position.z = 0;

   //m_pVertices[FONT_UPRIGHT].m_TextureCoord.x = OldToNewWidthRatio;
   //m_pVertices[FONT_UPRIGHT].m_TextureCoord.y = OldToNewHeightRatio;

   m_InterleavedArray.Initialize(m_pVertices, 4, GL_T2F_V3F);
   
   //memcpy(&m_OriginalVerts, m_pVertices, sizeof(CVertexT2FC4UBV3F) * NumVertices);

   // set up the indices
   unsigned int* pIndices = new unsigned int[4];
   
   pIndices[0] = 0;
   pIndices[1] = 1;
   pIndices[2] = 2;
   pIndices[3] = 3;
   
   m_IndexArray.Initialize(pIndices, 4, GL_TRIANGLE_STRIP);

   m_TextureHeightDivisor = 1.0f / 16.0f;
}

void CBitmapFont::BeginRendering()
{
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluOrtho2D(-1, 1, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(TEXTURE_2D, true);
   pGLSTATE->SetState(ALPHA_TEST, true);
   pGLSTATE->SetState(BLEND, true);
   //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glDepthMask(false);

   pGLSTATE->SetState(DEPTH_TEST, false);

   m_InterleavedArray.Enable();

   g_TextureManager.ActivateTexture(m_Texture);
}

void CBitmapFont::EndRendering()
{
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
      
   pGLSTATE->SetState(BLEND, false);
   pGLSTATE->SetState(ALPHA_TEST, false);
   glDepthMask(true);

   pGLSTATE->SetState(DEPTH_TEST, true);
}

void CBitmapFont::Print(const char* pText, const CVector2f* pPosition,
                        const Color4f* pColor, const CVector2f* pScale)
{
   /*
   if (pScale)
   {
      if (!m_Filtered)
      {
         if (pScale->x < 1.0f || pScale->y < 1.0f)
         {
            g_TextureManager.SetFilters(m_Texture, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
            m_Filtered = true;
         }
      }
      
      else if (pScale->x > 1.0f && pScale->y > 1.0f)
      {
         g_TextureManager.SetFilters(m_Texture, GL_NEAREST, GL_NEAREST);
         m_Filtered = false;
      }
   }
   else if (m_Filtered)
   {
      g_TextureManager.SetFilters(m_Texture, GL_NEAREST, GL_NEAREST);
      m_Filtered = false;
   }*/

   if (pColor)
      glColor4f(pColor->r, pColor->g, pColor->b, pColor->a);
   else
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   unsigned int Length = (unsigned int)strlen(pText);
   
   CVector2f NewPos = *pPosition;

   for (unsigned int i = 0; i < Length; ++i)
   {      
      Print(pText[i], &NewPos, pScale);
      
      NewPos.x += m_pCharWidths[pText[i]];             
   }
}

void CBitmapFont::Print(char Character, const CVector2f* pPosition,
                        const CVector2f* pScale)
{   
   // 16x16 characters, find the center x and upper y
   float TextureX = (Character & 15) * m_CharacterTextureWidth + m_CharacterTextureWidth * .5f;
   float TextureY = m_TextureHeightOffset - floorf(Character * m_TextureHeightDivisor) * m_CharacterTextureHeight
                    ;   

   // set up the texture coords
   m_pVertices[FONT_UPRIGHT].m_TextureCoord.x = TextureX + m_pTexWidths[Character];
   m_pVertices[FONT_UPRIGHT].m_TextureCoord.y = -m_PixelHeightFix + TextureY;
   
   m_pVertices[FONT_UPLEFT].m_TextureCoord.x = TextureX - m_pTexWidths[Character];
   m_pVertices[FONT_UPLEFT].m_TextureCoord.y = -m_PixelHeightFix + TextureY;    

   m_pVertices[FONT_LOWLEFT].m_TextureCoord.x = TextureX - m_pTexWidths[Character];
   m_pVertices[FONT_LOWLEFT].m_TextureCoord.y = TextureY - m_CharacterTextureHeight;

   m_pVertices[FONT_LOWRIGHT].m_TextureCoord.x = TextureX + m_pTexWidths[Character];
   m_pVertices[FONT_LOWRIGHT].m_TextureCoord.y = TextureY - m_CharacterTextureHeight;
   /*
   if (pScale)
   {
      // set up the position
      m_pVertices[FONT_UPRIGHT].m_Position.x = pPosition->x +
                                               (m_CharacterWidth * pScale->x);
      m_pVertices[FONT_UPRIGHT].m_Position.y = pPosition->y;
   
      m_pVertices[FONT_UPLEFT].m_Position.x = pPosition->x;
      m_pVertices[FONT_UPLEFT].m_Position.y = pPosition->y;    

      m_pVertices[FONT_LOWLEFT].m_Position.x = pPosition->x;
      m_pVertices[FONT_LOWLEFT].m_Position.y = pPosition->y -
                                               (m_CharacterHeight * pScale->y);

      m_pVertices[FONT_LOWRIGHT].m_Position.x = pPosition->x +
                                                (m_CharacterWidth * pScale->x);

      m_pVertices[FONT_LOWRIGHT].m_Position.y = pPosition->y -
                                                (m_CharacterHeight * pScale->y);
   }
   else*/
   {
      // set up the position
      m_pVertices[FONT_UPRIGHT].m_Position.x = pPosition->x + m_pCharWidths[Character];
      m_pVertices[FONT_UPRIGHT].m_Position.y = pPosition->y;
   
      m_pVertices[FONT_UPLEFT].m_Position.x = pPosition->x;
      m_pVertices[FONT_UPLEFT].m_Position.y = pPosition->y;    

      m_pVertices[FONT_LOWLEFT].m_Position.x = pPosition->x;
      m_pVertices[FONT_LOWLEFT].m_Position.y = pPosition->y - m_CharacterHeight;

      m_pVertices[FONT_LOWRIGHT].m_Position.x = pPosition->x + m_pCharWidths[Character];

      m_pVertices[FONT_LOWRIGHT].m_Position.y = pPosition->y - m_CharacterHeight;
   }

   // render
   m_IndexArray.Render();
}