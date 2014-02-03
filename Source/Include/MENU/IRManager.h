/// \file Interface.h
/// IRManager header file
//////////////////////////////////////////////////////////////////////////
#pragma once

/// An header for a menu object

#include <string>

#include <vector>
#include "math3d/primitives.h"

// forward declarations
class CInterfaceRenderable;

class CIRManager
{
   private:
	std::vector <CInterfaceRenderable*> m_IRArray;
   int m_CurrentIndex;

   public:
      CIRManager();
      ~CIRManager();
      int IRLoad(std::string pPath, Point3f* pPos);
      void IRRender(unsigned int Index);
      void SetPos(unsigned int Index, Point3f* pPos);
      void Clear();
      FloatRect GetRect(int Index);
      CVector3f GetPos(unsigned int Index);
};
