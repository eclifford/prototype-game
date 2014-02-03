/// \file gamemain.cpp
/// Main Game 
//////////////////////////////////////////////////////////////////////////

#define VC_EXTRALEAN
#include <windows.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include "level editor\editormain.h"
#include "core\oglstatemanager.h"
#include "graphics\texturemanager.h"
#include "graphics\geometrymanager.h"
#include "Camera\Camera.h"
#include "graphics\ptgfile.h"
#include "graphics\pttfile.h"
#include "level editor\resource.h"
#include "input\inputmanager.h"
#include "math3d\intersection.h"
#include "graphics\drawshape.h"
#include "gamelogic\player.h"
#include "core\gamemain.h"
#include "gamelogic\mothership.h"
#include "level editor\level.h"
#include "core\menumain.h"
#include "graphics\light.h"
#include "gamelogic\objectfactory.h"
#include "graphics\skybox.h"
#include "graphics\terrain.h"
#include "physics\physics.h"
#include "camera\frustum.h"

#include "utils\mmgr.h"

extern CDirectionalLight g_Sunlight;


// Global instances
extern CObjectFactory g_ObjectFactory;
extern COpenGLStateManager *pGLSTATE;
extern CTextureManager g_TextureManager;
extern CFrustum g_Frustum;
extern CBaseCamera* g_Camera;
extern CFirstPersonCamera g_FirstPersonCamera;
extern CGeometryManager g_GeometryManager;
extern CQuadtree g_Quadtree;
extern COpenGLWindow *pGLWIN;
extern CTerrain* g_pCurrLevel;
extern CEditorMain EditorMain;
extern CPlayer g_Player;
extern CGameMain GameMain;
extern CMenuMain MenuMain;
extern CGameStateManager GameStateManager;
extern CGameOptions g_GameOptions;
extern float g_time;
extern float g_FrameTime;
extern CPlayer g_Player;
extern CGameObject* g_CollisionAvoidanceList;
extern CGameOptions g_GameOptions;
extern unsigned int g_NumTextures;
extern unsigned int g_NumModels;
extern CMothership g_MotherShip;

void CEditorMain::AddWaypoint(WaypointNode* pNewNode)
{
   // if the list is emtpy
   /*if (!m_pFirstWaypoint)
   {
      m_pFirstWaypoint = pNewNode;
      m_pFirstWaypoint->m_pPrev = 0;
      m_pFirstWaypoint->m_pNext = 0;
      return;
   }*/
   /*
   // check to make sure we shouldn't put it before the first one
   if (pNewNode->m_pWaypoint->m_Group <= m_pFirstWaypoint->m_pWaypoint->m_Group)
   {
      if (pNewNode->m_pWaypoint->m_Index <= m_pFirstWaypoint
   }*/

   WaypointNode* pCurr = m_pFirstWaypoint;
   WaypointNode* pPrev = 0;

   while (pCurr)
   {
      // if we found a higher or equal group
      if (pNewNode->m_pWaypoint->m_Group <= pCurr->m_pWaypoint->m_Group)
      {
         // insert it into the group based on its index
         while (pCurr && pCurr->m_pWaypoint->m_Group == pNewNode->m_pWaypoint->m_Group)
         {
            // if we found a higher index
            if (pNewNode->m_pWaypoint->m_Index < pCurr->m_pWaypoint->m_Index)
            {
               // insert here
               pNewNode->m_pPrev = pCurr->m_pPrev;
               pNewNode->m_pNext = pCurr;
               
               if (pCurr->m_pPrev->m_pNext)
                  pCurr->m_pPrev->m_pNext = pNewNode;
                              
               pCurr->m_pPrev = pNewNode;
               return;            
            }

            pPrev = pCurr;
            pCurr = pCurr->m_pNext;
         }
         
         // we got to the end of our group so just put it here
         // insert here
         pNewNode->m_pPrev = pPrev;
         pNewNode->m_pNext = pCurr;
         
         if (pPrev)
         {
            if (pPrev->m_pNext)
               pCurr->m_pPrev->m_pNext = pNewNode;
                        
            pCurr->m_pPrev = pNewNode;
         }

         return;   
      }
      
      pPrev = pCurr;
      pCurr = pCurr->m_pNext; 
   }
   
   // we got to the end of the list so put it here   
   if (pPrev)
   {   
      pPrev->m_pNext = pNewNode;   
      pNewNode->m_pPrev = pCurr;
      pNewNode->m_pNext = 0;
   }
   // there are no waypoints in the list
   else
   {
      m_pFirstWaypoint = pNewNode;
      m_pFirstWaypoint->m_pPrev = 0;
      m_pFirstWaypoint->m_pNext = 0;   
   }                       
}

void CEditorMain::ReinsertWaypoint(WaypointNode* pWaypointNode)
{
   RemoveWaypoint(pWaypointNode);
      
   // add it
   AddWaypoint(pWaypointNode);   
}

void CEditorMain::RemoveWaypoint(WaypointNode* pWaypointNode)
{
   if (pWaypointNode->m_pPrev)
      pWaypointNode->m_pPrev->m_pNext = pWaypointNode->m_pNext;
      
   if (pWaypointNode->m_pNext)
      pWaypointNode->m_pNext->m_pPrev = pWaypointNode->m_pPrev;   
}

void CEditorMain::DeleteWaypoint(WaypointNode* pWaypointNode)
{
   RemoveWaypoint(pWaypointNode);
   
   delete pWaypointNode->m_pWaypoint;
   delete pWaypointNode;   
}

void CEditorMain::ClearWaypoints()
{
   WaypointNode* pNextNode;
   while (m_pFirstWaypoint)
   {
      pNextNode = m_pFirstWaypoint->m_pNext;
      
      delete m_pFirstWaypoint->m_pWaypoint;
      delete m_pFirstWaypoint;
      
      m_pFirstWaypoint = pNextNode;   
   }
}

bool CEditorMain::Backup()
{
   // if we haven't loaded anything to backup, then break
   if (!m_Loaded)
   {
      MessageBox(NULL, "No level loaded.", "Error", MB_OK);
      return false;
   }

   if (!m_Saved)
   {
      MessageBox(NULL, "Cannot backup without saving.", "Error", MB_OK);
   }

   if (!m_PlayerStart)
   {
      MessageBox(NULL, "Cannot save without player in level.", "Error", MB_OK);
      return false;
   }

   // back up the file with a backup file name
   // find a backup file that hasn't been created
   unsigned int i = 0;
   char pBackupLevelFilePath[PT_FILE_PATH_LEN];

   // copy just the path into the backup file path
   strncpy(pBackupLevelFilePath, m_pLevelFilePath, m_OpenFile.nFileOffset);
   
   // append backup onto it
   strcpy(pBackupLevelFilePath + m_OpenFile.nFileOffset, "Backup");

   // append the number onto it
   itoa(i, pBackupLevelFilePath + strlen(pBackupLevelFilePath), 10);

   // append the file name onto it
   strcat(pBackupLevelFilePath, m_pLevelFilePath + m_OpenFile.nFileOffset);

   FILE* pLevelFile;
   while ((pLevelFile = fopen(pBackupLevelFilePath, "r")))
   { 
      fclose(pLevelFile);

      // copy just the path into the backup file path
      strncpy(pBackupLevelFilePath, m_pLevelFilePath, m_OpenFile.nFileOffset);
   
      // append backup onto it
      strcpy(pBackupLevelFilePath + m_OpenFile.nFileOffset, "Backup");

      // append the number onto it
      itoa(i, pBackupLevelFilePath + strlen(pBackupLevelFilePath), 10);

      // append the file name onto it
      strcat(pBackupLevelFilePath, m_pLevelFilePath + m_OpenFile.nFileOffset);
      
      ++i;   
   }

   SetWindowText(m_ToolboxHandle, "Backing up...");
   
   // we found a file path that hasn't been used so save it here
   if (!SaveLevel(pBackupLevelFilePath, m_NumObjects))
   {
      MessageBox(NULL, "Error saving level.", "Error", MB_OK);
      SetWindowText(m_ToolboxHandle, m_pLevelFilePath + m_OpenFile.nFileOffset);
      return false;
   }

   SetWindowText(m_ToolboxHandle, m_pLevelFilePath + m_OpenFile.nFileOffset);

   return true;
}

