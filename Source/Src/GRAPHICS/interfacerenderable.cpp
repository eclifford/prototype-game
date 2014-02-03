#include "graphics\interfacerenderable.h"
#include "graphics\texturemanager.h"
#include "core\oglwindow.h"

#include "utils\mmgr.h"

extern CTextureManager g_TextureManager;
extern COpenGLWindow* pGLWIN;

void CInterfaceRenderable::Initialize(const char* pTextureFilePath, const FloatRect* pOrthoRect,
                                      const Point3f* pPosition, unsigned int DefaultWidth,
                                      unsigned int DefaultHeight)
{
   // load in the texture and get its old and newly sized height
   unsigned int OldWidth;
   unsigned int OldHeight;

   unsigned int NewWidth;
   unsigned int NewHeight;

   if (DefaultWidth != (unsigned int)pGLWIN->GetScreenWidth() || DefaultHeight != (unsigned int)pGLWIN->GetScreenHeight())
      m_Texture = g_TextureManager.LoadTexture(pTextureFilePath, GL_LINEAR, GL_LINEAR, GL_CLAMP,
                                               GL_CLAMP, &OldWidth, &OldHeight, &NewWidth,
                                               &NewHeight);
   else
      m_Texture = g_TextureManager.LoadTexture(pTextureFilePath, GL_NEAREST, GL_NEAREST, GL_CLAMP,
                                               GL_CLAMP, &OldWidth, &OldHeight, &NewWidth,
                                               &NewHeight);

   // start creating the vertices
   unsigned int NumVertices = 4;
   m_pVertices = new CVertexT2FC4UBV3F[NumVertices];
   
   // calculate the half width and half height
   
   float HalfWidth = .5f * (float)OldWidth * ((float)pGLWIN->GetScreenWidth() / (float)DefaultWidth) * 
                      ((pOrthoRect->Max.x - pOrthoRect->Min.x) / (float)pGLWIN->GetScreenWidth());
   float HalfHeight = .5f * (float)OldHeight * ((float)pGLWIN->GetScreenHeight() / (float)DefaultHeight) * 
                      ((pOrthoRect->Max.y - pOrthoRect->Min.y) / (float)pGLWIN->GetScreenHeight());
   
   
   // calculate width and height ratios for texture coords
   float OldToNewWidthRatio = (float)OldWidth / (float)NewWidth;
   float OldToNewHeightRatio = (float)OldHeight / (float)NewHeight;

   if (OldWidth != NewWidth || OldHeight != NewHeight)
   {
      // calculate pixel fix offsets
      OldToNewWidthRatio -= (1.0f / NewWidth * .25f);
      OldToNewHeightRatio -= (1.0f / NewHeight * .25f);      
   }

   // lower left vertex   
   m_pVertices[INTERFACE_LOWLEFT].m_Color.r = 255;
   m_pVertices[INTERFACE_LOWLEFT].m_Color.g = 255;
   m_pVertices[INTERFACE_LOWLEFT].m_Color.b = 255;
   m_pVertices[INTERFACE_LOWLEFT].m_Color.a = 255;

   m_pVertices[INTERFACE_LOWLEFT].m_Position.x = -HalfWidth;
   m_pVertices[INTERFACE_LOWLEFT].m_Position.y = -HalfHeight;
   m_pVertices[INTERFACE_LOWLEFT].m_Position.z = 0;

   m_pVertices[INTERFACE_LOWLEFT].m_TextureCoord.x = 0;
   m_pVertices[INTERFACE_LOWLEFT].m_TextureCoord.y = 0;

   // lower right vertex   
   m_pVertices[INTERFACE_LOWRIGHT].m_Color.r = 255;
   m_pVertices[INTERFACE_LOWRIGHT].m_Color.g = 255;
   m_pVertices[INTERFACE_LOWRIGHT].m_Color.b = 255;
   m_pVertices[INTERFACE_LOWRIGHT].m_Color.a = 255;

   m_pVertices[INTERFACE_LOWRIGHT].m_Position.x = HalfWidth;
   m_pVertices[INTERFACE_LOWRIGHT].m_Position.y = -HalfHeight;
   m_pVertices[INTERFACE_LOWRIGHT].m_Position.z = 0;

   m_pVertices[INTERFACE_LOWRIGHT].m_TextureCoord.x = OldToNewWidthRatio;
   m_pVertices[INTERFACE_LOWRIGHT].m_TextureCoord.y = 0;

   // upper left vertex   
   m_pVertices[INTERFACE_UPLEFT].m_Color.r = 255;
   m_pVertices[INTERFACE_UPLEFT].m_Color.g = 255;
   m_pVertices[INTERFACE_UPLEFT].m_Color.b = 255;
   m_pVertices[INTERFACE_UPLEFT].m_Color.a = 255;

   m_pVertices[INTERFACE_UPLEFT].m_Position.x = -HalfWidth;
   m_pVertices[INTERFACE_UPLEFT].m_Position.y = HalfHeight;
   m_pVertices[INTERFACE_UPLEFT].m_Position.z = 0;

   m_pVertices[INTERFACE_UPLEFT].m_TextureCoord.x = 0;
   m_pVertices[INTERFACE_UPLEFT].m_TextureCoord.y = OldToNewHeightRatio;

   // upper right vertex   
   m_pVertices[INTERFACE_UPRIGHT].m_Color.r = 255;
   m_pVertices[INTERFACE_UPRIGHT].m_Color.g = 255;
   m_pVertices[INTERFACE_UPRIGHT].m_Color.b = 255;
   m_pVertices[INTERFACE_UPRIGHT].m_Color.a = 255;

   m_pVertices[INTERFACE_UPRIGHT].m_Position.x = HalfWidth;
   m_pVertices[INTERFACE_UPRIGHT].m_Position.y = HalfHeight;
   m_pVertices[INTERFACE_UPRIGHT].m_Position.z = 0;

   m_pVertices[INTERFACE_UPRIGHT].m_TextureCoord.x = OldToNewWidthRatio;
   m_pVertices[INTERFACE_UPRIGHT].m_TextureCoord.y = OldToNewHeightRatio;

   m_InterleavedArray.Initialize(m_pVertices, NumVertices, GL_T2F_C4UB_V3F);
   
   memcpy(&m_OriginalVerts, m_pVertices, sizeof(CVertexT2FC4UBV3F) * NumVertices);

   // set up the indices
   unsigned int* pIndices = new unsigned int[NumVertices];
   
   pIndices[0] = 0;
   pIndices[1] = 1;
   pIndices[2] = 2;
   pIndices[3] = 3;
   
   m_IndexArray.Initialize(pIndices, NumVertices, GL_TRIANGLE_STRIP);    

   // set up the frame
   m_Frame.SetPosition(pPosition);
}

