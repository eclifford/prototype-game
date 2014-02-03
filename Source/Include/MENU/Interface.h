/// \file Interface.h
/// Interface header file
//////////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4786)
#pragma once

/// An header for a menu object

// #includes here
#include <list>
#include <vector>
#include "InterfaceObject.h"
#include "math3d\math3d.h"
#include "input/inputmanager.h"

class CGameInterface
{
   public:
      CGameInterface();
      ~CGameInterface();
      void  Init();
      int   GetId();
      void  SetId(int Id);
      bool  SetCurrentItem(int ItemId);
      void  SetItemTexture(int TextureId);
      void  SetActiveSelectedTexture(int TextureId);
      void  SetInactiveSelectedTexture(int TextureId);
      void  SetActiveUnSelectedTexture(int TextureId);
      void  SetInactiveUnSelectedTexture(int TextureId);
      void  SetItemFuncAble(bool FuncAble);
      void  SetItemFuncAble(void (*pOnClickFunc)(CGameOptions* pOptions));
      void  SetItemLoc(CVector2f Loc);
      void  SetItemPlaneId(int PlaneId);
      void  MakeItemUnclicked(int ItemId);
      void  MakeItemSelected(int ItemId);
      void  ActivateItem(int ItemId);
      void  DeactivateItem(int ItemId);
      void  MakeAllUnSelected();
      void  SetItemId(int Id);
      void  SetItemId();
      int   GetItemId();
      int   MakeItem();
      void  Update();
      void  Render();
      void  GetInput();
/*      bool  GotoNextItem();
      void  GotoFirstItem();*/
      void  SetItemHeight(float Height);
      void  SetItemWidth(float Width);
      void  StartInterfaceRender();
      void  EndInterfaceRender();
      bool  GetItemClicked(int ItemId);
      void  SetMousePos(CVector3f vec);
      bool  IsItemActive();
      void  ActivateItem();
      void  DeactivateItem();
      void  AddBackground(unsigned int Index);

   protected:
   
   private:
      list<CGameInterfaceObject*>   m_MenuItems;
      list<CGameInterfaceObject*>
               ::iterator           m_ItemIterator;
      list<CGameInterfaceObject*>
               ::iterator           m_ItemIterator2;
      vector<unsigned int>          m_Backgrounds;
      unsigned int                  m_TotalBackgrounds;
      int                           m_Id;
      CGameInterfaceObject*         m_CurrentObject;
      CVector3f                     m_MousePos;
};