bool CEditorMain::SaveAs()
{
   // make sure it's loaded
   if (!m_Loaded)
   {
      MessageBox(NULL, "No map loaded.", "Error", MB_OK);               
      return false;
   }
   // make sure they placed a player in the map
   if (!m_PlayerStart)
   {
      MessageBox(NULL, "Cannot save without player in level.", "Error", MB_OK);
      return false;
   }
                              

   if (GetSaveFileName(&m_OpenFile))
   {
      SetWindowText(m_ToolboxHandle, "Saving...");
      if (SaveLevel(m_pLevelFilePath, m_NumObjects))
      {
         m_Saved = true;
         m_SavedLatest = true;
      }
      else
      {
         m_Saved = false;
         m_SavedLatest = false;
         SetWindowText(m_ToolboxHandle, m_pLevelFilePath + m_OpenFile.nFileOffset);
         MessageBox(NULL, "Error saving level.", "Save Error", MB_OK);                    
         return false;
      }

      SetWindowText(m_ToolboxHandle, m_pLevelFilePath + m_OpenFile.nFileOffset);
   }
   else
      return false;

   return true;
}

bool CEditorMain::Save()
{
   // make sure it's loaded
   if (!m_Loaded)
   {
      MessageBox(NULL, "No map loaded.", "Error", MB_OK);               
      return false;
   }

   if (!m_PlayerStart)
   {
      MessageBox(NULL, "Cannot save without player in level.", "Error", MB_OK);
      return false;
   }
                              
   // if its not the first time, just save it in the same spot   
   if (m_Saved)
   {
      SetWindowText(m_ToolboxHandle, "Saving...");
      if (!SaveLevel(m_pLevelFilePath, m_NumObjects))
      {
         SetWindowText(m_ToolboxHandle, m_pLevelFilePath + m_OpenFile.nFileOffset);
         MessageBox(NULL, "Error saving level.", "Error", MB_OK);
         m_Saved = false;
         m_SavedLatest = false;                  
         return false;
      }
      m_SavedLatest = true;
      SetWindowText(m_ToolboxHandle, m_pLevelFilePath + m_OpenFile.nFileOffset);   
   }
   // if it is the first time saving
   else
   {
      return SaveAs();
   }

   return true;
}

bool CEditorMain::Load()
{   
   if (!m_SavedLatest)
   {           
      if (MessageBox(NULL, "Would you like to save before loading?", "Save?", 
                        MB_YESNO) == IDYES)
      {
         if (!Save())
            return false;
      }
   }

   if (GetOpenFileName(&m_OpenFile))
   {
      if (m_Loaded)
      {
         // unload all the stuff
         g_pCurrLevel->Unload();
         DeleteObjects();
//         g_ObjectFactory.SetupObjects();
         m_pCurrentObject = 0;
         m_Saved = false;
         m_SavedLatest = false;
         m_Loaded = false;
         m_PlayerStart = false;
         m_pCurrentObject = 0;
         m_LastRotation = 0.0f;
         m_MovingObject = false;
         CObjFrame PlayerFrame;
         g_Player.SetFrame(&PlayerFrame, 0);
         //g_Player.GetRenderObj()->RemoveFromQuadtree();
         
         SetWindowText(m_ToolboxHandle, "No file loaded.");
      }
      
      SetWindowText(m_ToolboxHandle, "Loading...");
      if (LoadLevel(m_pLevelFilePath, m_pRenderList, &m_NumObjects))
      {
         m_Loaded = true;
         m_Saved = true;
         m_SavedLatest = true;

         SetWindowText(m_ToolboxHandle, m_pLevelFilePath + m_OpenFile.nFileOffset);
 
         Point3f CameraPos(0.0f, 0.0f, 0.0f);
         g_FirstPersonCamera.SetPosition(&CameraPos);
      }
      else
      {
         SetWindowText(m_ToolboxHandle, "No file loaded.");
         MessageBox(NULL, "Failed to load level.", "Error", MB_OK);         
         return false;
      }
   }
   else
      return false;


   return false;
}

void CEditorMain::ToolDialog::Enable()
{
   ShowWindow(m_DialogHandle, SW_SHOWNORMAL);   
}

void CEditorMain::ToolDialog::Disable()
{
   ShowWindow(m_DialogHandle, SW_HIDE);      
}

void CEditorMain::ToolDialog::Initialize(DialogProc pDialogProc, unsigned int ResourceDefine)
{
   m_DialogHandle = CreateDialog(pGLWIN->GetHINSTANCE(), MAKEINTRESOURCE(ResourceDefine),
                                 pGLWIN->GetHWND(), pDialogProc);

   RECT Rect;
   GetWindowRect(m_DialogHandle, &Rect);

   MoveWindow(m_DialogHandle, pGLWIN->GetScreenWidth() - (Rect.right - Rect.left),
              pGLWIN->GetScreenHeight() - (Rect.bottom - Rect.top), Rect.right - Rect.left, 
              Rect.bottom - Rect.top, FALSE);

   m_pDialogProc = pDialogProc;
}

BOOL CALLBACK ToolboxDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   switch (uMsg)									
	{
      case WM_COMMAND:      
         switch (LOWORD(wParam))
         {
            /*
            case ID_NEW:

               // if they haven't saved the latest
               if (!EditorMain.m_SavedLatest)
               {           
                  if (MessageBox(NULL, "Would you like to save before loading?", "Save?", 
                                    MB_YESNO) == IDYES)
                  {
                     

                  }



                  if (EditorMain.m_Loaded)
                  {
                    // unload all the stuff
                    g_pCurrLevel->Unload();
                    EditorMain.DeleteObjects();
                    g_ObjectFactory.SetupObjects();
                    EditorMain.m_pCurrentObject = 0;
                    EditorMain.m_Saved = false;
                  }
               }
               break;
            */
            case ID_TEST:
               if (!EditorMain.m_Loaded)
                  break;

               if (!EditorMain.m_SavedLatest)
               {
                  MessageBox(NULL, "Cannot test without saving first.", "Error", MB_OK);
                  break;
               }
               EditorMain.m_Testing = true;
               GameMain.m_CurrentLevel = CGameMain::EditorLevel;
               EditorMain.m_CurrentEditorState = CEditorMain::ENTER_GAME;
               break;
            case ID_QUIT:
               EditorMain.m_Quitting = true;
               if (EditorMain.m_Loaded && !EditorMain.m_SavedLatest)
               {
                  if (MessageBox(NULL, "Would you like to save before quitting?", "Save?", 
                                 MB_YESNO) == IDYES)
                  {
                     if (!EditorMain.Save())
                        EditorMain.m_Quitting = false;   
                  }
               }
               break;
            case ID_SAVE:              
               EditorMain.Save();
               break;
            case ID_SAVE_AS:
               EditorMain.SaveAs();         
               break;   
            case ID_LOAD:
               EditorMain.Load();
               break;
            case ID_BACKUP:
               EditorMain.Backup();
               break;

            case IDC_CURRENT_TOOL:
               if (HIWORD(wParam) == CBN_SELENDOK)
               {
                  // disable the current Tool
                  EditorMain.m_pToolDialogs[EditorMain.m_CurrentTool].Disable();

                  // change the current Tool
                  EditorMain.m_CurrentTool = (unsigned int)SendDlgItemMessage(EditorMain.m_ToolboxHandle,
                                                                IDC_CURRENT_TOOL, CB_GETCURSEL,
                                                                0, 0);
                  
                  // enable the new one
                  EditorMain.m_pToolDialogs[EditorMain.m_CurrentTool].Enable();    
               }   
               break;
         }
         break;
	}

	// Pass All Unhandled Messages To DefDlgProc
	return FALSE;
}

BOOL CALLBACK DeformDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   
   switch (uMsg)									
	{
      case WM_HSCROLL:      
         switch (GetDlgCtrlID((HWND)lParam))
         {            
            case IDC_DEFORM_HEIGHT_SLIDER:
               if (LOWORD(wParam) == TB_ENDTRACK)
               {
                  EditorMain.m_DeformHeight = .5f * SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);    
               }
               break;
            case IDC_DEFORM_RADIUS_SLIDER:
               if (LOWORD(wParam) == TB_ENDTRACK)
               {
                  EditorMain.m_DeformRadius = (float)SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
                     
                  //EditorMain.m_DeformRadius *= .05f;    
               }   
               break;
         }
         break;	
	}

	// Pass All Unhandled Messages To DefDlgProc
	return FALSE;
}

static BOOL CALLBACK FlattenDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   
   switch (uMsg)									
	{
      case WM_HSCROLL:      
         switch (GetDlgCtrlID((HWND)lParam))
         {            
            case IDC_FLATTEN_WIDTH_SLIDER:
               if (LOWORD(wParam) == TB_ENDTRACK)
               {
                  EditorMain.m_FlattenWidth = (float)SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
               }
               break;
            case IDC_FLATTEN_HEIGHT_SLIDER:
               if (LOWORD(wParam) == TB_ENDTRACK)
               {
                  EditorMain.m_FlattenHeight = (float)SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);    
               }   
               break;
         }
         break;	
	}

	// Pass All Unhandled Messages To DefDlgProc
	return FALSE;
}

