#pragma once

#include <windows.h>
#include <gl/GL.h>
#include "graphics\ptgfile.h"
#include "graphics\color3ub.h"

class CTextureManager
{
   public:
      CTextureManager() : m_FreeIndex(0) { }
      ~CTextureManager();
      void Initialize(unsigned int MaxTextures);
      unsigned int LoadTexture(const char* pFilePath, GLint MinFilter, GLint MagFilter,
                               GLint SWrap, GLint TWrap, unsigned int* pOldWidth = NULL,
                               unsigned int* pOldHeight = NULL, unsigned int* pNewWidth = NULL,
                               unsigned int* pNewHeight = NULL, bool Resize = true);
      void UnloadAll();
      void ActivateTexture(unsigned int Index);
      bool IsTransparent(unsigned int Index) { return m_pTextures[Index].m_Transparent;}

      void SetFilters(unsigned int Index, GLint MinFilter, GLint MagFilter);
      /*
      const char* GetFilePath(unsigned int Index)
      {
         return m_pTextures[Index].m_FilePath;
      }*/

   private:      
      struct Texture
      {
         char m_pFilePath[PT_FILE_PATH_LEN];
         unsigned int m_Handle;
         unsigned int m_OldWidth;
         unsigned int m_OldHeight;
         unsigned int m_NewWidth;
         unsigned int m_NewHeight;
         bool m_Transparent;   
      };

      unsigned int m_FreeIndex;
      Texture* m_pTextures;
      unsigned int m_MaxTextures;
};