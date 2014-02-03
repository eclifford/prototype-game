/// \file InterfaceManager.h
/// Interface header file
//////////////////////////////////////////////////////////////////////////

//#pragma warning(disable : 4786)
#pragma once


// #includes here
#include <list>
#include "math3d\math3d.h"

// forward declarations
class CGameInterface;
class CInputManager;
class CGameOptions;

/// An header for a menu object

class CGameInterfaceManager
{
   public:

      ~CGameInterfaceManager();
      void  Shutdown();
      void  Init();
      static CGameInterfaceManager* GetInstance();
      bool  SetCurrentMenu(int MenuId);
      bool  GetItemClicked(int ItemId);
      void  MakeItemUnclicked(int ItemId);
      void  MakeItemSelected(int ItemId);
      void  DeactivateItem(int ItemId);
      void  ActivateItem(int ItemId);
      void  MakeAllUnSelected();
      void  MakeAllUnclicked();
      bool  IsItemActive();
      void  ActivateItem();
      void  DeactivateItem();
 /*     bool  GotoNextItem();
      void  GotoFirstItem();*/
      int   MakeMenu();
      int   MakeItem();
      void  SetItemTexture(int ItemId, int TextureId);
      void  SetActiveSelectedTexture(int ItemId, int TextureId);
      void  SetInactiveSelectedTexture(int ItemId, int TextureId);
      void  SetActiveUnSelectedTexture(int ItemId, int TextureId);
      void  SetInactiveUnSelectedTexture(int ItemId, int TextureId);
      void  SetItemFuncAble(int ItemId, bool FuncAble);
      void  SetItemFuncAble(int ItemId, void (*pOnClickFunc)(CGameOptions*));
      void  SetItemLoc(int ItemId, CVector2f Loc);
      void  SetItemTexture(int TextureId);
      void  SetActiveSelectedTexture(int TextureId);
      void  SetInactiveSelectedTexture(int TextureId);
      void  SetActiveUnSelectedTexture(int TextureId);
      void  SetInactiveUnSelectedTexture(int TextureId);
      void  SetItemFuncAble(bool FuncAble);
      void  SetItemFuncAble(void (*pOnClickFunc)(CGameOptions*));
      void  SetItemLoc(CVector2f Loc);
      void  SetItemPlaneId(int PlaneId);
      void  SetItemWidth(float Width);
      void  SetItemHeight(float Height);
      void  Update();
      void  Render();
      void  GetInput();
      void  SetPointerTexture(int Id);
      void  SetPointerWidth(float Width);
      void  SetPointerHeight(float Height);
      void  AddBackground(unsigned int Index);
      CVector3f GetMousePos();
      void  SetMousePos(CVector3f Pos);
      void  ShowMouse(bool State);
      void  RenderMouse();

   protected:
   
   private:
      CGameInterfaceManager();
		static CGameInterfaceManager* m_pInstance; // Pointer to instance of singleton class
		CGameInterfaceManager(const &CGameInterfaceManager) {};
		CGameInterfaceManager &operator = (const &CGameInterfaceManager) {};

      std::list<CGameInterface*>   m_Menus;
      std::list<CGameInterface*>::iterator     m_MenuIterator;
      CGameInterface*         m_CurrentMenu;
      CInputManager*          m_pInput;
      CVector3f               m_MousePos;
      int                     m_PointerTexture;
      float                   m_PointerWidth;
      float                   m_PointerHeight;
      bool                    m_ShowMouse;
};