BOOL CALLBACK ObjectDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   CVector2f Min;
   CVector2f Max;
   CGameObject* pCurrentObject;
      
   switch (uMsg)									
	{
      case WM_COMMAND:      
         switch (LOWORD(wParam))
         {
            case ID_ROTATE_LEFT:
               if (EditorMain.m_pCurrentObject)
               {
                  EditorMain.m_pCurrentObject->RotateLocalY(.3f, 0);
                  
                  
                  EditorMain.m_pCurrentObject->FollowTerrain();

                  EditorMain.m_SavedLatest = false;

                  CVector3f Angles;
                  EditorMain.m_pCurrentObject->GetFrame(0)->GetEulerAngles(&Angles);
                  EditorMain.m_LastRotation = Angles.y;
               }
               break;
            case ID_ROTATE_RIGHT:
               if (EditorMain.m_pCurrentObject)
               {
                  EditorMain.m_pCurrentObject->RotateLocalY(-.3f, 0);
                     
                  EditorMain.m_pCurrentObject->FollowTerrain();

                  EditorMain.m_SavedLatest = false;

                  CVector3f Angles;
                  EditorMain.m_pCurrentObject->GetFrame(0)->GetEulerAngles(&Angles);
                  EditorMain.m_LastRotation = Angles.y;
               }
               break;
            case ID_FLATTEN_SURROUNDINGS:
               if (EditorMain.m_pCurrentObject)
               {
                  // flatten the terrain around the object
                  float FlattenSize = EditorMain.m_pCurrentObject->GetSphere()->Radius; 
                  Min.x = -FlattenSize + EditorMain.m_pCurrentObject->GetSphere()->Center.x;
                  Min.y = -FlattenSize + EditorMain.m_pCurrentObject->GetSphere()->Center.z;

                  Max.x = FlattenSize + EditorMain.m_pCurrentObject->GetSphere()->Center.x;
                  Max.y = FlattenSize + EditorMain.m_pCurrentObject->GetSphere()->Center.z;

                  g_pCurrLevel->FlattenSquare(&Min, &Max);
               
                  // loop through all the game objects and place them
                  // oriented above the terrain                        
                  pCurrentObject = GameMain.m_ObjList;
                  while (pCurrentObject)
                  {
                     pCurrentObject->FollowTerrain();

                     pCurrentObject = pCurrentObject->m_Next;
                  }
               }
               break;

            case IDC_CURRENT_OBJECT:
               if (HIWORD(wParam) == CBN_SELENDOK)
               {
                  // change the current Tool
                  EditorMain.m_CurrentObjectType = (unsigned int)SendDlgItemMessage(EditorMain.m_pToolDialogs[CEditorMain::OBJECT].m_DialogHandle,
                                                                      IDC_CURRENT_OBJECT,
                                                                      CB_GETCURSEL, 0, 0);    
               }   
               break;
            case ID_DELETE:
               if (EditorMain.m_pCurrentObject)
               {         
                  GameMain.RemoveFromObjList(EditorMain.m_pCurrentObject);
                  EditorMain.m_pCurrentObject->RemoveFromQuadtree();
         
                  if (EditorMain.m_pCurrentObject == &g_Player)
                     EditorMain.m_PlayerStart = false;
                  else
                     g_ObjectFactory.ReturnAndNoRefund(EditorMain.m_pCurrentObject);
         
                  EditorMain.m_pCurrentObject = 0;

                  EditorMain.m_NumObjects--;

                  EditorMain.m_SavedLatest = false;
               }
               break;
         }
         break;	
	}

	// Pass All Unhandled Messages To DefDlgProc
	return FALSE;
}

BOOL CALLBACK WaypointDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   return FALSE;
}


BOOL CALLBACK UtilitiesDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   AABB TerrainBox;
   unsigned int NumCollisions = 0;      
   switch (uMsg)									
	{
      //case WM_SETFOCUS:
      //   SetFocus(GetDlgItem(hwndDlg, IDC_NUM_TREES_EDIT));
      //   break;
      case WM_COMMAND:      
         switch (LOWORD(wParam))
         {
            case IDC_APPLY:
               
               if (!EditorMain.m_Loaded)
                  break;

               EditorMain.m_SavedLatest = false;

               char Buffer[4];
               GetDlgItemText(hwndDlg, IDC_NUM_TREES_EDIT, Buffer, 4);
               unsigned int NumTrees = atoi(Buffer);

               // loop through the map and create trees
               g_pCurrLevel->GetAABB(&TerrainBox);
               
               SetWindowText(EditorMain.m_ToolboxHandle, "Placing trees...");

               for (unsigned int i = 0; i < NumTrees; i++)
               {
                  int TreeType = rand() % 3;
                  CGameObject* pGameObject;

                  switch (TreeType)
                  {
                     case 0:
                        pGameObject = g_ObjectFactory.CreateTree();
                        break;
                     case 1:
                        pGameObject = g_ObjectFactory.CreateTree2();
                        break;
                     case 2:
                        pGameObject = g_ObjectFactory.CreateTree3();
                        break;   
                  }

                  EditorMain.m_NumObjects++;

                  float Radius = pGameObject->GetSphere()->Radius;
                  pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2), 0);
                  
                  Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius,
                                                    TerrainBox.Max.x - Radius),
                                                    0.0f,
                                   RandomFloatRange(TerrainBox.Min.z + Radius,
                                                    TerrainBox.Max.z - Radius));

                  pGameObject->SetPosition(&Position, 0);
               
                  pGameObject->FollowTerrain();
               
                  // check to make sure we aren't colliding with anything

                  CGameObject* pCurr = GameMain.m_ObjList;
                  while (pCurr)
                  {
                     if (pCurr != pGameObject && OBBToOBB(pGameObject->GetOBB(),
                                  pCurr->GetOBB()))
                     {
                        GameMain.RemoveFromObjList(pGameObject);
                        pGameObject->RemoveFromQuadtree();
                        
                        g_ObjectFactory.ReturnAndNoRefund(pGameObject);

                        --EditorMain.m_NumObjects;
                        
                        --i;

                        NumCollisions++;

                        if (NumCollisions > NumTrees)
                        {
                           MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                           i = NumTrees;
                        }

                        break;                           
                     }
                     
                     pCurr = pCurr->m_Next;   
                  }
               } 

               SetWindowText(EditorMain.m_ToolboxHandle, EditorMain.m_pLevelFilePath + 
                             EditorMain.m_OpenFile.nFileOffset);
               
               break;
         }
         break;	
	}

	// Pass All Unhandled Messages To DefDlgProc
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////
/// Initialize
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////
void CEditorMain::Initialize()
{
   if(glIsEnabled(GL_LIGHTING))
	{
		bool debug = true;
	}

   g_CollisionAvoidanceList = NULL;

   Point3f CameraPos(0.0f, 0.0f, 0.0f);
   g_FirstPersonCamera.SetPosition(&CameraPos);

   float SceneAmbient[] = { 1.0, 1.0f, 1.0f, 1.0f };
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, SceneAmbient);

   m_PlayerStart = false;
   m_CurrentEditorState = EDITOR_RUNNING;
   GameMain.m_ObjList = 0;
   m_NumObjects = 0;
   m_Loaded = false;
   g_GameOptions.AIOff();
   g_Camera = &g_FirstPersonCamera;   
   g_pCurrLevel = new CTerrain;
   m_pRenderList = new CRenderList;   

	g_TextureManager.Initialize(g_NumTextures);
	g_GeometryManager.Initialize(g_NumModels);
	
	pGLSTATE->SetState(LIGHTING, true);
