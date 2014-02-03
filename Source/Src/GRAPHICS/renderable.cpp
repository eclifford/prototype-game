#include "graphics/renderable.h"
#include "graphics/glinclude.h"
#include "graphics/geometrymanager.h"

#include "utils\mmgr.h"

extern CGeometryManager g_GeometryManager;

void CFrameRenderObj::MoveForward(float Dist)
{
   m_Frame.MoveForward(Dist);   
}
void CFrameRenderObj::MoveRight(float Dist)
{
   m_Frame.MoveRight(Dist);   
}
void CFrameRenderObj::MoveUp(float Dist)
{
   m_Frame.MoveUp(Dist);
}

void CFrameRenderObj::RotateLocalX(float Angle)
{
   m_Frame.RotateLocalX(Angle);
}

void CFrameRenderObj::RotateLocalY(float Angle)
{
   m_Frame.RotateLocalY(Angle);
}

void CFrameRenderObj::RotateLocalZ(float Angle)
{
   m_Frame.RotateLocalZ(Angle);
}

void CFrameRenderObj::Render()
{
   glPushMatrix();
      
      float Matrix[16];
      m_Frame.GetOpenGLMatrix(Matrix);
      glMultMatrixf(Matrix);

      g_GeometryManager.RenderPart(m_GeometryIndex, 0);

   glPopMatrix();
}