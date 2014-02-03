#include "menu\InterfaceManager.h"
#include "menu\irmanager.h"
#include "menu\interface.h"
#include "input\inputmanager.h"

#include "utils\mmgr.h"


// globals
//extern CTextureManager           g_TextureManager;
extern CIRManager                g_MenuGraphics;

CGameInterfaceManager* CGameInterfaceManager::m_pInstance = 0;

CGameInterfaceManager::CGameInterfaceManager()
{

}


CGameInterfaceManager::~CGameInterfaceManager()
{
}

void CGameInterfaceManager::Shutdown()
{
   if (m_pInstance)
   {
      CGameInterface* Temp;

      while (!m_Menus.empty())
      {
         Temp = m_Menus.back();
         m_Menus.pop_back();
   
         if (Temp)
            delete Temp;
      }
   }

   m_Menus.clear();
   
   delete m_pInstance;
   m_pInstance = 0;
}

void CGameInterfaceManager::Init()
{
   m_CurrentMenu   = 0;
   m_PointerHeight = 0.0f;
   m_PointerWidth  = 0.0f;
   m_pInput        = CInputManager::GetInstance();
   m_ShowMouse     = true;
}

void  CGameInterfaceManager::ShowMouse(bool State)
{
   m_ShowMouse = State;
}

CGameInterfaceManager* CGameInterfaceManager::GetInstance()
{
   if (!m_pInstance)
      m_pInstance = new CGameInterfaceManager;

   return m_pInstance;
}


bool  CGameInterfaceManager::SetCurrentMenu(int MenuId)
{

   if (m_Menus.empty())
      return false;

   bool end = false;

   m_MenuIterator = m_Menus.begin();


   while (m_MenuIterator != m_Menus.end())
   {
      if ((*m_MenuIterator)->GetId() == MenuId)
      {
         end = true;
         break;
      }

      m_MenuIterator ++;
   }

   m_CurrentMenu = *m_MenuIterator;
   
   return end;
}


bool  CGameInterfaceManager::GetItemClicked(int ItemId)
{
   return m_CurrentMenu->GetItemClicked(ItemId);
}


int  CGameInterfaceManager::MakeMenu()
{
   CGameInterface* Temp = new CGameInterface;

   Temp->SetId((int)m_Menus.size());


   m_Menus.push_back(Temp);

   m_CurrentMenu = Temp;

   return m_CurrentMenu->GetId();
}


int   CGameInterfaceManager::MakeItem()
{
   return m_CurrentMenu->MakeItem();
}


void  CGameInterfaceManager::SetItemTexture(int ItemId, int TextureId)
{
   m_CurrentMenu->SetCurrentItem(ItemId);
   m_CurrentMenu->SetItemTexture(TextureId);
}


void  CGameInterfaceManager::SetActiveSelectedTexture(int ItemId, int TextureId)
{
   m_CurrentMenu->SetCurrentItem(ItemId);
   m_CurrentMenu->SetActiveSelectedTexture(TextureId);
}


void  CGameInterfaceManager::SetInactiveSelectedTexture(int ItemId, int TextureId)
{
   m_CurrentMenu->SetCurrentItem(ItemId);
   m_CurrentMenu->SetInactiveSelectedTexture(TextureId);
}



void  CGameInterfaceManager::SetActiveUnSelectedTexture(int ItemId, int TextureId)
{
   m_CurrentMenu->SetCurrentItem(ItemId);
   m_CurrentMenu->SetActiveUnSelectedTexture(TextureId);
}



void  CGameInterfaceManager::SetInactiveUnSelectedTexture(int ItemId, int TextureId)
{
   m_CurrentMenu->SetCurrentItem(ItemId);
   m_CurrentMenu->SetInactiveUnSelectedTexture(TextureId);
}



void  CGameInterfaceManager::SetItemFuncAble(int ItemId, bool FuncAble)
{
   m_CurrentMenu->SetCurrentItem(ItemId);
   m_CurrentMenu->SetItemFuncAble(FuncAble);
}


void  CGameInterfaceManager::SetItemFuncAble(int ItemId, void (*pOnClickFunc)(CGameOptions* pOptions))
{
   m_CurrentMenu->SetCurrentItem(ItemId);
   m_CurrentMenu->SetItemFuncAble(pOnClickFunc);
}


void  CGameInterfaceManager::SetItemLoc(int ItemId, CVector2f Loc)
{
   m_CurrentMenu->SetCurrentItem(ItemId);
   m_CurrentMenu->SetItemLoc(Loc);
}


void  CGameInterfaceManager::SetItemTexture(int TextureId)
{
   m_CurrentMenu->SetItemTexture(TextureId);
}


void  CGameInterfaceManager::SetActiveSelectedTexture(int TextureId)
{
   m_CurrentMenu->SetActiveSelectedTexture(TextureId);
}


void  CGameInterfaceManager::SetInactiveSelectedTexture(int TextureId)
{
   m_CurrentMenu->SetInactiveSelectedTexture(TextureId);
}