//	pGLSTATE->SetState(LIGHT1, true);

   GameMain.SetupLightingAndFog();
   		
	pGLSTATE->SetState(CULL_FACE, true);
	
	pGLSTATE->SetState(DEPTH_TEST, true);

   glAlphaFunc(GL_GREATER, 0);
	
	float Amb[] = {0.8f, 0.8f, 0.8f, 1.0f};
	float Dif[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float Spec[] = {.1f, .1f, .1f, 1.0f};
	
	pGLSTATE->SetState(BLEND, true);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	
//	glLightfv(GL_LIGHT1, GL_AMBIENT, Amb);
//	glLightfv(GL_LIGHT1, GL_DIFFUSE, Dif);
//	glLightfv(GL_LIGHT1, GL_SPECULAR, Spec);
	
	//glLightfv(GL_LIGHT1, GL_AMBIENT, Amb);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, Dif);
	//glLightfv(GL_LIGHT1, GL_SPECULAR, Spec);
	
	// enable the best texture filtering
	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	pGLSTATE->SetState(TEXTURE_2D, true);

   //glEnable(GL_TEXTURE_2D);
		
	glMatrixMode(GL_MODELVIEW);
				
	// enable vertex arrays and normal arrays
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);			

   //EnableLockArrays();

   //CVector2f SunDir;
   //SunDir.x = 1.0f;
   //SunDir.y = 1.0f;

   //OBJToPTG("resources\\models\\rocket.obj", "resources\\images\\missile.bmp",
   //         "resources\\models\\missile.ptg", false, 1.0f, 1.0f, 1.0f);

   //HeightMapToPTT("resources\\levels\\flat.raw", "resources\\images\\rock.tga",
   //               "resources\\levels\\flat.ptt", 17, 1.0f, .5f, 1.0f, &SunDir, .8f, .8f, .2f);   

   GameMain.LoadObjects();
  // g_ObjectFactory.SetupObjects();
   //g_Player.SetupPlayer();

   
   m_pSkyBox = new CSkyBox("resources\\images\\mtnbk.tga", "resources\\images\\mtnft.tga",
                           "resources\\images\\mtnlf.tga", "resources\\images\\mtnrt.tga",
                           "resources\\images\\mtnup.tga", "resources\\images\\mtndn.tga",
                           20.0f);
   
   INITCOMMONCONTROLSEX CommonControls;
   CommonControls.dwSize = sizeof(CommonControls);
   CommonControls.dwICC = ICC_BAR_CLASSES;
   InitCommonControlsEx(&CommonControls);
   // set up the editor win32 interface   
   m_ToolboxHandle = CreateDialog(pGLWIN->GetHINSTANCE(),
                               MAKEINTRESOURCE(IDD_TOOLBOX), pGLWIN->GetHWND(),
                               ToolboxDialogProc);

   // set up the Tool names
   char ppToolNames[NUM_TOOLS][256] = { "Deform", "Flatten", "Object", "Waypoint",
                                        "Utilities" };
   
   m_pToolDialogs[DEFORM].Initialize(DeformDialogProc, IDD_DEFORM);
   m_pToolDialogs[FLATTEN].Initialize(FlattenDialogProc, IDD_FLATTEN);
   m_pToolDialogs[OBJECT].Initialize(ObjectDialogProc, IDD_OBJECT);
   m_pToolDialogs[UTILITIES].Initialize(UtilitiesDialogProc, IDD_UTILITIES);
   m_pToolDialogs[WAYPOINT].Initialize(WaypointDialogProc, IDD_WAYPOINT);

   m_DeformRadius = 10.0f;
   m_DeformHeight = .5f;

   m_FlattenWidth = 10.0f;
   m_FlattenHeight = 10.0f;

   // set up the deform's track bars
   SendMessage(GetDlgItem(m_pToolDialogs[DEFORM].m_DialogHandle, IDC_DEFORM_HEIGHT_SLIDER),
               TBM_SETRANGE, 0, MAKELONG(-100, 100));
   SendMessage(GetDlgItem(m_pToolDialogs[DEFORM].m_DialogHandle, IDC_DEFORM_HEIGHT_SLIDER),
               TBM_SETTICFREQ, (WPARAM)50, 0);
   SendMessage(GetDlgItem(m_pToolDialogs[DEFORM].m_DialogHandle, IDC_DEFORM_RADIUS_SLIDER),
               TBM_SETRANGE, 0, MAKELONG(1, 100));
   SendMessage(GetDlgItem(m_pToolDialogs[DEFORM].m_DialogHandle, IDC_DEFORM_RADIUS_SLIDER),
               TBM_SETTICFREQ, (WPARAM)25, 0);

   SendMessage(GetDlgItem(m_pToolDialogs[DEFORM].m_DialogHandle, IDC_DEFORM_HEIGHT_SLIDER),
               TBM_SETPOS, (WPARAM)TRUE, LPARAM(m_DeformHeight * .5f));
   SendMessage(GetDlgItem(m_pToolDialogs[DEFORM].m_DialogHandle, IDC_DEFORM_RADIUS_SLIDER),
               TBM_SETPOS, (WPARAM)TRUE, (LPARAM)m_DeformRadius);
   
   // set up the flatten's track bars
   SendMessage(GetDlgItem(m_pToolDialogs[FLATTEN].m_DialogHandle, IDC_FLATTEN_HEIGHT_SLIDER),
               TBM_SETRANGE, 0, MAKELONG(1, 100));
   SendMessage(GetDlgItem(m_pToolDialogs[FLATTEN].m_DialogHandle, IDC_FLATTEN_HEIGHT_SLIDER),
               TBM_SETTICFREQ, (WPARAM)25, 0);
   SendMessage(GetDlgItem(m_pToolDialogs[FLATTEN].m_DialogHandle, IDC_FLATTEN_WIDTH_SLIDER),
               TBM_SETRANGE, 0, MAKELONG(1, 100));
   SendMessage(GetDlgItem(m_pToolDialogs[FLATTEN].m_DialogHandle, IDC_FLATTEN_WIDTH_SLIDER),
               TBM_SETTICFREQ, (WPARAM)25, 0);
   
   SendMessage(GetDlgItem(m_pToolDialogs[FLATTEN].m_DialogHandle, IDC_FLATTEN_HEIGHT_SLIDER),
               TBM_SETPOS, (WPARAM)TRUE, (LPARAM)m_FlattenHeight);
   SendMessage(GetDlgItem(m_pToolDialogs[FLATTEN].m_DialogHandle, IDC_FLATTEN_WIDTH_SLIDER),
               TBM_SETPOS, (WPARAM)TRUE, (LPARAM)m_FlattenWidth);

   // set up the waypoint's track bars
   SendMessage(GetDlgItem(m_pToolDialogs[WAYPOINT].m_DialogHandle, IDC_PITCH),
               TBM_SETRANGE, 0, MAKELONG(1, 100));
   SendMessage(GetDlgItem(m_pToolDialogs[WAYPOINT].m_DialogHandle, IDC_PITCH),
               TBM_SETTICFREQ, (WPARAM)25, 0);
   
   
   SendMessage(GetDlgItem(m_pToolDialogs[WAYPOINT].m_DialogHandle, IDC_YAW),
               TBM_SETRANGE, 0, MAKELONG(1, 100));
   SendMessage(GetDlgItem(m_pToolDialogs[WAYPOINT].m_DialogHandle, IDC_YAW),
               TBM_SETTICFREQ, (WPARAM)25, 0);

   SendMessage(GetDlgItem(m_pToolDialogs[WAYPOINT].m_DialogHandle, IDC_ROLL),
               TBM_SETRANGE, 0, MAKELONG(1, 100));
   SendMessage(GetDlgItem(m_pToolDialogs[WAYPOINT].m_DialogHandle, IDC_ROLL),
               TBM_SETTICFREQ, (WPARAM)25, 0);
   
   //SendMessage(GetDlgItem(m_pToolDialogs[WAYPOINT].m_DialogHandle, IDC_ROLL),
   //            TBM_SETPOS, (WPARAM)TRUE, (LPARAM)m_WAYPOINTHeight);
   //SendMessage(GetDlgItem(m_pToolDialogs[WAYPOINT].m_DialogHandle, IDC_ROLL),
   //            TBM_SETPOS, (WPARAM)TRUE, (LPARAM)m_FlattenWidth);


   // set up utils edit box
   char Buffer[4];
   SetDlgItemText(m_pToolDialogs[UTILITIES].m_DialogHandle, IDC_NUM_TREES_EDIT,
                  itoa(100, Buffer, 10));
   
   // set up toolbox combo box
   for (unsigned int i = 0; i < NUM_TOOLS; ++i)
   {
      SendDlgItemMessage(m_ToolboxHandle, IDC_CURRENT_TOOL, CB_ADDSTRING, 0,
                         (LPARAM)(LPCSTR)ppToolNames[i]);
   }

   // set up object combo box
   char ppEditorObjectNames[NUM_EDITOR_OBJECTS][256] =
   {
      "Player", "Medium Tank", "Heavy Tank", "Missile Tank", "Kamikaze", "Sphere", "Flyer", "Scout",
      "Enemy Turret", "Friendly Turret", "Building 1", "Building 2", "Building 3", "Tree 1",
      "Tree 2", "Tree 3", "Shield Power-up", "Missile Pack", "Tracking Missile Pack",
      "Tracking Missile Power-up", "Missile Power-up", "Plasma Power-up", "Rock 1",
      "Rock 2", "Rock 3", "Rock 4", "Rock 5", "Rock 6" 
   };
   
   for (i = 0; i < NUM_EDITOR_OBJECTS; ++i)
   {
      SendDlgItemMessage(m_pToolDialogs[OBJECT].m_DialogHandle, IDC_CURRENT_OBJECT, CB_ADDSTRING, 0,
                         (LPARAM)(LPCSTR)ppEditorObjectNames[i]);
   }

   SendDlgItemMessage(m_pToolDialogs[OBJECT].m_DialogHandle, IDC_CURRENT_OBJECT, CB_SETCURSEL, 0,
                      0);

   SendDlgItemMessage(m_ToolboxHandle, IDC_CURRENT_TOOL, CB_SETCURSEL, 0, 0);    
   
   m_CurrentTool = DEFORM;   

   m_pToolDialogs[m_CurrentTool].Enable();
   
   m_MousePos.x = 0.0f;
   m_MousePos.y = 0.0f;
   m_MousePos.z = 0.0f;
   
   m_CurrentObjectType = OBJ_PLAYER;

   m_LastClickTime = 0.0f;
   m_ClickDelay = .25f;

   m_pCurrentObject = 0;          

   static char pFileFilter[] = "Prototype Level Files (*.PTL)\0*.ptl\0\0"; 
   //static char pDefaultPath[] = "\\resources\\levels\\";
   static char pDefaultExtension[] = ".ptl";

   m_OpenFile.lStructSize = sizeof(m_OpenFile);
   m_OpenFile.hwndOwner = pGLWIN->GetHWND();
   m_OpenFile.hInstance = 0;
   m_OpenFile.lpstrFilter = pFileFilter;
   m_OpenFile.lpstrCustomFilter = 0;
   m_OpenFile.nMaxCustFilter = 0;
   m_OpenFile.nFilterIndex = 0;
   m_OpenFile.lpstrFile = m_pLevelFilePath;
   m_OpenFile.nMaxFile = PT_FILE_PATH_LEN;
   m_OpenFile.lpstrFileTitle = NULL;
   m_OpenFile.nMaxFileTitle = 0;
   m_OpenFile.lpstrInitialDir = 0;//pDefaultPath;
   m_OpenFile.lpstrTitle = NULL;
   m_OpenFile.Flags = OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
   m_OpenFile.nFileExtension = 0;
   m_OpenFile.nFileOffset = 0;
   m_OpenFile.lpstrDefExt = pDefaultExtension;
   m_OpenFile.lCustData = 0;
   m_OpenFile.lpfnHook = 0;
   m_OpenFile.lpTemplateName = 0;

   m_Saved = false;

   m_LastRotation = 0.0f;

   ShowCursor(true);

   m_LastClickTime = g_time;
   m_ClickDelay = .1f;

   m_Quitting = false;
   
   m_SavedLatest = true;

   m_MovingObject = false;
   
   CInputManager::GetInstance()->SetTimerInput(0.0f);

   /*
   CVector2f SunDir;
   SunDir.x = 1.0f;
   SunDir.y = 1.0f;
   HeightMapToPTT("resources\\levels\\height2.raw", "resources\\images\\greystone5.bmp",
                  "resources\\levels\\testbig.ptt", 17, 50, 4.0f, .4f, 4.0f, &SunDir, .8f, .8f,
                  .2f);
   */
   
   /*
   FILE* pTerrain = fopen("resources\\levels\\testbig.ptt", "rb");

   g_pCurrLevel->Initialize(pTerrain);
   fclose(pTerrain);

   AABB TerrainAABB;
   g_pCurrLevel->GetAABB(&TerrainAABB);
   CVector2f Max;
   Max.x = TerrainAABB.Max.x;
   Max.y = TerrainAABB.Max.z;
   CVector2f Min;
   Min.x = TerrainAABB.Min.x;
   Min.y = TerrainAABB.Min.z;
   g_Quadtree.Initialize(&Min, &Max, TerrainAABB.Min.y - 100.0f, TerrainAABB.Max.y + 100.0f, 7,
                         &g_Frustum, m_pRenderList);         

   g_pCurrLevel->AddPatchesToQuadtree(&g_Quadtree);   

   SaveLevel("resources\\levels\\blank.ptl", 0);
   */

   //LoadLevel("resources\\levels\\testbig.ptl", 17, m_pRenderList, &m_NumObjects);
   //m_Loaded = true;   

   if (m_Testing)
   {
      m_Loaded = true;
      m_Saved = true;
      m_SavedLatest = true;
      LoadLevel(m_pLevelFilePath, m_pRenderList, &m_NumObjects);
      m_Testing = false;

      g_FirstPersonCamera.SetPosition(&m_LastCameraPos);
   }

   char TempBuffer[256];
   g_Camera->GetPosition(&m_LastCameraPos);
   sprintf(TempBuffer, "x %.2f y %.2f z %.2f", m_LastCameraPos.x, m_LastCameraPos.y,
          m_LastCameraPos.z);
   SetWindowText(GetDlgItem(m_ToolboxHandle, IDC_POSITION), TempBuffer);
}