void CInterfaceRenderable::Render()
{
   glPushMatrix();
      g_TextureManager.ActivateTexture(m_Texture);
      m_InterleavedArray.Enable();

      float Matrix[16];
      m_Frame.GetOpenGLMatrix(Matrix);
      glMultMatrixf(Matrix);
      m_IndexArray.Render();
   glPopMatrix();
}

void CInterfaceRenderable::CropRight(float CropRatio)
{
    
    m_pVertices[INTERFACE_LOWRIGHT].m_Position.x =
    m_OriginalVerts[INTERFACE_LOWLEFT].m_Position.x +
    (m_OriginalVerts[INTERFACE_LOWRIGHT].m_Position.x -
    m_OriginalVerts[INTERFACE_LOWLEFT].m_Position.x) * CropRatio;
    
    m_pVertices[INTERFACE_UPRIGHT].m_Position.x =
    m_pVertices[INTERFACE_LOWRIGHT].m_Position.x;   
    
    m_pVertices[INTERFACE_LOWRIGHT].m_TextureCoord.x =
    m_OriginalVerts[INTERFACE_LOWRIGHT].m_TextureCoord.x * CropRatio;
    
    m_pVertices[INTERFACE_UPRIGHT].m_TextureCoord.x =
    m_pVertices[INTERFACE_LOWRIGHT].m_TextureCoord.x;  
}

