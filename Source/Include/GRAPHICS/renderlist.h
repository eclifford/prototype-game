// File: "renderlist.h"
// Author(s): Jesse Edmunds

#pragma once

#include "renderable.h"

// Used to sort render objects by state
class CRenderList
{
   public:
      // constructor to initialize members to default values
      CRenderList() : m_pBack(0), m_CurrentGeometry(-1), m_CurrentTexture(-2),
                      m_Transparent(0) { }
      
      // add a render object to the list 
      void AddRenderObj(CRenderObj* pRenderObj);
      
      // render all objects in the list
      void Render();
   private:             
      CRenderObj* m_pBack; // the back of the list

      int m_CurrentGeometry; // the current geometry index
      int m_CurrentTexture; // the current texture index
      bool m_Transparent; // whether we have blending enabled or not
      bool m_Lighting; // whether we have lighting enabled or not   
};