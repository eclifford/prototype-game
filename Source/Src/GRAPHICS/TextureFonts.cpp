#include "GRAPHICS\TextureFonts.h"
#include "core\oglstatemanager.h"

#include "utils\mmgr.h"


extern COpenGLStateManager* pGLSTATE;

//#define colwidth 16
//#define rowwidth 256

#define colwidth 64
#define rowwidth 1024


CTextureFonts::CTextureFonts()
{
   m_Location.x = 0.0f;
   m_Location.y = 0.0f;
   m_RowSize = 0;
   m_Scale = 0.0f;
}

CTextureFonts::~CTextureFonts()
{

}

void  CTextureFonts::PrintText(string LineOfText)
{
   /*

   if (m_RowSize < 1 || m_Scale <= 0)
      return;

   float       StartVertX;
   float       StartVertY;
   float       CurrVertX;
   float       CurrVertY;
   int         ASCIIVal;
   int         TexX;
   int         TexY;
   int         i, j;
   CVector2f   Verts[4];      // a quad

   InitGL();

   // need calc how many rows so that we can set start y to the half of num rows * scale

   StartVertX = (m_Location.x - ((m_RowSize/2.0f) * m_Scale));
   StartVertY = m_Location.y + ((LineOfText.size()/m_RowSize) * m_Scale/2.0f);
   CurrVertX = StartVertX;
   CurrVertY = StartVertY;

   pGLSTATE->SetState(LIGHTING, false);
//   pGLSTATE->SetState(LIGHT1, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_ONE, GL_ONE);
   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   g_TextureManager.ActivateTexture(m_FontTextureIdx);
   glBegin(GL_QUADS);

   for (i = 0; i < LineOfText.size(); i += m_RowSize)
   {
      
      for (j = i; j < i + m_RowSize; j++)
      {

         // first vert, upper left
         Verts[0].x = CurrVertX;
         Verts[0].y = CurrVertY;

         // second vert, lower left (going counter-clockwise)
         Verts[1].x = CurrVertX;
         Verts[1].y = CurrVertY - m_Scale;

         // third vert, lower right
         Verts[2].x = CurrVertX + m_Scale;
         Verts[2].y = CurrVertY - m_Scale;

         // fourth vert, upper right
         Verts[3].x = CurrVertX + m_Scale;
         Verts[3].y = CurrVertY;

         ASCIIVal = LineOfText[j];

         TexX = ASCIIVal%16;
         TexY = ASCIIVal/16;

         TexX *= colwidth;
         TexY *= colwidth;

         glTexCoord2f((float)TexX/rowwidth, 1.0f - (float)TexY/rowwidth);
         glVertex2f(Verts[0].x, Verts[0].y);

         glTexCoord2f((float)TexX/rowwidth, 1.0f - ((float)TexY + colwidth)/rowwidth);
         glVertex2f(Verts[1].x, Verts[1].y);

         glTexCoord2f(((float)TexX + colwidth)/rowwidth, 1.0f - ((float)TexY + colwidth)/rowwidth);
         glVertex2f(Verts[2].x, Verts[2].y);
        
         glTexCoord2f(((float)TexX + colwidth)/rowwidth, 1.0f - (float)TexY/rowwidth);
         glVertex2f(Verts[3].x, Verts[3].y);

         CurrVertX += m_Scale;  // next  start point = this start point + the size of font
      }
   
      CurrVertX = StartVertX;
      CurrVertY -= m_Scale;
   }

   glEnd();

   glMatrixMode(GL_PROJECTION);

   glPopMatrix();

   glMatrixMode(GL_MODELVIEW);
      pGLSTATE->SetState(BLEND, false);
   pGLSTATE->SetState(LIGHTING, true);
//   pGLSTATE->SetState(LIGHT1, true);

   */
}


