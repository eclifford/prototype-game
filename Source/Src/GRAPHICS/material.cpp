#include "graphics\material.h"
#include "graphics\glinclude.h"

#include "utils\mmgr.h"

void CMaterial::Activate()
{
   glMateriali(GL_FRONT, GL_SHININESS, m_Shininess);
   glMaterialfv(GL_FRONT, GL_AMBIENT, (float *)&m_Ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, (float *)&m_Diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, (float *)&m_Specular);
}

void CMaterial::SetDefault()
{
   m_Ambient.a = 1.0f;
   m_Ambient.r = .3f;
   m_Ambient.g = .3f;
   m_Ambient.b = .3f;

   m_Diffuse.a = 1.0f;
   m_Diffuse.r = .85f;
   m_Diffuse.g = .85f;
   m_Diffuse.b = .85f;

   m_Specular.a = 1.0f;
   m_Specular.r = 0.1f;
   m_Specular.g = 0.1f;
   m_Specular.b = 0.1f;

   m_Shininess = 128;
}
