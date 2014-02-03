#include "menu\InterfaceObject.h"
#include "menu\IRManager.h"

#include "utils\mmgr.h"


// globals here
extern CGameOptions              g_GameOptions;
extern CTextureManager           g_TextureManager;
extern float                     g_time;
extern CIRManager g_MenuGraphics;

CGameInterfaceObject::CGameInterfaceObject()
{
   Init();
}



CGameInterfaceObject::~CGameInterfaceObject()
{
}


void CGameInterfaceObject::Init()
{
   m_Selected              = false;
   m_Active                = false;
   m_Clicked               = false;
   m_FuncAble              = false;
   m_Texture               = -1;
   m_UnSelectedInactive    = -1;
   m_SelectedInactive      = -1;
   m_UnSelectedActive      = -1;
   m_SelectedActive        = -1;
   m_ModelId               = -1;
   m_PlaneId               = -1;
   m_Id                    = -1;
   m_Text                  = "";
   m_pOnClickFunc          = 0;
   m_Location.x            = 0.0f;
   m_Location.y            = 0.0f;
}


void CGameInterfaceObject::Update()
{
   if (m_Clicked)
   {
      if (m_FuncAble)
      {
         m_pOnClickFunc(&g_GameOptions);
      }
   }
}

void CGameInterfaceObject::Render()
{

/*
   float RightX;
   float TopY;

   RightX   = m_Location.x - (m_Width/2.0f);
   TopY     = m_Location.y + (m_Height/2.0f);

   if (m_Active)
   {
      if (m_Selected)
         g_TextureManager.ActivateTexture(m_SelectedActive);
      else
         g_TextureManager.ActivateTexture(m_UnSelectedActive);
   }
   else
   {
      if (m_Selected)
         g_TextureManager.ActivateTexture(m_SelectedInactive);
      else
         g_TextureManager.ActivateTexture(m_UnSelectedInactive);

   }

   glBegin(GL_QUADS);

   glTexCoord2i(0,1);
   glVertex2f(RightX , TopY);

   glTexCoord2i(0,0);
   glVertex2f(RightX, TopY - m_Height);

   glTexCoord2i(1,0);
   glVertex2f(RightX + m_Width, TopY - m_Height);
  
   glTexCoord2i(1,1);
   glVertex2f(RightX + m_Width, TopY);

   glEnd();

*/

   if (m_Active)
   {
      if (m_UnSelectedActive != -1)
         g_MenuGraphics.IRRender(m_UnSelectedActive);
   }
   else
   {
      if (m_UnSelectedInactive != -1)
         g_MenuGraphics.IRRender(m_UnSelectedInactive);
   }

   if (m_Selected)
   {
      if (m_SelectedInactive != -1)
         g_MenuGraphics.IRRender(m_SelectedInactive);
   }
}


void CGameInterfaceObject::SetFuncAble(bool State)
{
   m_FuncAble = State;
}


void CGameInterfaceObject::SetFuncAble(void (*pOnClickFunc)(CGameOptions*))
{
   m_pOnClickFunc = pOnClickFunc;
   m_FuncAble = true;
}


bool CGameInterfaceObject::GetClicked()
{
   return m_Clicked;
}


void CGameInterfaceObject::MakeClicked()
{
   m_Clicked = true;
}



bool CGameInterfaceObject::GetSelected()
{
   return m_Selected;
}



void  CGameInterfaceObject::SetPlaneId(int Id)
{
   m_PlaneId = Id;
}



void  CGameInterfaceObject::SetModel(int Id)
{
   m_ModelId = Id;
}



void  CGameInterfaceObject::SetItemId(int Id)
{
   m_Id = Id;
}



void  CGameInterfaceObject::SetLocation(CVector2f Loc)
{
   m_Location = Loc;
}



void  CGameInterfaceObject::SetText(string Txt)
{
   m_Text = Txt;
}



void CGameInterfaceObject::SetTexture(int Id)
{
   m_Texture = Id;
}



int   CGameInterfaceObject::GetId()
{
   return m_Id;
}



void  CGameInterfaceObject::SetId(int Id)
{
   m_Id = Id;
}



void  CGameInterfaceObject::SetActiveSelectedTexture(int TextureId)
{
   m_SelectedActive = TextureId;
}


void  CGameInterfaceObject::SetInactiveSelectedTexture(int TextureId)
{
   m_SelectedInactive = TextureId;
}



void  CGameInterfaceObject::SetActiveUnSelectedTexture(int TextureId)
{
   m_UnSelectedActive = TextureId;
}



void  CGameInterfaceObject::SetInactiveUnSelectedTexture(int TextureId)
{
   m_UnSelectedInactive = TextureId;

}

void  CGameInterfaceObject::MakeUnclicked()
{
   m_Clicked = false;
}

void  CGameInterfaceObject::SetWidth(float Width)
{
   m_Width = Width;
}


void  CGameInterfaceObject::SetHeight(float Height)
{
   m_Height = Height;
}

void CGameInterfaceObject::MakeSelected()
{
   m_Selected = true;
}

void CGameInterfaceObject::MakeUnselected()
{
   m_Selected = false;
}


void CGameInterfaceObject::GetInput()
{

}


float  CGameInterfaceObject::GetHeight()
{
   return m_Height;
}


float  CGameInterfaceObject::GetWidth()
{
   return m_Width;
}


CVector2f CGameInterfaceObject::GetLocation()
{
   return m_Location;
}

void CGameInterfaceObject::Activate()
{
   m_Active = true;
}

void CGameInterfaceObject::Deactivate()
{
   m_Active = false;
}


bool CGameInterfaceObject::IsActive()
{
   return m_Active;
}

int CGameInterfaceObject::GetTexture()
{
   return m_UnSelectedInactive;
}