//////////////////////////////////////////////////////////////////////////
/// Main loop for our game
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CEditorMain::Main()
{
   switch (m_CurrentEditorState)
   {
      case EDITOR_RUNNING:
      
         GetInput();
         Update();
         Render();

         return NO_CHANGE;

      case ENTER_GAME:
         return GAME;

      case GAME_EXIT:
      {
         GameMain.m_CurrentLevel = CGameMain::Level1;
         return MENU;
      }
   }

   return NO_CHANGE;
}


//////////////////////////////////////////////////////////////////////////
/// Get Input from our Input Manager
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CEditorMain::GetInput()
{
   if (!m_Loaded)
      return;

   CInputManager::GetInstance()->PollMouse();
   CInputManager::GetInstance()->PollKeyboard();

   CVector3f MouseMove(0.0f, 0.0f, 0.0f);
   CInputManager::GetInstance()->PollMouseMove(MouseMove.x, MouseMove.y, MouseMove.z);
   m_MousePos.x += MouseMove.x;
   m_MousePos.y += MouseMove.y;
   m_MousePos.z = MouseMove.z;

   float Velocity = 100.0f;

   if (CInputManager::GetInstance()->GetKey(DIK_LCONTROL))
   {
      Velocity *= 4.0f;
   }

   float Speed = Velocity * g_FrameTime;
   float AngularSpeed = 2.0f * g_FrameTime;

   if (CInputManager::GetInstance()->GetKey(DIK_LSHIFT))
   {
      g_FirstPersonCamera.MoveUp(-Speed);

      char TempBuffer[256];
      g_Camera->GetPosition(&m_LastCameraPos);
      sprintf(TempBuffer, "x %.2f y %.2f z %.2f", m_LastCameraPos.x, m_LastCameraPos.y,
             m_LastCameraPos.z);
      SetWindowText(GetDlgItem(m_ToolboxHandle, IDC_POSITION), TempBuffer);
   }
   else if (CInputManager::GetInstance()->GetKey(DIK_SPACE))
   {
      g_FirstPersonCamera.MoveUp(Speed);

      char TempBuffer[256];
      g_Camera->GetPosition(&m_LastCameraPos);
      sprintf(TempBuffer, "x %.2f y %.2f z %.2f", m_LastCameraPos.x, m_LastCameraPos.y,
              m_LastCameraPos.z);
      SetWindowText(GetDlgItem(m_ToolboxHandle, IDC_POSITION), TempBuffer);
   }

   POINT MousePos;
   GetCursorPos(&MousePos);
   CVector2f Center;   
   static float CenterX = 0.0f;
   static float CenterY = 0.0f;
   
   if (CInputManager::GetInstance()->GetKey(DIK_DELETE))
   {
      if (m_pCurrentObject)
      {         
         GameMain.RemoveFromObjList(m_pCurrentObject);
         m_pCurrentObject->RemoveFromQuadtree();
         
         if (m_pCurrentObject == &g_Player)
            m_PlayerStart = false;
         else
            g_ObjectFactory.ReturnAndNoRefund(m_pCurrentObject);
         
         m_pCurrentObject = 0;

         m_NumObjects--;
      }
   }

   if (!CInputManager::GetInstance()->PollMouseRClick())
      m_MovingObject = false;      

   if ((CInputManager::GetInstance()->PollMouseClick() ||
       CInputManager::GetInstance()->PollMouseRClick())
       && m_LastClickTime + m_ClickDelay < g_time)
   {      
      // make sure the mouse isn't inside of the dialog box
      GetWindowRect(m_ToolboxHandle, &m_ToolboxRect);           
      GetWindowRect(m_pToolDialogs[m_CurrentTool].m_DialogHandle, &m_ToolRect);
            
      if (!RectToPoint(&m_ToolRect, &MousePos) && !RectToPoint(&m_ToolboxRect, &MousePos))
      {
         m_LastClickTime = g_time;
                 
         double ProjectionMatrix[16];
         glGetDoublev(GL_PROJECTION_MATRIX, ProjectionMatrix);

         int Viewport[4];
         glGetIntegerv(GL_VIEWPORT, Viewport);

         // find the start point of the ray's direction
         double OriginX;
         double OriginY;
         double OriginZ;
         gluUnProject(MousePos.x, pGLWIN->GetScreenHeight() - MousePos.y, .1f, m_ModelMatrix, ProjectionMatrix,
                      Viewport, &OriginX,
                      &OriginY, &OriginZ);
   
         // find the destination of the ray's direction
         double DestX;
         double DestY;
         double DestZ;
         gluUnProject(MousePos.x, pGLWIN->GetScreenHeight() - MousePos.y, 1.0f, m_ModelMatrix,
                      ProjectionMatrix, Viewport, &DestX, &DestY,
                      &DestZ);
   
         // calculate the ray's direction and normalize it
         m_SelectionRay.Direction.x = float(DestX - OriginX);
         m_SelectionRay.Direction.y = float(DestY - OriginY);
         m_SelectionRay.Direction.z = float(DestZ - OriginZ);
         
         Vec3fNormalize(&m_SelectionRay.Direction, &m_SelectionRay.Direction);

         // the ray's origin is the camera position
         g_FirstPersonCamera.GetPosition(&m_SelectionRay.Origin);

         if (CInputManager::GetInstance()->PollMouseRClick())
         {
            if (m_MovingObject)
            {
               if (m_pCurrentObject)
               {
                  unsigned int NumPatches = g_pCurrLevel->GetNumPatches();
                  CPatch* pPatches = g_pCurrLevel->GetPatches();

                  m_PatchList.SetCameraPos(&m_SelectionRay.Origin);
                  for (unsigned int i = 0; i < NumPatches; ++i)
                  {
                     AABB PatchAABB;
                     pPatches[i].GetAABB(&PatchAABB);
                     if (RayToAABB(&m_SelectionRay, &PatchAABB))
                     {               
                        // add the patch to the list in the proper place
                        m_PatchList.AddPatch(&pPatches[i]);                                        
                     }
                  }         

                  // do the collision
                  if (!m_PatchList.IsEmpty())
                  {
                     if (m_PatchList.RayCollisionPoint(&m_SelectionRay, &m_RayCollidePoint))
                     {
                        m_SavedLatest = false;
                        CVector3f LastPosition;
                        m_pCurrentObject->GetPosition(&LastPosition, 0); 
                        // move the current object to the mouse location in the terrain
                        m_pCurrentObject->SetPosition(&m_RayCollidePoint, 0);
                        
                        m_pCurrentObject->FollowTerrain();

                        // check to make sure we aren't colliding with anything
                        CGameObject* pCurr = GameMain.m_ObjList;

                        while (pCurr)
                        {
                           if (pCurr != m_pCurrentObject && OBBToOBB(m_pCurrentObject->GetOBB(),
                                        pCurr->GetOBB()))
                           {
                              m_pCurrentObject->SetPosition(&LastPosition, 0);
                              
                              m_pCurrentObject->FollowTerrain();
                           
                              break;
                           }
                           
                           pCurr = pCurr->m_Next;   
                        }

                     
                     }

                     m_PatchList.Clear();
                  }
               }
            }
            else
            {
               // check for collisions with the game objects
               CGameObject* pGameObject = GameMain.m_ObjList;
               CGameObject* pClosestObject = 0;
            
               // get the distance of the closest object
               float BestDistSq;
               float DistSq;
               Point3f CameraPosition;
               CVector3f DistanceVector;
               /*
               if (pClosestObject)
               {
                  g_Camera->GetPosition(&CameraPosition);
                  Vec3fSubtract(&DistanceVector, &pGameObject->GetSphere()->Center, &CameraPosition); 
                  BestDistSq = Vec3fMagnitudeSquare(&DistanceVector);
               }
               else*/
                  BestDistSq = -1;

               while (pGameObject)
               {
                  if (RayToSphere(&m_SelectionRay, pGameObject->GetSphere()))
                  {
                     g_Camera->GetPosition(&CameraPosition);
                     Vec3fSubtract(&DistanceVector, &pGameObject->GetSphere()->Center,
                                   &CameraPosition); 
                     DistSq = Vec3fMagnitudeSquare(&DistanceVector);
                  
                     if (BestDistSq != -1)
                     {
                        if (DistSq < BestDistSq)
                        {
                           BestDistSq = DistSq;
                           pClosestObject = pGameObject;
                        }
                     }
                     else
                     {
                        BestDistSq = DistSq;
                        pClosestObject = pGameObject;
                     }
                  }

                  pGameObject = pGameObject->m_Next;
               }

               if (pClosestObject)
               {
                  if (m_CurrentTool != OBJECT)
                  {
                     // disable the current Tool
                     m_pToolDialogs[m_CurrentTool].Disable();

                     // change the current Tool
                     m_CurrentTool = OBJECT;
                  
                     SendDlgItemMessage(m_ToolboxHandle, IDC_CURRENT_TOOL, CB_SETCURSEL, OBJECT, 0);
               
                     // enable the new one
                     m_pToolDialogs[m_CurrentTool].Enable();
                  }

                  m_pCurrentObject = pClosestObject;

                  m_MovingObject = true;
               }
               else
                  m_pCurrentObject = 0;
            }
         }
         else
         {

            unsigned int NumPatches = g_pCurrLevel->GetNumPatches();
            CPatch* pPatches = g_pCurrLevel->GetPatches();

            m_PatchList.SetCameraPos(&m_SelectionRay.Origin);
            for (unsigned int i = 0; i < NumPatches; ++i)
            {
               AABB PatchAABB;
               pPatches[i].GetAABB(&PatchAABB);
               if (RayToAABB(&m_SelectionRay, &PatchAABB))
               {               
                  // add the patch to the list in the proper place
                  m_PatchList.AddPatch(&pPatches[i]);                                        
               }
            }         

            // do the collision
            if (!m_PatchList.IsEmpty())
            {
               if (m_PatchList.RayCollisionPoint(&m_SelectionRay, &m_RayCollidePoint))
               {         
                  // deform the terrain at the point
                  m_SelectionPoint.x = m_RayCollidePoint.x;
                  m_SelectionPoint.y = m_RayCollidePoint.z;
               
                  CVector2f Min;
                  CVector2f Max;

                  m_SavedLatest = false;

                  CGameObject* pCurrentObject = GameMain.m_ObjList;

                  CGameObject* pLastObject = m_pCurrentObject;

                  switch (m_CurrentTool)
                  {
                  

                     case DEFORM:
                        g_pCurrLevel->DeformCircle(&m_SelectionPoint, m_DeformRadius,
                                                   m_DeformHeight);
                        // loop through all the game objects and place them
                        // oriented above the terrain                        
                        while (pCurrentObject)
                        {
                           pCurrentObject->FollowTerrain();

                           pCurrentObject = pCurrentObject->m_Next;
                        }
                        break;
                     case FLATTEN:
                     
                     
                        Min.x = -m_FlattenWidth * .5f + m_RayCollidePoint.x;
                        Min.y = -m_FlattenHeight * .5f + m_RayCollidePoint.z;
                     
                     
                        Max.x = m_FlattenWidth * .5f + m_RayCollidePoint.x;
                        Max.y = m_FlattenHeight * .5f + m_RayCollidePoint.z;

                        g_pCurrLevel->FlattenSquare(&Min, &Max);
                        
                        // loop through all the game objects and place them
                        // oriented above the terrain                        
                        while (pCurrentObject)
                        {                   
                           pCurrentObject->FollowTerrain();

                           pCurrentObject = pCurrentObject->m_Next;
                        }
                        break;
                     case OBJECT:
                        m_NumObjects++;
                        CObjFrame PlayerFrame;
                              
                        switch (m_CurrentObjectType)
                        {
                           
                           case OBJ_PLAYER:
                              m_pCurrentObject = &g_Player;
                              g_Player.GetRigidBody()->SetTerrainOffset(1.0f);
                              g_Player.SetFrame(&PlayerFrame, 0);

                              if (!m_PlayerStart)
                              {
                                 g_Quadtree.AddObject(g_Player.GetRenderObj());
                                 GameMain.AddToObjList(&g_Player);
                                 
                                 m_PlayerStart = true;                                 
                              }
                              else
                                 m_NumObjects--;
                              break;
                                    
                           case OBJ_MEDTANK:
                              m_pCurrentObject = g_ObjectFactory.CreateMediumTank();  
                              break;
                           case OBJ_HVYTANK:
                              m_pCurrentObject = g_ObjectFactory.CreateHeavyTank();
                              break;
                           case OBJ_MSLETANK:
                              m_pCurrentObject = g_ObjectFactory.CreateMissileTank();  
                              break;
                           case OBJ_KAMITANK:
                              m_pCurrentObject = g_ObjectFactory.CreateMeleeTank();  
                              break;
                           case OBJ_SPHERE:
                              m_pCurrentObject = g_ObjectFactory.CreateSphere();
                              break;
                           case OBJ_FTURRET:
                              m_pCurrentObject = g_ObjectFactory.CreateFriendlyTurret();

                              break;
                           case OBJ_ETURRET:
                              m_pCurrentObject = g_ObjectFactory.CreateEnemyTurret();  

                              break;
                           case OBJ_TREE:
                              m_pCurrentObject = g_ObjectFactory.CreateTree();
  
                              break;
                           case OBJ_TREE2:
                              m_pCurrentObject = g_ObjectFactory.CreateTree2();  

                              break;
                           case OBJ_TREE3:
                              m_pCurrentObject = g_ObjectFactory.CreateTree3();  

                              break;
                           case OBJ_BUILDING:
                              m_pCurrentObject = g_ObjectFactory.CreateBuilding();
                                
                              break;
                           case OBJ_BUILDING2:
                              m_pCurrentObject = g_ObjectFactory.CreateBuilding2();

                              break;
                           case OBJ_BUILDING3:
                              m_pCurrentObject = g_ObjectFactory.CreateBuilding3();

                              break;
                           case OBJ_SHDPWRUP:
                              m_pCurrentObject = g_ObjectFactory.CreateShieldPowerUp();
                              break;
                           case OBJ_TRKMSLPWRUP:
                              m_pCurrentObject = g_ObjectFactory.CreateTrkMissileUpgrade();
                              break;
                           case OBJ_MSLPWRUP:
                              m_pCurrentObject = g_ObjectFactory.CreateMissileUpgrade();
                              break;
                           case OBJ_PLASMAPWRUP:
                              m_pCurrentObject = g_ObjectFactory.CreatePlasmaUpgrade();
                              break; 
                           case OBJ_FIGHTER:
                              m_pCurrentObject = g_ObjectFactory.CreateFighter();
                              break;
                           case OBJ_MSLPACK:
                              m_pCurrentObject = g_ObjectFactory.CreateMissilePack();
                              break;
                           case OBJ_TRKMSLPACK:
                              m_pCurrentObject = g_ObjectFactory.CreateTrackingMissilePack();
                              break;
                           case OBJ_SCOUT:
                              m_pCurrentObject = g_ObjectFactory.CreateScout();
                              break;
                           case OBJ_ROCK1:
                              m_pCurrentObject = g_ObjectFactory.CreateRock1();
                              break;
                           case OBJ_ROCK2:
                              m_pCurrentObject = g_ObjectFactory.CreateRock2();
                              break;
                           case OBJ_ROCK3:
                              m_pCurrentObject = g_ObjectFactory.CreateRock3();
                              break;
                           case OBJ_ROCK4:
                              m_pCurrentObject = g_ObjectFactory.CreateRock4();
                              break;
                           case OBJ_ROCK5:
                              m_pCurrentObject = g_ObjectFactory.CreateRock5();
                              break;
                           case OBJ_ROCK6:
                              m_pCurrentObject = g_ObjectFactory.CreateRock6();
                              break;
                        }

                        
                        EditorMain.m_pCurrentObject->RotateLocalY(m_LastRotation, 0);
                        m_pCurrentObject->SetPosition(&m_RayCollidePoint, 0);

                        m_pCurrentObject->FollowTerrain();   

                        // check to make sure we aren't colliding with anything
                        CGameObject* pCurr = GameMain.m_ObjList;

                        while (pCurr)
                        {
                           if (pCurr != m_pCurrentObject && OBBToOBB(m_pCurrentObject->GetOBB(),
                                        pCurr->GetOBB()))
                           {
                              GameMain.RemoveFromObjList(m_pCurrentObject);
                              m_pCurrentObject->RemoveFromQuadtree();
         
                              if (m_pCurrentObject == &g_Player)
                                 m_PlayerStart = false;
                              else
                                 g_ObjectFactory.ReturnAndNoRefund(m_pCurrentObject);
         
                              m_pCurrentObject = pLastObject;

                              m_NumObjects--;
                              
                              break;                           
                           }
                           
                           pCurr = pCurr->m_Next;   
                        }

                        break;
                  }               
               }

               m_PatchList.Clear();
            }
         }
      }
   }

   /*
   CVector3f Cam;
   g_FirstPersonCamera.GetPosition(&Cam);

   if (Cam.x < 0)
      Beep(100, 100);

   if (Cam.z < 0)
      Beep(100, 100);
   */

   if (CInputManager::GetInstance()->GetKey(DIK_W))
   {
      g_FirstPersonCamera.MoveForward(Speed);

      char TempBuffer[256];
      g_Camera->GetPosition(&m_LastCameraPos);
      sprintf(TempBuffer, "x %.2f y %.2f z %.2f", m_LastCameraPos.x, m_LastCameraPos.y,
             m_LastCameraPos.z);
      SetWindowText(GetDlgItem(m_ToolboxHandle, IDC_POSITION), TempBuffer);
   }

   else if (CInputManager::GetInstance()->GetKey(DIK_S))
   {
      g_FirstPersonCamera.MoveForward(-Speed);
   
      char TempBuffer[256];
      g_Camera->GetPosition(&m_LastCameraPos);
      sprintf(TempBuffer, "x %.2f y %.2f z %.2f", m_LastCameraPos.x, m_LastCameraPos.y,
             m_LastCameraPos.z);
      SetWindowText(GetDlgItem(m_ToolboxHandle, IDC_POSITION), TempBuffer);
   }

   else if (CInputManager::GetInstance()->GetKey(DIK_A))
   {
      g_FirstPersonCamera.MoveRight(-Speed);
   
      char TempBuffer[256];
      g_Camera->GetPosition(&m_LastCameraPos);
      sprintf(TempBuffer, "x %.2f y %.2f z %.2f", m_LastCameraPos.x, m_LastCameraPos.y,
             m_LastCameraPos.z);
      SetWindowText(GetDlgItem(m_ToolboxHandle, IDC_POSITION), TempBuffer);
   }
   else if (CInputManager::GetInstance()->GetKey(DIK_D))
   {
      g_FirstPersonCamera.MoveRight(Speed);
   
      char TempBuffer[256];
      g_Camera->GetPosition(&m_LastCameraPos);
      sprintf(TempBuffer, "x %.2f y %.2f z %.2f", m_LastCameraPos.x, m_LastCameraPos.y,
             m_LastCameraPos.z);
      SetWindowText(GetDlgItem(m_ToolboxHandle, IDC_POSITION), TempBuffer);
   }

   if (MousePos.y >= pGLWIN->GetScreenHeight() - 1)
   {
      g_FirstPersonCamera.TurnUp(AngularSpeed);
   }
   if (MousePos.y <= 0)
   {
      g_FirstPersonCamera.TurnUp(-AngularSpeed);
   }
   if (MousePos.x <= 0)
   {
      g_FirstPersonCamera.TurnRight(-AngularSpeed);
   }
   if (MousePos.x >= pGLWIN->GetScreenWidth() - 1)
   {
      g_FirstPersonCamera.TurnRight(AngularSpeed);
   }

   /*
   if (GetAsyncKeyState('I'))
      g_pCurrLevel->SlopeLight();

   if (GetAsyncKeyState('L'))
   {
      pGLSTATE->SetState(TEXTURE_2D, false);
	   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   }
   else if (GetAsyncKeyState('P'))
   {
      pGLSTATE->SetState(TEXTURE_2D, true);
	   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);			
   }
   */

   /*
   if (GetAsyncKeyState(VK_DOWN))
      m_DeformHeight -= .1f;
   else if (GetAsyncKeyState(VK_UP))
      m_DeformHeight += .1f;

   if (GetAsyncKeyState(VK_RIGHT))
      m_DeformRadius += .4f;
   else if (GetAsyncKeyState(VK_LEFT))
      m_DeformRadius -= .4f;
   */
}

