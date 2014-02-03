
#include "menu\Interface.h"
#include "input\inputmanager.h"
#include "menu\irmanager.h"
#include "math3d\intersection.h"

#include "utils\mmgr.h"


// globals here
extern float g_time;
extern CIRManager g_MenuGraphics;

CGameInterface::CGameInterface()
{
   Init();
}


CGameInterface::~CGameInterface()
{

   CGameInterfaceObject* Temp;

   while (!m_MenuItems.empty())
   {
      Temp = m_MenuItems.back();
      m_MenuItems.pop_back();
      
      if (Temp)
         delete Temp;
   }
}



void CGameInterface::Init()
{
   m_CurrentObject = 0;
   m_TotalBackgrounds = 0;
}



bool  CGameInterface::SetCurrentItem(int ItemId)
{
   // iterate through list and find object with ItemId
   // set this m_CurrentObject to this object
   bool end = false;

   m_ItemIterator = m_MenuItems.begin();

   while (m_ItemIterator != m_MenuItems.end())
   {
      if ((*m_ItemIterator)->GetId() == ItemId)
      {
         end = true;
         break;
      }

      m_ItemIterator ++;
   }

   m_CurrentObject = *m_ItemIterator;

   return end;
}


void  CGameInterface::SetItemTexture(int TextureId)
{
   m_CurrentObject->SetTexture(TextureId);
}


void  CGameInterface::SetActiveSelectedTexture(int TextureId)
{
   m_CurrentObject->SetActiveSelectedTexture(TextureId);
}


void  CGameInterface::SetInactiveSelectedTexture(int TextureId)
{
   m_CurrentObject->SetInactiveSelectedTexture(TextureId);
}


void  CGameInterface::SetActiveUnSelectedTexture(int TextureId)
{
   m_CurrentObject->SetActiveUnSelectedTexture(TextureId);
}


void  CGameInterface::SetInactiveUnSelectedTexture(int TextureId)
{
   m_CurrentObject->SetInactiveUnSelectedTexture(TextureId);
}


void  CGameInterface::SetItemFuncAble(bool FuncAble)
{
   m_CurrentObject->SetFuncAble(FuncAble);
}


void  CGameInterface::SetItemFuncAble(void (*pOnClickFunc)(CGameOptions* pOptions))
{
   m_CurrentObject->SetFuncAble(pOnClickFunc);
}



void  CGameInterface::SetItemLoc(CVector2f Loc)
{
   m_CurrentObject->SetLocation(Loc);
}


int CGameInterface::MakeItem()
{
   CGameInterfaceObject* Temp = new CGameInterfaceObject;

   Temp->SetId((int)m_MenuItems.size());

   m_MenuItems.push_back(Temp);

   m_CurrentObject = Temp;

   return m_CurrentObject->GetId();
}


void CGameInterface::SetItemId(int Id)
{
   m_CurrentObject->SetId(Id);
}


void CGameInterface::SetId(int Id)
{
   m_Id = Id;
}


int CGameInterface::GetId()
{
   return m_Id;
}


int CGameInterface::GetItemId()
{
   return m_CurrentObject->GetId();
}


void CGameInterface::SetItemPlaneId(int PlaneId)
{
   m_CurrentObject->SetPlaneId(PlaneId);
}

/*
bool CGameInterface::GotoNextItem()
{
   if (m_CurrentObject == m_MenuItems.back())
      return false;
   else
   {
      m_ItemIterator ++;
      m_CurrentObject = *m_ItemIterator;
      return true;
   }
}


void CGameInterface::GotoFirstItem()
{
   m_CurrentObject = m_MenuItems.front();
   m_ItemIterator = m_MenuItems.begin();
}

*/

void CGameInterface::Render()
{
   if (m_MenuItems.empty())
      return;

   StartInterfaceRender();

// go through background images and render here
// for # of images ...
// g_MenuGraphics.IRRender(# of image);
   if (m_TotalBackgrounds > 0)
   {
      for (unsigned int i = 0; i < m_TotalBackgrounds; i++)
         g_MenuGraphics.IRRender(m_Backgrounds[i]);
   }

   m_ItemIterator = m_MenuItems.begin();

   while (m_ItemIterator != m_MenuItems.end())
   {
      (*m_ItemIterator)->Render();
      m_ItemIterator ++;
   }

   EndInterfaceRender();

}


void CGameInterface::Update()
{
   if (m_MenuItems.empty())
      return;

   m_ItemIterator = m_MenuItems.begin();

   while (m_ItemIterator != m_MenuItems.end())
   {
      (*m_ItemIterator)->Update();
      m_ItemIterator ++;
   }

}


