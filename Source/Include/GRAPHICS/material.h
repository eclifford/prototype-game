#pragma once

#include "color4f.h"

class CMaterial
{
   public:
      CMaterial() { SetDefault(); }
      void Activate();
      void SetDefault();
      bool IsTransparent()
      {
         if (m_Ambient.a < 1.0f || m_Diffuse.a < 1.0f || m_Specular.a < 1.0f)
            return true;
         else
            return false;
      }

      Color4f m_Ambient;
      Color4f m_Diffuse;
      Color4f m_Specular;
      unsigned int m_Shininess;

   private:
       
};