//////////////////////////////////////////////////////////////////////////
/// Render all objects in our world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CEditorMain::Render()
{
   if (!m_Loaded)
      return;	
	
	glPushMatrix();	
		g_FirstPersonCamera.ApplyTransform(); 
		
      g_Sunlight.Update();
  //    float SunDir[4];
  //    g_pCurrLevel->GetOpenGLSunDir(SunDir);
//      glLightfv(GL_LIGHT1, GL_POSITION, SunDir);

      //if (GetAsyncKeyState(VK_RETURN))
      //   g_Quadtree.DrawDebugLines();


      glGetDoublev(GL_MODELVIEW_MATRIX, m_ModelMatrix);

      /*
      glDisable(GL_LIGHTING);
      glLineWidth(4.0f);
      glPointSize(10.0f);
      glColor3f(1.0f, 0.0f, 0.0f);      

      int NumPatches = g_pCurrLevel->GetNumPatches();
      CPatch* pPatches = g_pCurrLevel->GetPatches();
      */
      /*
      for (unsigned int i = 0; i < NumPatches; ++i)
      {
         AABB PatchAABB;
         pPatches[i].GetAABB(&PatchAABB);
         DrawAABB(&PatchAABB);         
      }*/
      
      /*
         static int i = 0;

         if (GetAsyncKeyState(VK_DOWN))
            i--;
         else if (GetAsyncKeyState(VK_UP))
            i++;

         if (i >= NumPatches)
            i = NumPatches - 1;
         else if (i < 0)
            i = 0;
       */
       /*
      for (unsigned int i = 0; i < NumPatches; ++i)
      {
         AABB PatchAABB;
         pPatches[i].GetAABB(&PatchAABB);
         if (RayToAABB(&m_SelectionRay, &PatchAABB))
         {
            glColor3f(1.0f, 1.0f, 0.0f);
         //   DrawAABB(&PatchAABB);           
         }
      }
      */
      
      /*
      if (GetAsyncKeyState(VK_DOWN))
         m_RayCollidePoint.z--;
      else if (GetAsyncKeyState(VK_UP))
         m_RayCollidePoint.z++;

      if (GetAsyncKeyState(VK_RIGHT))
         m_RayCollidePoint.x++;
      else if (GetAsyncKeyState(VK_LEFT))
         m_RayCollidePoint.x--;
      */
      /*
      glDisable(GL_LIGHTING);
       glPointSize(5.0f);
       glColor3f(0.0f, 1.0f, 1.0f);
               glBegin(GL_POINTS);

                  glVertex3f(m_RayCollidePoint.x, m_RayCollidePoint.y, m_RayCollidePoint.z);

               glEnd();
               glColor3f(1.0f, 0.0f, 0.0f);
      

      DrawRay(&m_SelectionRay, 500.0f);
      */
      
      m_pSkyBox->Render();
		g_pCurrLevel->BeginRendering();		      
      g_Quadtree.CreateVisibleList();
		g_pCurrLevel->ResetPatchVisibility();
      g_pCurrLevel->ResetStates();
      m_pRenderList->Render();
		
      if (m_pCurrentObject)
      {
         glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_POINT_BIT);
         glEnable(GL_BLEND);
         glDisable(GL_LIGHTING);
         glDisable(GL_TEXTURE_2D);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

         glColor4f(1.0f, 0.0f, 0.0f, .3f);

         // draw their bounding sphere
         DrawSphere(m_pCurrentObject->GetSphere(), 10, 10);
         
         // draw their communication sphere
         Sphere EnemySphere;
         if (EnemySphere.Radius = m_pCurrentObject->GetAwakenEnemyRadius())
         {
            m_pCurrentObject->GetPosition(&EnemySphere.Center, 0);
            glColor4f(0.0f, 1.0f, 0.0f, .3f);
            DrawSphere(&EnemySphere, 10, 10);
         }
         
         if (EnemySphere.Radius = m_pCurrentObject->GetAwakenPlayerRadius())
         {
            // draw their player sight sphere
            m_pCurrentObject->GetPosition(&EnemySphere.Center, 0);
            glColor4f(0.0f, 0.0f, 1.0f, .3f);
            DrawSphere(&EnemySphere, 10, 10);
         }
         
         glPopAttrib();
      }

	glPopMatrix();
  
}