void CGameInterface::MakeItemUnclicked(int ItemId)
{
   SetCurrentItem(ItemId);
   m_CurrentObject->MakeUnclicked();
}

void  CGameInterface::SetItemHeight(float Height)
{
   m_CurrentObject->SetHeight(Height);
}


void  CGameInterface::SetItemWidth(float Width)
{
   m_CurrentObject->SetWidth(Width);
}

void CGameInterface::StartInterfaceRender()
{
   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}


void CGameInterface::EndInterfaceRender()
{

}



bool CGameInterface::GetItemClicked(int Id)
{
   SetCurrentItem(Id);
   return m_CurrentObject->GetClicked();
}

void  CGameInterface::MakeItemSelected(int ItemId)
{
   SetCurrentItem(ItemId);
   m_CurrentObject->MakeSelected();
}


void  CGameInterface::MakeAllUnSelected()
{
   if (m_MenuItems.empty())
      return;

   m_ItemIterator2 = m_MenuItems.begin();

   while (m_ItemIterator2 != m_MenuItems.end())
   {
      (*m_ItemIterator2)->MakeUnselected();
      m_ItemIterator2 ++;
   }

}


void CGameInterface::GetInput()
{

   CInputManager::GetInstance()->PollKeyboard();

   if (m_MenuItems.empty())
      return;

   if (CInputManager::GetInstance()->GetKey(DIK_DOWN))
   {
      m_ItemIterator = m_MenuItems.begin();

      while (m_ItemIterator != m_MenuItems.end())
      {
         if ((*m_ItemIterator)->GetSelected())
         {

            MakeAllUnSelected();
            
            if (*m_ItemIterator == m_MenuItems.back())
            {
               m_MenuItems.front()->MakeSelected();
            }
            else
            {
               m_ItemIterator++;
               (*m_ItemIterator)->MakeSelected();
            }

            break;
         }

         m_ItemIterator ++;
      }
   }
   else if (CInputManager::GetInstance()->GetKey(DIK_UP))
   {
      m_ItemIterator = m_MenuItems.begin();

      while (m_ItemIterator != m_MenuItems.end())
      {
         if ((*m_ItemIterator)->GetSelected())
         {

            MakeAllUnSelected();

            if (*m_ItemIterator == m_MenuItems.front())
            {
               m_MenuItems.back()->MakeSelected();
            }
            else
            {
               m_ItemIterator--;
               (*m_ItemIterator)->MakeSelected();
            }

            return;
         }

         m_ItemIterator ++;
      }
   }

   m_ItemIterator = m_MenuItems.begin();

   while (m_ItemIterator != m_MenuItems.end())
   {
   /*
      loc      = (*m_ItemIterator)->GetLocation();
      height   = (*m_ItemIterator)->GetHeight();
      width    = (*m_ItemIterator)->GetWidth();

      left     = loc.x - (width/2.0f);
      right    = loc.x + (width/2.0f);
      top      = loc.y + (height/2.0f);
      bottom   = loc.y - (height/2.0f);
   */

      int tId = (*m_ItemIterator)->GetTexture();

      FloatRect TempRect;

      TempRect = g_MenuGraphics.GetRect(tId);
      CVector2f pt;

      pt.x = m_MousePos.x - .0175f;
      pt.y = m_MousePos.y + .0241667f;

      if (FloatRectToPoint(&TempRect, &pt))
      {
         MakeAllUnSelected();
         (*m_ItemIterator)->MakeSelected();
         if (CInputManager::GetInstance()->PollMouseClick())
         {
            (*m_ItemIterator)->MakeClicked();
         }
      }


      if ((*m_ItemIterator)->GetSelected())
      {
         if (CInputManager::GetInstance()->GetKey(DIK_RETURN))
         {
            (*m_ItemIterator)->MakeClicked();
         }
      }

      m_ItemIterator ++;
   }

}


void CGameInterface::SetMousePos(CVector3f vec)
{
   m_MousePos.x = vec.x;
   m_MousePos.y = vec.y;
   m_MousePos.z = vec.z;
}


bool  CGameInterface::IsItemActive()
{
   return m_CurrentObject->IsActive();
}



void  CGameInterface::ActivateItem()
{
   m_CurrentObject->Activate();
}


void  CGameInterface::DeactivateItem()
{
   m_CurrentObject->Deactivate();
}


void  CGameInterface::ActivateItem(int ItemId)
{
   SetCurrentItem(ItemId);
   m_CurrentObject->Activate();
}


void  CGameInterface::DeactivateItem(int ItemId)
{
   SetCurrentItem(ItemId);
   m_CurrentObject->Deactivate();
}

void CGameInterface::AddBackground(unsigned int Index)
{
   m_Backgrounds.push_back(Index);
   m_TotalBackgrounds ++;
}

