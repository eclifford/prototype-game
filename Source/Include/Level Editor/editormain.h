/// \file gamemain.h
/// The ingame loop
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <commdlg.h>

#include "core\gamestatemanager.h"
#include "math3d\primitives.h"
#include "level editor\patchlist.h"
#include "graphics\ptgfile.h"

// forward declarations
class CRenderList;
class CSkyBox;
class CGameObject;

struct Waypoint
{
   void Draw();
   CObjFrame m_Frame;
   unsigned int m_Group;
   unsigned int m_Index;         
};

struct WaypointNode
{
   Waypoint* m_pWaypoint;
   WaypointNode* m_pNext;
   WaypointNode* m_pPrev;
};

class CEditorMain : CGameStateManager 
{
   friend class CGameMain;

	public:
		CEditorMain() { m_Testing = false; m_Initialized = false; m_CurrentEditorState = EDITOR_RUNNING;};
		~CEditorMain() {}; 

      enum EditorStates{EDITOR_RUNNING, GAME_EXIT, ENTER_GAME};
      unsigned int m_CurrentEditorState;

		virtual void Initialize();
		virtual void GetInput();
		virtual unsigned int Main();
		virtual void Render();
		virtual unsigned int Update();
		virtual void Shutdown();
      friend BOOL CALLBACK ToolboxDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,
                                             LPARAM lParam);
      friend BOOL CALLBACK DeformDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,
                                            LPARAM lParam);
      friend BOOL CALLBACK FlattenDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,
                                             LPARAM lParam);
	   friend BOOL CALLBACK ObjectDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,
                                           LPARAM lParam);
      friend BOOL CALLBACK UtilitiesDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam,
                                               LPARAM lParam);
   
      bool m_SavedLatest;
      bool m_PlayerStart;

   private:

      bool m_Testing;

      void AddWaypoint(WaypointNode* pNewNode);
      void RemoveWaypoint(WaypointNode* pWaypointNode);
      void ReinsertWaypoint(WaypointNode* pWaypointNode);
      void DeleteWaypoint(WaypointNode* pWaypointNode);

      void ClearWaypoints();

      bool Save();
      bool SaveAs();
      bool Load();
      bool Backup();

      void DeleteObjects();

      typedef BOOL (CALLBACK *DialogProc)(HWND hwndDlg, UINT uMsg, WPARAM wParam,
                                             LPARAM lParam);

      struct ToolDialog
      {
         void Initialize(DialogProc pDialogProc, unsigned int ResourceDefine);
         void Enable();
         void Disable();
         HWND m_DialogHandle;
         DialogProc m_pDialogProc;
      };
      
      enum Tool { DEFORM, FLATTEN, OBJECT, WAYPOINT, UTILITIES, NUM_TOOLS};     
            
      ToolDialog m_pToolDialogs[NUM_TOOLS];

      WaypointNode* m_pFirstWaypoint;

      unsigned int m_CurrentTool;

      bool m_Quitting;

      OPENFILENAME m_OpenFile;

      // deform dialog options
      float m_DeformRadius;
      float m_DeformHeight;

      // flatten dialog options
      float m_FlattenWidth;
      float m_FlattenHeight;

      // enemy dialog options
      unsigned int m_CurrentObjectType;
            		
		CRenderList* m_pRenderList;
      CSkyBox* m_pSkyBox;
      HWND m_ToolboxHandle;

      Point3f m_LastCameraPos;
   
      Ray m_SelectionRay;
      CVector2f m_SelectionPoint;
      CVector3f m_RayCollidePoint;

      float m_LastClickTime;
      float m_ClickDelay;
      
      Point3f m_MousePos;

      RECT m_ToolboxRect;

      RECT m_ToolRect;

      bool m_MovingObject;

      float m_LastRotation;
      
      double m_ModelMatrix[16];

      CPatchList m_PatchList;
      char m_pLevelFilePath[PT_FILE_PATH_LEN];
      

      CGameObject* m_pCurrentObject;
      
      unsigned int m_NumObjects;
      
      bool m_Loaded;
      bool m_Saved;
      
              
};