//////////////////////////////////////////////////////////////////////////
/// Update all objects in our world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CEditorMain::Update()
{
   if (m_Quitting)
   {
	   m_CurrentEditorState = GAME_EXIT;	
   }

   if (!m_Loaded)
      return 0;

	g_Frustum.TransformFrustum(g_Camera);
   
   // update all the game object's render objects for the quadtree
   CGameObject* pGameObject = GameMain.m_ObjList;
   
   while (pGameObject)
   {
      pGameObject->GetRenderObj()->Update();

      pGameObject = pGameObject->m_Next;
   }
   
   if (m_pCurrentObject)
   {
      char TempBuffer[256];
      CVector3f Pos;
      m_pCurrentObject->GetPosition(&Pos, 0);
      sprintf(TempBuffer, "x %.2f y %.2f z %.2f", Pos.x, Pos.y, Pos.z);
      SetWindowText(GetDlgItem(m_pToolDialogs[OBJECT].m_DialogHandle,
                    IDC_OBJECT_POSITION), TempBuffer);
   }
   else
   {
      SetWindowText(GetDlgItem(m_pToolDialogs[OBJECT].m_DialogHandle,
                    IDC_OBJECT_POSITION), NULL);
   }      

	return 0;	
}

//////////////////////////////////////////////////////////////////////////
/// Lets clean up
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CEditorMain::Shutdown()
{
    g_Player.Unload();
    g_MotherShip.Unload();
	g_TextureManager.UnloadAll();
	g_GeometryManager.UnloadAll();

   delete g_pCurrLevel;
   delete m_pRenderList;
   delete m_pSkyBox;

   DeleteObjects();

	delete GameMain.m_pPlasmaObject;
   delete GameMain.m_pMedTankObject;
   delete GameMain.m_pMeleeTankObject;
   delete GameMain.m_pShieldPowerUpObject;
   delete GameMain.m_pMissileObject;
   delete GameMain.m_pIonCannonObject;
   delete GameMain.m_pTreeObject;
   delete GameMain.m_pTree2Object;
   delete GameMain.m_pTree3Object;
   delete GameMain.m_pMissilePackObject;
   delete GameMain.m_pTrackingMissilePackObject;
   delete GameMain.m_pTrackingMissileObject;
   delete GameMain.m_pScoutObject;
   delete GameMain.m_pBuildingObject;
   delete GameMain.m_pHeavyTankObject;
   delete GameMain.m_pMissileUpgradeObject;
   delete GameMain.m_pTrkMissileUpgradeObject;
   delete GameMain.m_pPlasmaUpgradeObject;
   delete GameMain.m_pConvoyObject;
   delete GameMain.m_pMissileTankObject;
   delete GameMain.m_pSphereObject;
   delete GameMain.m_pFighterObject;
   delete GameMain.m_pIonFlareObject;
   delete GameMain.m_pLaserObject;
   delete GameMain.m_pFriendlyTurretObject;
   delete GameMain.m_pBuildingObject2;
   delete GameMain.m_pBuildingObject3;
   delete GameMain.m_pEnemyTurretObject;
   delete GameMain.m_pRock1Object;
   delete GameMain.m_pRock2Object;
   delete GameMain.m_pRock3Object;
   delete GameMain.m_pRock4Object;
   delete GameMain.m_pRock5Object;
   delete GameMain.m_pRock6Object;
   delete GameMain.m_pBossObject;
   delete GameMain.m_pBombObject;

   EndDialog(m_ToolboxHandle, 0);

   for (unsigned int i = 0; i < NUM_TOOLS; ++i)
   {                             
      EndDialog(m_pToolDialogs[i].m_DialogHandle, 0);
   }

   g_GameOptions.AIOn();

   pGLWIN->DestroyWindowGL (&pGLWIN->window);
   pGLWIN->CreateWindowGL (&pGLWIN->window, g_GameOptions.GetVideoWidth(), g_GameOptions.GetVideoHeight(), 32, true); 
   pGLSTATE->Initialize();
   pGLWIN->OpenGLInit();
   
   ShowCursor(false);

    g_Quadtree.Unload();
}

void CEditorMain::DeleteObjects()
{
   g_ObjectFactory.Shutdown();

   // Clean up objects in the game list
	CGameObject *obj;
   while ((obj = GameMain.m_ObjList))
	{
		GameMain.m_ObjList = GameMain.m_ObjList->m_Next;

      if (obj != &g_Player)
		   delete obj;
	}
}