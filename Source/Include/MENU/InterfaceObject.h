/// \file InterfaceObject.h
/// InterfaceObject header file
//////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4786)
#pragma once

// #includes here
#include "math3d\Math3D.h"
#include "core\oglstatemanager.h"
#include "GRAPHICS\TextureManager.h"
#include "gamelogic\GameOptions.h"
#include "core\oglwindow.h"
#include "input\inputmanager.h"

#include <string>
using namespace std;

/// An header for menu interface objects

class CGameInterfaceObject
{
   public:
      CGameInterfaceObject();
      ~CGameInterfaceObject();
      void  Init();
      void  Update();
      void  Render();
      void  GetInput();
      void  OnClicked();
      void  OnMouseOver();
      void  SetFuncAble(bool State);
      void  SetFuncAble(void (*pOnClickFunc)(CGameOptions*));
      bool  GetClicked();
      bool  GetSelected();
      void  SetPlaneId(int Id);
      void  SetModel(int Id);
      void  SetItemId(int Id);
      void  SetLocation(CVector2f Loc);
      void  SetText(string Txt);
      void  SetTexture(int Id);
      int   GetId();
      void  SetId(int Id);
      void  MakeUnclicked();
      void  MakeClicked();
      void  SetActiveSelectedTexture(int TextureId);
      void  SetInactiveSelectedTexture(int TextureId);
      void  SetActiveUnSelectedTexture(int TextureId);
      void  SetInactiveUnSelectedTexture(int TextureId);
      void  SetWidth(float Width);
      void  SetHeight(float Height);
      void  MakeSelected();
      void  MakeUnselected();
      void  Deactivate();
      void  Activate();
      CVector2f GetLocation();
      float GetHeight();
      float GetWidth();
      bool  IsActive();
      int   GetTexture();

   protected:

   private:
      bool           m_Selected;
      bool           m_Active;
      bool           m_Clicked;
      bool           m_FuncAble;
      CVector2f      m_Location;
      float          m_Height;
      float          m_Width;
      int            m_Texture;
      int            m_UnSelectedInactive;
      int            m_SelectedInactive;
      int            m_UnSelectedActive;
      int            m_SelectedActive;
      int            m_ModelId;
      int            m_PlaneId;
      int            m_Id;
      string         m_Text;
      void           (*m_pOnClickFunc)(CGameOptions*);
};
