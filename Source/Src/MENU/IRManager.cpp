#include "menu\irmanager.h"
#include "gamelogic\gameoptions.h"
#include "graphics\interfacerenderable.h"

#include "utils\mmgr.h"

extern CGameOptions g_GameOptions;

CIRManager::CIRManager()
{
   m_CurrentIndex = 0;
}

CIRManager::~CIRManager()
{
   Clear();
}

int CIRManager::IRLoad(std::string Path, Point3f* pPos)
{
   float width, height;

   FloatRect OrthoRect;
   OrthoRect.Max.x = 1.0f;
   OrthoRect.Max.y = 1.0f;
   OrthoRect.Min.x = -1.0f;
   OrthoRect.Min.y = -1.0f;

   std::string Suffix = Path.substr(Path.size() - 4, 4);

   Path.erase(Path.size() - 4, 4);

   switch(g_GameOptions.GetVideoMode())
   {
      case RES1600X1200:
      {
         Path += Suffix;
      }
         break;

      case RES1024X768:
      {
         Path = Path + "1024" + Suffix;
      }
         break;

      case RES800X600:
      {
         Path = Path + "800" + Suffix;
      }
         break;

      case RES640X480:
      {
         Path = Path + "640" + Suffix;
      }
         break;

      default:
         return 0;
   };

   width = (float)g_GameOptions.GetVideoWidth();
   height = (float)g_GameOptions.GetVideoHeight();

   CInterfaceRenderable* Temp = new CInterfaceRenderable;

   m_IRArray.push_back(Temp);

   m_IRArray[m_CurrentIndex]->Initialize(Path.c_str(), &OrthoRect, pPos, (unsigned int)width, (unsigned int)height);

   m_CurrentIndex ++;

   return m_CurrentIndex - 1;
}

void CIRManager::Clear()
{
   if (m_CurrentIndex == 0)
      return;

   for (int i = 0; i < m_CurrentIndex; i ++)
   {
      CInterfaceRenderable* Temp = m_IRArray.back();
      m_IRArray.pop_back();
      delete Temp;
   }

   m_CurrentIndex = 0;
   m_IRArray.clear();
}

void CIRManager::IRRender(unsigned int Index)
{
   if (Index < (unsigned int)m_CurrentIndex)
      m_IRArray[Index]->Render();
}

void CIRManager::SetPos(unsigned int Index, Point3f* pPos)
{
   m_IRArray[Index]->m_Frame.SetPosition(pPos);
}

FloatRect CIRManager::GetRect(int Index)
{
   FloatRect temp;

   m_IRArray[Index]->GetRect(&temp);

   return temp;
}

CVector3f CIRManager::GetPos(unsigned int Index)
{
   return *(m_IRArray[Index]->m_Frame.GetPosition());
}
