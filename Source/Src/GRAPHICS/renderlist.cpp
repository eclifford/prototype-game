// File: "renderlist.cpp"
// Author(s): Jesse Edmunds

#include "graphics/renderlist.h"
#include "graphics/geometrymanager.h"
#include "graphics/texturemanager.h"
#include "core\oglstatemanager.h"

#include "utils\mmgr.h"

// external globals
extern CGeometryManager g_GeometryManager;
extern CTextureManager g_TextureManager;
extern COpenGLStateManager* pGLSTATE;

// add a render object to the list
void CRenderList::AddRenderObj(CRenderObj* pRenderObj)
{
   // add it to the list based on its priority
   // sort by alpha
   // sort by depth
   // sort by textures   

   // if the list is empty
   if (!m_pBack)
   {
      // make this render obj the first one
      m_pBack = pRenderObj;
      pRenderObj->m_pNext = 0;
   }
   // if the list isn't empty
   else
   {
      CRenderObj* pCurr = m_pBack;   

      // if its transparent
      if (pRenderObj->IsTransparent())
      {
         pRenderObj->CalcDist();
         float Dist = pRenderObj->GetDist();

         // check to see if the first one is transparent
         if (pCurr->IsTransparent())
         {
            if (Dist >= pCurr->GetDist())
            {
               // insert it at the front
               pRenderObj->m_pNext = m_pBack;
               m_pBack = pRenderObj;
               return;
            }   
         }

         while (pCurr->m_pNext)
         {
            // if we've found another transparent one
            if (pCurr->m_pNext->IsTransparent())
            {
               // sort by depth, back to front
               if (Dist >= pCurr->m_pNext->GetDist())
               {
                  // insert it here
                  pRenderObj->m_pNext = pCurr->m_pNext;
                  pCurr->m_pNext = pRenderObj;
                  return;
               }   
            }
         
            pCurr = pCurr->m_pNext;   
         }

         // we've reached the end of the list so just put it here
         pCurr->m_pNext = pRenderObj;
         pRenderObj->m_pNext = 0;
      }
      // if its not transparent
      else
      {
         // sort by texture
         unsigned int TextureIndex = pRenderObj->GetTextureIndex();
  
         // if the first one is transparent then add it to the front and exit
         if (pCurr->IsTransparent())
         {
            pRenderObj->m_pNext = m_pBack;
            m_pBack = pRenderObj;
            return;
         }

         while (pCurr->m_pNext)
         {
            // if we found matching textures
            if (pCurr->m_pNext->GetTextureIndex() == TextureIndex)
            {
               // insert it here
               pRenderObj->m_pNext = pCurr->m_pNext;
               pCurr->m_pNext = pRenderObj;
               return;
            }
            // if we find the transparent ones
            else if (pCurr->m_pNext->IsTransparent())
            {
               // insert before the transparent ones
               pRenderObj->m_pNext = pCurr->m_pNext;
               pCurr->m_pNext = pRenderObj;
               return;
            }

            pCurr = pCurr->m_pNext;
         }

         // we've reached the end of the list so just put it here
         pCurr->m_pNext = pRenderObj;
         pRenderObj->m_pNext = 0;
      }  
   }
}

// render all objects in the list
void CRenderList::Render()
{
   // while the list isn't empty
   while (m_pBack)
   {
      // check the current geometry index
      if ((int)m_pBack->GetGeometryIndex() != m_CurrentGeometry)
      {
         g_GeometryManager.Enable(m_pBack->GetGeometryIndex());
         m_CurrentGeometry = m_pBack->GetGeometryIndex();
      }

      // check the current texture index
      if (m_pBack->GetTextureIndex() != m_CurrentTexture)
      {
         if (m_pBack->GetTextureIndex() == -1)
         {
            pGLSTATE->SetState(TEXTURE_2D, false);
            m_CurrentTexture = -1;
         }
         else
         {
            
            pGLSTATE->SetState(TEXTURE_2D, true);
            
            g_TextureManager.ActivateTexture(m_pBack->GetTextureIndex());
            m_CurrentTexture = m_pBack->GetTextureIndex();
         }
      }

      // check for transparency
      if (!m_Transparent)
      {
         if (m_pBack->IsTransparent())
         {
            pGLSTATE->SetState(BLEND, true);
            pGLSTATE->SetState(ALPHA_TEST, true);
            m_Transparent = true;
         } 
      }
      else
      {
         if (!m_pBack->IsTransparent())
         {
            pGLSTATE->SetState(BLEND, false);
            pGLSTATE->SetState(ALPHA_TEST, false);
            m_Transparent = false;
         }
      }
      
      // check for lighting
      if (!m_Lighting && m_pBack->HasLighting())
      {
         pGLSTATE->SetState(LIGHTING, true);
         m_Lighting = true;
      }
      else if (!m_pBack->HasLighting())
      {
         pGLSTATE->SetState(LIGHTING, false);
         m_Lighting = false;
      }
      
      // render it and go to the next one
      m_pBack->Render();
      m_pBack = m_pBack->m_pNext;
   }

   // reset all states
   m_CurrentGeometry = -1;
   m_CurrentTexture = -2;
   m_Transparent = false;
   m_Lighting = true;
}