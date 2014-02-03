#include "graphics\fader.h"
#include "graphics\vertexv2f.h"
#include "core\oglwindow.h"
#include "core\oglstatemanager.h"

#include "utils\mmgr.h"

extern COpenGLStateManager* pGLSTATE;

void CFader::Initialize()
{
   float HalfWidth = 1.0f;
   float HalfHeight = 1.0f;

   CVertexV2F* pVertices = new CVertexV2F[4];
      
   // lower left vertex   
   pVertices[FADER_LOWLEFT].m_Position.x = -HalfWidth;
   pVertices[FADER_LOWLEFT].m_Position.y = -HalfHeight;

   // lower right vertex   
   pVertices[FADER_LOWRIGHT].m_Position.x = HalfWidth;
   pVertices[FADER_LOWRIGHT].m_Position.y = -HalfHeight;

   // upper left vertex   
   pVertices[FADER_UPLEFT].m_Position.x = -HalfWidth;
   pVertices[FADER_UPLEFT].m_Position.y = HalfHeight;

   // upper right vertex   
   pVertices[FADER_UPRIGHT].m_Position.x = HalfWidth;
   pVertices[FADER_UPRIGHT].m_Position.y = HalfHeight;

   m_InterleavedArray.Initialize(pVertices, 4, GL_V2F);

   // set up the indices
   unsigned int* pIndices = new unsigned int[4];
   
   pIndices[0] = 0;
   pIndices[1] = 1;
   pIndices[2] = 2;
   pIndices[3] = 3;
   
   m_IndexArray.Initialize(pIndices, 4, GL_TRIANGLE_STRIP);

   m_Color.r = 0.0f;
   m_Color.g = 0.0f;
   m_Color.b = 0.0f;

   m_FadeTime = 3.0f;

   m_FadeIn = true;

   m_Enabled = false;

   m_StartTime = 0.0f;
}

void CFader::Render()
{
   if (m_Enabled)
   {
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      gluOrtho2D(-1, 1, -1, 1);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();

      pGLSTATE->SetState(LIGHTING, false);
      pGLSTATE->SetState(TEXTURE_2D, false);
      pGLSTATE->SetState(BLEND, true);      
      glDepthMask(false);

      pGLSTATE->SetState(DEPTH_TEST, false);

      m_InterleavedArray.Enable();
      if (m_FadeIn)
      {
         glColor4f(m_Color.r, m_Color.g, m_Color.b,
                   1.0f - (g_time - m_StartTime) / m_FadeTime);         
      }
      else
      {
         glColor4f(m_Color.r, m_Color.g, m_Color.b,
                   (g_time - m_StartTime) / m_FadeTime);            
      }

      m_IndexArray.Render();

      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      
      pGLSTATE->SetState(BLEND, false);
      glDepthMask(true);

      pGLSTATE->SetState(DEPTH_TEST, true);
   }      
}
