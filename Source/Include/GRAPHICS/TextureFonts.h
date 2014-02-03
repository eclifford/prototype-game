/// \file TextureFonts.h
/// CTextureFonts class declaration
/////////////////////////////////////////////////////////////////////////////

#pragma once

// Includes go here
#include <string>

#include "math3d\math3d.h"
#include "GRAPHICS\TextureManager.h"
#include "core\oglstatemanager.h"
using namespace std;

// Globals go here
extern CTextureManager        g_TextureManager;

/// Class used to store all game geometry information.
class CTextureFonts
{
   public:
      CTextureFonts();
      ~CTextureFonts();
      void  PrintText(string LineOfText);
      void  LoadTextureFont(string Path);
      void  PrintText(float Scale, int RowSize, string LineOfText);
      void  InitGL();
      void  EndGL();
      void  SetLocation(CVector2f vec);
      void  SetScale(float scale);
      void  SetRowSize(int rowsize);
      void  Shutdown();

   protected:

   private:
      string      m_LineOfText;
      CVector2f   m_Location;
      int         m_FontTextureIdx;
      float       m_Scale;
      int         m_RowSize;
};