void  CGameInterfaceManager::SetActiveUnSelectedTexture(int TextureId)
{
   m_CurrentMenu->SetActiveUnSelectedTexture(TextureId);
}



void  CGameInterfaceManager::SetInactiveUnSelectedTexture(int TextureId)
{
   m_CurrentMenu->SetInactiveUnSelectedTexture(TextureId);
}



void  CGameInterfaceManager::SetItemFuncAble(bool FuncAble)
{
   m_CurrentMenu->SetItemFuncAble(FuncAble);
}


void  CGameInterfaceManager::SetItemFuncAble(void (*pOnClickFunc)(CGameOptions*))
{
   m_CurrentMenu->SetItemFuncAble(pOnClickFunc);
}


void  CGameInterfaceManager::SetItemLoc(CVector2f Loc)
{
   m_CurrentMenu->SetItemLoc(Loc);
}


void  CGameInterfaceManager::SetItemPlaneId(int PlaneId)
{
   m_CurrentMenu->SetItemPlaneId(PlaneId);
}


void  CGameInterfaceManager::Update()
{
   m_CurrentMenu->Update();
}


void CGameInterfaceManager::Render()
{
   // render contents of entire menu
   m_CurrentMenu->Render();

   // render the mouse pointer
   RenderMouse();

/*
   g_TextureManager.ActivateTexture(m_PointerTexture);

   glBegin(GL_QUADS);

   glTexCoord2i(0,1);
   glVertex2f(m_MousePos.x - m_PointerWidth , m_MousePos.y);

   glTexCoord2i(0,0);
   glVertex2f(m_MousePos.x - m_PointerWidth, m_MousePos.y - m_PointerHeight);

   glTexCoord2i(1,0);
   glVertex2f(m_MousePos.x, m_MousePos.y - m_PointerHeight);
  
   glTexCoord2i(1,1);
   glVertex2f(m_MousePos.x, m_MousePos.y);

   glEnd();
*/
}

void CGameInterfaceManager::RenderMouse()
{
   if (m_ShowMouse)
      g_MenuGraphics.IRRender(m_PointerTexture);
}

void CGameInterfaceManager::MakeItemUnclicked(int ItemId)
{
   m_CurrentMenu->MakeItemUnclicked(ItemId);
}


void  CGameInterfaceManager::SetItemWidth(float Width)
{
   m_CurrentMenu->SetItemWidth(Width);
}


void  CGameInterfaceManager::SetItemHeight(float Height)
{
   m_CurrentMenu->SetItemHeight(Height);
}


void CGameInterfaceManager::GetInput()
{
   float x = 0.0f, y = 0.0f, z = 0.0f;

   m_pInput->PollMouse();
   m_pInput->PollMouseMove(x, y, z);

   m_MousePos.x += (x/250.0f);
   m_MousePos.y -= (y/250.0f);

   if (m_MousePos.x < -1.0f + m_PointerWidth)
      m_MousePos.x = -1.0f;

   if (m_MousePos.x > 1.0f)
      m_MousePos.x = 1.0f;

   if (m_MousePos.y < -1.0f + m_PointerHeight)
      m_MousePos.y = -1.0f;

   if (m_MousePos.y > 1.0f)
      m_MousePos.y = 1.0f;

   m_CurrentMenu->SetMousePos(m_MousePos);
   g_MenuGraphics.SetPos(m_PointerTexture, &m_MousePos);
   m_CurrentMenu->GetInput();

}

void  CGameInterfaceManager::MakeItemSelected(int ItemId)
{
   m_CurrentMenu->MakeItemSelected(ItemId);
}


void  CGameInterfaceManager::MakeAllUnSelected()
{
   m_CurrentMenu->MakeAllUnSelected();
}


void CGameInterfaceManager::SetPointerTexture(int Id)
{
   m_PointerTexture = Id;
}


void CGameInterfaceManager::SetPointerWidth(float Width)
{
   m_PointerWidth = Width;
}


void CGameInterfaceManager::SetPointerHeight(float Height)
{
   m_PointerHeight = Height;
}


bool  CGameInterfaceManager::IsItemActive()
{
   return m_CurrentMenu->IsItemActive();
}


void  CGameInterfaceManager::ActivateItem()
{
   m_CurrentMenu->ActivateItem();
}


void  CGameInterfaceManager::DeactivateItem()
{
   m_CurrentMenu->DeactivateItem();
}



void CGameInterfaceManager::DeactivateItem(int ItemId)
{
   m_CurrentMenu->DeactivateItem(ItemId);
}



void CGameInterfaceManager::ActivateItem(int ItemId)
{
   m_CurrentMenu->ActivateItem(ItemId);
}

void CGameInterfaceManager::AddBackground(unsigned int Index)
{
   m_CurrentMenu->AddBackground(Index);
}

CVector3f CGameInterfaceManager::GetMousePos()
{
   return m_MousePos;
}

void CGameInterfaceManager::SetMousePos(CVector3f Pos)
{
   m_MousePos.x = Pos.x;
   m_MousePos.y = Pos.y;
   m_MousePos.z = Pos.z;
   m_CurrentMenu->SetMousePos(Pos);
}