void CInterfaceRenderable::CropTop(float CropRatio)
{
    m_pVertices[INTERFACE_UPLEFT].m_Position.y =
    m_OriginalVerts[INTERFACE_LOWLEFT].m_Position.y +
    (m_OriginalVerts[INTERFACE_UPLEFT].m_Position.y -
    m_OriginalVerts[INTERFACE_LOWLEFT].m_Position.y) * CropRatio;
    
    m_pVertices[INTERFACE_UPRIGHT].m_Position.y =
    m_pVertices[INTERFACE_UPLEFT].m_Position.y;
                
    m_pVertices[INTERFACE_UPLEFT].m_TextureCoord.y =
    m_OriginalVerts[INTERFACE_UPLEFT].m_TextureCoord.y * CropRatio;
    
    m_pVertices[INTERFACE_UPRIGHT].m_TextureCoord.y =
    m_pVertices[INTERFACE_UPLEFT].m_TextureCoord.y;
}

void CInterfaceRenderable::CropBottom(float CropRatio)
{
    m_pVertices[INTERFACE_LOWLEFT].m_Position.y =
    m_OriginalVerts[INTERFACE_UPLEFT].m_Position.y -
    (m_OriginalVerts[INTERFACE_UPLEFT].m_Position.y -
    m_OriginalVerts[INTERFACE_LOWLEFT].m_Position.y) * CropRatio;
    
    m_pVertices[INTERFACE_LOWRIGHT].m_Position.y =
    m_pVertices[INTERFACE_LOWLEFT].m_Position.y;
                
    m_pVertices[INTERFACE_LOWLEFT].m_TextureCoord.y =
    m_OriginalVerts[INTERFACE_UPLEFT].m_TextureCoord.y -
    m_OriginalVerts[INTERFACE_UPLEFT].m_TextureCoord.y * CropRatio;
    
    m_pVertices[INTERFACE_LOWRIGHT].m_TextureCoord.y =
    m_pVertices[INTERFACE_LOWLEFT].m_TextureCoord.y;
}

void CInterfaceRenderable::CropLeft(float CropRatio)
{
    m_pVertices[INTERFACE_LOWLEFT].m_Position.x =
    m_OriginalVerts[INTERFACE_LOWRIGHT].m_Position.x -
    (m_OriginalVerts[INTERFACE_LOWRIGHT].m_Position.x -
    m_OriginalVerts[INTERFACE_LOWLEFT].m_Position.x) * CropRatio;
    
    m_pVertices[INTERFACE_UPLEFT].m_Position.x =
    m_pVertices[INTERFACE_LOWLEFT].m_Position.x;   
    
    m_pVertices[INTERFACE_LOWLEFT].m_TextureCoord.x =
    m_OriginalVerts[INTERFACE_LOWRIGHT].m_TextureCoord.x -
    m_OriginalVerts[INTERFACE_LOWRIGHT].m_TextureCoord.x * CropRatio;
    
    m_pVertices[INTERFACE_UPLEFT].m_TextureCoord.x =
    m_pVertices[INTERFACE_LOWLEFT].m_TextureCoord.x;
}

void CInterfaceRenderable::SetColor(const Color4UB* pColor, EInterfaceCorner Corner)
{
    m_pVertices[Corner].m_Color = *pColor;
}

void CInterfaceRenderable::GetRect(FloatRect* pRect)
{
    Point3f Position;
    m_Frame.GetPosition(&Position);
    pRect->Max.x = m_pVertices[INTERFACE_UPRIGHT].m_Position.x + Position.x;
    pRect->Max.y = m_pVertices[INTERFACE_UPRIGHT].m_Position.y + Position.y;
    pRect->Min.x = m_pVertices[INTERFACE_LOWLEFT].m_Position.x + Position.x;
    pRect->Min.y = m_pVertices[INTERFACE_LOWLEFT].m_Position.y + Position.y;
}

void CInterfaceRenderable::SetColor(const Color4UB* pColor)
{
    for (unsigned int i = 0; i < 4; i++)
    {
        m_pVertices[i].m_Color = *pColor;
    }   
}

void CInterfaceRenderable::Shutdown()
{
    m_InterleavedArray.Unload();    
    m_IndexArray.Unload();
}

CInterfaceRenderable::CInterfaceRenderable()
{
    m_pVertices = 0;
}

CInterfaceRenderable::~CInterfaceRenderable()
{
    Shutdown();   
}