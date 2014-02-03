#include "graphics/texturemanager.h"
#include <gl/GLU.h>
#include "graphics/nexgenipl/nexgenipl.h"
#include "graphics\glextensions.h"
#include "math3d\miscmath.h"

#include "utils\mmgr.h"


CTextureManager::~CTextureManager()
{
   UnloadAll();
}

void CTextureManager::Initialize(unsigned int MaxTextures)
{
   m_pTextures = new Texture[MaxTextures];
   memset(m_pTextures, 0, sizeof(Texture) * MaxTextures);

   m_MaxTextures = MaxTextures;
}

void CTextureManager::ActivateTexture(unsigned int Index)
{
   glBindTexture(GL_TEXTURE_2D, m_pTextures[Index].m_Handle);
}

unsigned int CTextureManager::LoadTexture(const char* pFilePath, GLint MinFilter,
                                          GLint MagFilter, GLint SWrap, GLint TWrap,
                                          unsigned int* pOldWidth,
                                          unsigned int* pOldHeight,
                                          unsigned int* pNewWidth,
                                          unsigned int* pNewHeight,
                                          bool Resize)
{
   if (m_FreeIndex == m_MaxTextures)
      bool debug = true;

   // make sure this texture hasn't already been loaded
   for (unsigned int i = 0; i < m_FreeIndex; ++i)
   {
      if (!stricmp(m_pTextures[i].m_pFilePath, pFilePath))
      {
         if (pNewHeight)
            *pNewHeight = m_pTextures[i].m_NewHeight;

         if (pNewWidth)
            *pNewWidth = m_pTextures[i].m_NewWidth;

         if (pOldHeight)
            *pOldHeight = m_pTextures[i].m_OldHeight;

         if (pOldWidth)
            *pOldWidth = m_pTextures[i].m_OldWidth;

         return i;
      }
   }

   // store the file path
   strcpy(m_pTextures[m_FreeIndex].m_pFilePath, pFilePath);   

   BTCImageObject ImageObject;
   if (!ImageObject.Load(pFilePath))
   {
      MessageBox(NULL, pFilePath, "Error loading texture.", MB_OK);
      return 0;
   }

   BTCImageData* pImageData = ImageObject.GetObjectDataPtr();
   pImageData->Flip();

   // if this image isn't a power of 2, resize it
   m_pTextures[m_FreeIndex].m_OldWidth = pImageData->GetWidth();
   m_pTextures[m_FreeIndex].m_OldHeight = pImageData->GetHeight();

   m_pTextures[m_FreeIndex].m_NewWidth = ClosestLargerPowerOfTwo(
                                         m_pTextures[m_FreeIndex].m_OldWidth);

   m_pTextures[m_FreeIndex].m_NewHeight = ClosestLargerPowerOfTwo(
                                          m_pTextures[m_FreeIndex].m_OldHeight);

   if (pNewHeight)
      *pNewHeight = m_pTextures[m_FreeIndex].m_NewHeight;

   if (pNewWidth)
      *pNewWidth = m_pTextures[m_FreeIndex].m_NewWidth;

   if (pOldHeight)
      *pOldHeight = m_pTextures[m_FreeIndex].m_OldHeight;

   if (pOldWidth)
      *pOldWidth = m_pTextures[m_FreeIndex].m_OldWidth;

   BTCImageObject ResizedImage;
   if (Resize)
   {
      if (m_pTextures[m_FreeIndex].m_NewWidth != m_pTextures[m_FreeIndex].m_OldWidth ||
          m_pTextures[m_FreeIndex].m_NewHeight != m_pTextures[m_FreeIndex].m_OldHeight)
      {
         // create a new image with the new width and height
         ResizedImage.GetObjectDataPtr()->Create(m_pTextures[m_FreeIndex].m_NewWidth,
                                                 m_pTextures[m_FreeIndex].m_NewHeight,
                                                 pImageData->GetBitsPerPixel());   
   
         // fill the image with white
         BTCOLORREF Color = 0xFFFFFFFF;
         ResizedImage.GetObjectDataPtr()->Fill(0, 0,
                                               m_pTextures[m_FreeIndex].m_NewWidth,
                                               m_pTextures[m_FreeIndex].m_NewHeight,
                                               Color);

         // copy the old image into the new image
         ResizedImage.GetObjectDataPtr()->Combine(pImageData, BTCImageData::Normal);
   
         // set our image data pointer to the new one
         pImageData = ResizedImage.GetObjectDataPtr();
      }
   }

   unsigned int ColorEntries;
   if (pImageData->GetBytesPerPixel() == 3)
   {
      m_pTextures[m_FreeIndex].m_Transparent = false;
      ColorEntries = GL_BGR_EXT;
   }
   else if (pImageData->GetBytesPerPixel() == 4)
   {
      m_pTextures[m_FreeIndex].m_Transparent = true;   
      ColorEntries = GL_BGRA_EXT;
   }

   glGenTextures(1, &m_pTextures[m_FreeIndex].m_Handle);

   glBindTexture(GL_TEXTURE_2D, m_pTextures[m_FreeIndex].m_Handle);

	if (MinFilter == GL_LINEAR_MIPMAP_LINEAR || MinFilter == GL_LINEAR_MIPMAP_NEAREST)
	{	
      gluBuild2DMipmaps(GL_TEXTURE_2D, pImageData->GetBytesPerPixel(), pImageData->GetWidth(),
                        pImageData->GetHeight(), ColorEntries, GL_UNSIGNED_BYTE,
                        pImageData->GetBits());
	
      
      //GLfloat largest_supported_anisotropy;
      //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest_supported_anisotropy);
      //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest_supported_anisotropy);   
      
   }
   else
      glTexImage2D(GL_TEXTURE_2D, 0, pImageData->GetBytesPerPixel(), pImageData->GetWidth(),
                   pImageData->GetHeight(), 0, ColorEntries, GL_UNSIGNED_BYTE,
                   pImageData->GetBits());

	// Set the filters and wrap modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
	   

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, SWrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TWrap);

   return m_FreeIndex++;
}

void CTextureManager::SetFilters(unsigned int Index, GLint MinFilter, GLint MagFilter)
{
   ActivateTexture(Index);
   
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);	   
}

void CTextureManager::UnloadAll()
{   
   for (unsigned int i = 0; i < m_FreeIndex; i++)
   {
      glDeleteTextures(1, &m_pTextures[i].m_Handle);
   }

   delete [] m_pTextures;   

   m_pTextures = 0;

   m_FreeIndex = 0;

   m_MaxTextures = 0;
}