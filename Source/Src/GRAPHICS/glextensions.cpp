#include "graphics/glextensions.h"
#include <wingdi.h>

#include "utils\mmgr.h"

static char* extList = (char*) glGetString(GL_EXTENSIONS);
PFNGLLOCKARRAYSEXTPROC glLockArraysEXT = 0;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT = 0;

void EnableLockArrays()
{
   if (extList && strstr(extList, "GL_EXT_compiled_vertex_array"))
   {
      glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC) wglGetProcAddress("glLockArraysEXT");
      glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC) wglGetProcAddress("glUnlockArraysEXT");
   }      
}