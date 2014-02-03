#include "graphics\skybox.h"
#include "graphics\texturemanager.h"
#include "graphics\vertext2fv3f.h"
#include "camera\camera.h"
#include "core\oglstatemanager.h"
#include "graphics\glextensions.h"

#include "utils\mmgr.h"

extern CTextureManager g_TextureManager;
extern CBaseCamera* g_Camera;
extern COpenGLStateManager* pGLSTATE;

CSkyBox::CSkyBox(const char *pBackImage, const char *pFrontImage, const char *pLeftImage,
              const char *pRightImage, const char *pTopImage, const char *pBottomImage,
              float HalfSize)
{
   Initialize(pBackImage, pFrontImage, pLeftImage, pRightImage, pTopImage, pBottomImage,
              HalfSize);
}

void CSkyBox::Initialize(const char *pBackImage, const char *pFrontImage,
                         const char *pLeftImage, const char *pRightImage, const char *pTopImage,
                         const char *pBottomImage, float HalfSize)
{
   m_pTextures[0] = g_TextureManager.LoadTexture(pBackImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE,
                                                 GL_CLAMP_TO_EDGE);
	m_pTextures[1] = g_TextureManager.LoadTexture(pFrontImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE,
                                                 GL_CLAMP_TO_EDGE);
	m_pTextures[2] = g_TextureManager.LoadTexture(pLeftImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE,
                                                 GL_CLAMP_TO_EDGE);
	m_pTextures[3] = g_TextureManager.LoadTexture(pRightImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE,
                                                 GL_CLAMP_TO_EDGE);
	m_pTextures[4] = g_TextureManager.LoadTexture(pTopImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE,
                                                 GL_CLAMP_TO_EDGE);
	m_pTextures[5] = g_TextureManager.LoadTexture(pBottomImage, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE,
                                                 GL_CLAMP_TO_EDGE);

   m_HalfSize = HalfSize;
}

void CSkyBox::Render()
{
   pGLSTATE->SetState(FOG, false);
   pGLSTATE->SetState(TEXTURE_2D, true);
	pGLSTATE->SetState(DEPTH_TEST, false);
   pGLSTATE->SetState(LIGHTING, false);
   glDepthMask(false);

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   float Height = -5.0f;

   CVector3f Position;
   g_Camera->GetPosition(&Position);      

   glPushMatrix();
      glTranslatef(Position.x, Position.y, Position.z);      		
      glRotatef(90.f, 0.f, 1.f, 0.f);
		// back side
		g_TextureManager.ActivateTexture(m_pTextures[0]);
		glBegin(GL_QUADS);				
			// lower right vertex
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(m_HalfSize, -m_HalfSize + Height, m_HalfSize);
			// lower left vertex
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-m_HalfSize, -m_HalfSize + Height, m_HalfSize);
			// upper left vertex
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-m_HalfSize, m_HalfSize + Height, m_HalfSize);
         // upper right vertex
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(m_HalfSize, m_HalfSize + Height, m_HalfSize);
		glEnd();

      // front side
		g_TextureManager.ActivateTexture(m_pTextures[1]);
		glBegin(GL_QUADS);			
			// upper left vertex
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-m_HalfSize, m_HalfSize + Height, -m_HalfSize);
			// lower left vertex
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-m_HalfSize, -m_HalfSize + Height, -m_HalfSize);         
			// lower right vertex
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(m_HalfSize, -m_HalfSize + Height, -m_HalfSize);
         // upper right vertex
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(m_HalfSize, m_HalfSize + Height, -m_HalfSize);
		glEnd();
		
			
		// left side
		g_TextureManager.ActivateTexture(m_pTextures[2]);
		glBegin(GL_QUADS);
			// lower right vertex
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-m_HalfSize, -m_HalfSize + Height, m_HalfSize);
			// lower left vertex
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(-m_HalfSize, -m_HalfSize + Height, -m_HalfSize);
			// upper left vertex
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(-m_HalfSize, m_HalfSize + Height, -m_HalfSize);
         // upper right vertex
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-m_HalfSize, m_HalfSize + Height, m_HalfSize);
		glEnd();
		
			
		// right side
		g_TextureManager.ActivateTexture(m_pTextures[3]);
		glBegin(GL_QUADS);
			// lower right vertex
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(m_HalfSize, -m_HalfSize + Height, -m_HalfSize);
			// lower left vertex
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(m_HalfSize, -m_HalfSize + Height, m_HalfSize);
			// upper left vertex
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(m_HalfSize, m_HalfSize + Height, m_HalfSize);
         // upper right vertex
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(m_HalfSize, m_HalfSize + Height, -m_HalfSize);
		glEnd();
			
		// top side (clockwise winding)
		g_TextureManager.ActivateTexture(m_pTextures[4]);
		glBegin(GL_QUADS);			
			// upper left vertex
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-m_HalfSize, m_HalfSize + Height, m_HalfSize);
			// lower left vertex
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-m_HalfSize, m_HalfSize + Height, -m_HalfSize);
			// lower right vertex
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(m_HalfSize, m_HalfSize + Height, -m_HalfSize);
         // upper right vertex
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(m_HalfSize, m_HalfSize + Height, m_HalfSize);
		glEnd();

      // bottom side
		g_TextureManager.ActivateTexture(m_pTextures[5]);
		glBegin(GL_QUADS);
			// lower right vertex
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(m_HalfSize, -m_HalfSize + Height, -m_HalfSize);
			// lower left vertex
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-m_HalfSize, -m_HalfSize + Height, -m_HalfSize);
			// upper left vertex
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-m_HalfSize, -m_HalfSize + Height, m_HalfSize);
         // upper right vertex
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(m_HalfSize, -m_HalfSize + Height, m_HalfSize);
		glEnd();

   glPopMatrix();

   glDepthMask(true);

   pGLSTATE->SetState(DEPTH_TEST, true);	
   pGLSTATE->SetState(FOG, true);
}