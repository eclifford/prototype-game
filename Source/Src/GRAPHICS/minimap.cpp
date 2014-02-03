// File: "minimap.cpp"
// Author(s): Jesse Edmunds

#include "graphics\minimap.h"
#include "graphics\quadtree.h"
#include "graphics\terrain.h"
#include "gamelogic\player.h"
#include "core\gamemain.h"
#include "graphics\drawshape.h"

#include "utils\mmgr.h"

// external globals
extern CQuadtree g_Quadtree;
extern CPlayer g_Player;
extern CTerrain* g_pCurrLevel;
extern COpenGLStateManager* pGLSTATE;
extern CGameMain GameMain;

// function used by the quadtree to render the minimap
void RenderMiniMap(CQuadtree::CObject* pObject)
{
      
   // if it's terrain, then render it
   if (pObject->IsTerrain())
   {
      // if we haven't rendered it before
      if (!pObject->IsVisible())
      {
         // set color to white
         glColor3f(1.0f, 1.0f, 1.0f);    
         
         // get the proper minimap patch and render it
         CPatch* pPatch = (CPatch*)pObject;
         g_pCurrLevel->m_pMiniMapIndices[pPatch->m_Index].Render();
         
         // set it visible so we know we already rendered it
         pObject->SetVisible(true);         
      }
   }
   // otherwise it's another object
   else
   {
      // check to make sure it's a valid game object
      CGameObject* pGameObject = pObject->m_pOwner;
      
      if (!pGameObject)
         return;

      // check for minimap flag
      if (pGameObject->m_InMiniMap)
         return;
      else
         pGameObject->m_InMiniMap = true;

      // black for convoy and buildings
      if (pGameObject->GetType() == OBJ_CONVOY || pGameObject->m_RadarType == CGameObject::RADAR_BUILDING)
         glColor3f(0.0f, 0.0f, 0.0f);         
      // blue for player
      else if (pGameObject->m_RadarType == CGameObject::RADAR_PLAYER)
         glColor3f(0.0f, 0.0f, 1.0f);
      // red for enemy
      else if (pGameObject->m_RadarType == CGameObject::RADAR_ENEMY)
         glColor3f(1.0f, 0.0f, 0.0f);
      // green for friendly
      else if (pGameObject->m_RadarType == CGameObject::RADAR_FRIENDLY)
         glColor3f(0.0f, 1.0f, 0.0f);
      else
         return;

      // draw the oriented bounding box of the object
      OBB ObjOBB = *pGameObject->GetOBB();
      DrawOBB(&ObjOBB);
   } 
}

// initialize the minimap
void CMiniMap::Initialize(float Radius)
{
   m_VisibilitySphere.Radius = Radius;

   m_Scale = .008f * 50.0f / m_VisibilitySphere.Radius;
}

// render the minimap
void CMiniMap::Render()
{
   // get the player's position for the center of our visibility sphere
   g_Player.GetPosition(&m_VisibilitySphere.Center, 0);

   // push the modelview matrix and load the identity to clear it out
   glPushMatrix();
   glLoadIdentity();
   m_Frame.Reset();

   // move the frame to the proper position on the screen
   m_Frame.SetScaleXYZ(m_Scale, m_Scale, m_Scale);
   m_Frame.RotateLocalY(PI);   
   m_Frame.MoveForward(2.0f);
   m_Frame.MoveRight(-.6f);
   m_Frame.MoveUp(.7f);
   m_Frame.RotateLocalX(-.61086523819801535192329176897101f);     

   // move the frame to the player's orientation
   CVector3f PlayerAngles;
   g_Player.GetFrame(0)->GetEulerAngles(&PlayerAngles);   
   m_Frame.RotateLocalY(-PlayerAngles.y);

   // move the frame to the proper position on the terrain
   m_Frame.MoveForward(-m_VisibilitySphere.Center.z * m_Scale);
   m_Frame.MoveRight(m_VisibilitySphere.Center.x * m_Scale);
   m_Frame.MoveUp(-m_VisibilitySphere.Center.y * m_Scale);

   // apply the frame transform
   float Matrix[16];
   m_Frame.GetOpenGLMatrix(Matrix);
   glMultMatrixf(Matrix);
   
   // create a stencil around the part of the terrain we want to render 
   glEnable(GL_STENCIL_TEST);
   
   glStencilFunc(GL_ALWAYS, 1, 1);
   glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

   glDepthMask(false);
   glColorMask(false, false, false, false);

   // draw a quad on the part of the minimap we want to render
   float Tolerance = .8f;
   glBegin(GL_QUADS);

   
   // lower left
   glVertex3f(m_VisibilitySphere.Center.x - m_VisibilitySphere.Radius * Tolerance,
              m_VisibilitySphere.Center.y,
              m_VisibilitySphere.Center.z - m_VisibilitySphere.Radius * Tolerance);

   // upper left
   glVertex3f(m_VisibilitySphere.Center.x - m_VisibilitySphere.Radius * Tolerance,
              m_VisibilitySphere.Center.y,
              m_VisibilitySphere.Center.z + m_VisibilitySphere.Radius * Tolerance);
   
   // upper right
   glVertex3f(m_VisibilitySphere.Center.x + m_VisibilitySphere.Radius * Tolerance,
              m_VisibilitySphere.Center.y,
              m_VisibilitySphere.Center.z + m_VisibilitySphere.Radius * Tolerance);

   // lower right
   glVertex3f(m_VisibilitySphere.Center.x + m_VisibilitySphere.Radius * Tolerance,
              m_VisibilitySphere.Center.y,
              m_VisibilitySphere.Center.z - m_VisibilitySphere.Radius * Tolerance);



   glEnd();

   // reset stencil states
   glColorMask(true, true, true, true);
   glDepthMask(true);
   glStencilFunc(GL_EQUAL, 1, 1);
   glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);   
   
   // set up states
   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(TEXTURE_2D, false);   
   g_pCurrLevel->EnableVerts();
   glDisableClientState(GL_COLOR_ARRAY);
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glLineWidth(.25f);

   // RENDER HERE   
   g_Quadtree.TraverseObjectsInSphere(RenderMiniMap, &m_VisibilitySphere);
   
   // reset the terrain visibility
   g_pCurrLevel->ResetPatchVisibility();

   // reset states
   glEnableClientState(GL_COLOR_ARRAY);
   glPopMatrix();
   glDisable(GL_STENCIL_TEST);
   pGLSTATE->SetState(TEXTURE_2D, true);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