void  CTextureFonts::PrintText(float Scale, int RowSize, string LineOfText)
{
   /*
   if (Scale <= 0 || RowSize < 1)
      return;

   float       StartVertX;
   float       StartVertY;
   float       CurrVertX;
   float       CurrVertY;
   int         ASCIIVal;
   int         TexX;
   int         TexY;
   int         i, j;
   CVector2f   Verts[4];      // a quad

   InitGL();

   m_Scale = Scale * 2.0f;
   m_RowSize = RowSize;

   // need calc how many rows so that we can set start y to the half of num rows * scale

   StartVertX = (m_Location.x - ((m_RowSize/2.0f) * m_Scale));
   StartVertY = m_Location.y + ((LineOfText.size()/m_RowSize) * m_Scale/2.0f);
   CurrVertX = StartVertX;
   CurrVertY = StartVertY;

     pGLSTATE->SetState(LIGHTING, false);
//   pGLSTATE->SetState(LIGHT1, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_ONE, GL_ONE);
   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   g_TextureManager.ActivateTexture(m_FontTextureIdx);
   glBegin(GL_QUADS);

   bool brk = false;

   for (i = 0; i < LineOfText.size(); i += m_RowSize)
   {
      
      for (j = i; j < i + m_RowSize; j++)
      {

         // first vert, upper left
         Verts[0].x = CurrVertX;
         Verts[0].y = CurrVertY;

         // second vert, lower left (going counter-clockwise)
         Verts[1].x = CurrVertX;
         Verts[1].y = CurrVertY - m_Scale;

         // third vert, lower right
         Verts[2].x = CurrVertX + m_Scale;
         Verts[2].y = CurrVertY - m_Scale;

         // fourth vert, upper right
         Verts[3].x = CurrVertX + m_Scale;
         Verts[3].y = CurrVertY;

         ASCIIVal = LineOfText[j];

         if (LineOfText[j] == '\0')
         {
            brk = true;
            break;
         }

         TexX = ASCIIVal%16;
         TexY = ASCIIVal/16;

         TexX *= colwidth;
         TexY *= colwidth;

         glTexCoord2f((float)TexX/rowwidth, 1.0f - (float)TexY/rowwidth);
         glVertex2f(Verts[0].x, Verts[0].y);

         glTexCoord2f((float)TexX/rowwidth, 1.0f - ((float)TexY + colwidth)/rowwidth);
         glVertex2f(Verts[1].x, Verts[1].y);

         glTexCoord2f(((float)TexX + colwidth)/rowwidth, 1.0f - ((float)TexY + colwidth)/rowwidth);
         glVertex2f(Verts[2].x, Verts[2].y);
        
         glTexCoord2f(((float)TexX + colwidth)/rowwidth, 1.0f - (float)TexY/rowwidth);
         glVertex2f(Verts[3].x, Verts[3].y);

         CurrVertX += m_Scale;  // next  start point = this start point + the size of font
      }
   
      if (brk == true)
         break;

      CurrVertX = StartVertX;
      CurrVertY -= m_Scale;
   }

   glEnd();

   glMatrixMode(GL_PROJECTION);

   glPopMatrix();

   glMatrixMode(GL_MODELVIEW);
      pGLSTATE->SetState(BLEND, false);
   pGLSTATE->SetState(LIGHTING, true);
//   pGLSTATE->SetState(LIGHT1, true);

   */
}


void  CTextureFonts::LoadTextureFont(string Path)
{
   /*
   string LongPath = "resources\\images\\";

   LongPath += Path;

   m_FontTextureIdx = g_TextureManager.LoadTexture(LongPath.c_str(), GL_NEAREST, GL_NEAREST,
                                                    GL_CLAMP, GL_CLAMP);
   */
}

void CTextureFonts::Shutdown()
{

}

void CTextureFonts::InitGL()
{
  /*
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   gluOrtho2D(-1, 1, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   */
}

void CTextureFonts::SetLocation(CVector2f vec)
{
   m_Location = vec;
}

void  CTextureFonts::SetScale(float scale)
{
   m_Scale = scale;
}


void  CTextureFonts::SetRowSize(int rowsize)
{
   m_RowSize = rowsize;
}