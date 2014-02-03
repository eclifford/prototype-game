// File: "quadtree.cpp"
// Author(s): Jesse Edmunds

#include "graphics/quadtree.h"
#include "graphics/renderlist.h"
#include "graphics/glinclude.h"
#include "graphics/texturemanager.h"
#include "graphics/geometrymanager.h"
#include "camera\camera.h"
#include <gl\GLU.h>
#include "camera\frustum.h"

#include "utils\mmgr.h"

// external globals
extern CTextureManager g_TextureManager;
extern CGeometryManager g_GeometryManager;
extern CBaseCamera* g_Camera;
extern CQuadtree g_Quadtree;

// check to see if it's transparent
bool CQuadtreeRenderObj::IsTransparent()
{
    if (m_Material.IsTransparent())
        return true;
    else if (m_TextureIndex != -1)
        return g_TextureManager.IsTransparent(m_TextureIndex);
    else
        return false;
}

// free memory
CQuadtree::~CQuadtree()
{
   Unload();
}

// free memory
void CQuadtree::Unload()
{
   delete m_pRoot;
   
   m_pRoot = 0;
}

// draw the quadtree
void CQuadtree::DrawDebugLines()
{
   DrawTree(m_pRoot);   
}

// free node memory
CQuadtree::CNode::~CNode()
{
   for (int i = 0; i < NUMCHILDREN; i++)
   {
      delete m_ppChildren[i];
   }
   
   m_ObjectList.clear();
}

// recursive draw tree function
void CQuadtree::DrawTree(CNode *pNode)
{
   // if we've reached a leaf then draw it
   if (!pNode->m_ppChildren[0])
   {
      pNode->Draw(m_MinY, m_MaxY);
      return;
   }
   // recurse to children
   else
   {
      for (int i = 0; i < NUMCHILDREN; i++)
      {
         DrawTree(pNode->m_ppChildren[i]);
      }
   }
}

// draw a node
void CQuadtree::CNode::Draw(float MinY, float MaxY) const
{
   // if we have objects, draw a point at our center
   if (m_ObjectList.size() != 0)
   {
      glBegin(GL_POINTS);      
      glVertex3f(m_Center.x, MinY, m_Center.y);
      glVertex3f(m_Center.x, MaxY, m_Center.y);
      glEnd();
   }
   
   glBegin(GL_LINE_LOOP);
   
   // bottom left
   glVertex3f(m_Min.x, MinY, m_Min.y);

   // bottom right
   glVertex3f(m_Max.x, MinY, m_Min.y);

   // top right
   glVertex3f(m_Max.x, MinY, m_Max.y);

   // top left
   glVertex3f(m_Min.x, MinY, m_Max.y);

   // line loop goes back to bottom left

   glEnd();

   glBegin(GL_LINE_LOOP);
   
   // bottom left
   glVertex3f(m_Min.x, MaxY, m_Min.y);

   // bottom right
   glVertex3f(m_Max.x, MaxY, m_Min.y);

   // top right
   glVertex3f(m_Max.x, MaxY, m_Max.y);

   // top left
   glVertex3f(m_Min.x, MaxY, m_Max.y);

   // line loop goes back to bottom left

   glEnd();


   
   // draw lines from bottom to top
   glBegin(GL_LINES);

   // bottom left
   glVertex3f(m_Min.x, MinY, m_Min.y);
   glVertex3f(m_Min.x, MaxY, m_Min.y);
   
   // bottom right
   glVertex3f(m_Max.x, MinY, m_Min.y);
   glVertex3f(m_Max.x, MaxY, m_Min.y);

   // top right
   glVertex3f(m_Max.x, MinY, m_Max.y);
   glVertex3f(m_Max.x, MaxY, m_Max.y);

   // top left
   glVertex3f(m_Min.x, MinY, m_Max.y);
   glVertex3f(m_Min.x, MaxY, m_Max.y);

   glEnd();
}

// init the tree
void CQuadtree::Initialize(const CVector2f* pMin, const CVector2f* pMax, float MinY,
                           float MaxY, unsigned int MaxDepth, CFrustum* pFrustum,
                           CRenderList* pVisibleList)
{
   m_MinY = MinY;
   m_MaxY = MaxY;
      
   // initialize the root
   m_pRoot = new CNode;
   m_pRoot->m_Max.x = pMax->x;
   m_pRoot->m_Max.y = pMax->y;
   m_pRoot->m_Min.x = pMin->x;
   m_pRoot->m_Min.y = pMin->y;

   m_pRoot->m_Center.x = (pMin->x + pMax->x) * .5f;
   m_pRoot->m_Center.y = (pMin->y + pMax->y) * .5f;
   
   m_pRoot->m_pParent = NULL;

   m_pFrustum = pFrustum;
   m_pVisibleList = pVisibleList;

   // create the tree
   SubdivideTree(MaxDepth, 0, m_pRoot);
}

// recursive function to divide the tree up
void CQuadtree::SubdivideTree(const unsigned int MaxDepth, const unsigned int CurrentDepth,
                              CNode* pNode)
{
   // check for max   
   if (CurrentDepth == MaxDepth)
   {
      return;
   }

   // create this node's children
   pNode->CreateChildren();

   // recurse to children
   for (int i = 0; i < NUMCHILDREN; i++)
   {
      SubdivideTree(MaxDepth, CurrentDepth + 1, pNode->m_ppChildren[i]);
   }
}

// add an object
void CQuadtree::AddObject(CObject* pObject)
{
   PlaceObject(pObject, m_pRoot);
}

// place an object
void CQuadtree::PlaceObject(CObject* pObject, CNode* pNode)
{
   // if we've reached a leaf then put it here and return
   if (!pNode->m_ppChildren[0])
   {
      pNode->m_ObjectList.push_back(pObject);
      pObject->AddQuadtreeNode(pNode);
      return;
   }      
   
   const Sphere* pSphere = pObject->GetSphere();      
   
   // if it's in one of the upper nodes
   if (pSphere->Center.z - pSphere->Radius > pNode->m_Center.y)
   {
      // if it's in the upper left
      if (pSphere->Center.x + pSphere->Radius < pNode->m_Center.x)
      {
         PlaceObject(pObject, pNode->m_ppChildren[UPLEFT]);              
      }
      // if it's in the upper right
      else if (pSphere->Center.x - pSphere->Radius > pNode->m_Center.x)
      {
         PlaceObject(pObject, pNode->m_ppChildren[UPRIGHT]);              
      }
      // if it's in the upper right and upper left
      else
      {
         PlaceObject(pObject, pNode->m_ppChildren[UPLEFT]);                            
         PlaceObject(pObject, pNode->m_ppChildren[UPRIGHT]);              
      }
   }
   // if it's in one of the lower nodes
   else if (pSphere->Center.z + pSphere->Radius < pNode->m_Center.y)
   {
      // if it's in the lower left
      if (pSphere->Center.x + pSphere->Radius < pNode->m_Center.x)
      {
         PlaceObject(pObject, pNode->m_ppChildren[LOWLEFT]);              
      }
      // if it's in the lower right
      else if (pSphere->Center.x - pSphere->Radius > pNode->m_Center.x)
      {
         PlaceObject(pObject, pNode->m_ppChildren[LOWRIGHT]);              
      }
      // if it's in the lower right and lower left
      else
      {
         PlaceObject(pObject, pNode->m_ppChildren[LOWLEFT]);
         PlaceObject(pObject, pNode->m_ppChildren[LOWRIGHT]);              
      }
   }
   // if its in the upper and lower nodes
   else
   {
      // if it's in the upper and lower left
      if (pSphere->Center.x + pSphere->Radius < pNode->m_Center.x)
      {
         PlaceObject(pObject, pNode->m_ppChildren[UPLEFT]);
         PlaceObject(pObject, pNode->m_ppChildren[LOWLEFT]);
      }
      // if it's in the lower and upper right
      else if (pSphere->Center.x - pSphere->Radius > pNode->m_Center.x)
      {
         PlaceObject(pObject, pNode->m_ppChildren[LOWRIGHT]);
         PlaceObject(pObject, pNode->m_ppChildren[UPRIGHT]);
      }
      // if it's in all four nodes
      else
      {
         PlaceObject(pObject, pNode->m_ppChildren[LOWLEFT]);
         PlaceObject(pObject, pNode->m_ppChildren[LOWRIGHT]);
         PlaceObject(pObject, pNode->m_ppChildren[UPLEFT]);
         PlaceObject(pObject, pNode->m_ppChildren[UPRIGHT]);
      }   
   }         
}

// apply a function to all objects in a sphere
void CQuadtree::TraverseObjectsInSphere(void (*pTraverseFunc)(CObject* pObject),
                                        const Sphere* pSphere)
{
   FindObjectsInSphere(m_pRoot, pTraverseFunc, pSphere);         
}

// recursive find objects in sphere
void CQuadtree::FindObjectsInSphere(CNode* pNode, void (*pTraverseFunc)(CObject*),
                                    const Sphere* pSphere)
{
   // if we've reached a leaf then do the traverse func on these objects
   if (!pNode->m_ppChildren[0])
   {
      unsigned int Size = (unsigned int)pNode->m_ObjectList.size();
      for (unsigned int i = 0; i < Size; ++i)
      {
         (*pTraverseFunc)(pNode->m_ObjectList[i]);
      }
      return;
   }            

   // if it's in one of the upper nodes
   if (pSphere->Center.z - pSphere->Radius > pNode->m_Center.y)
   {
      // if it's in the upper left
      if (pSphere->Center.x + pSphere->Radius < pNode->m_Center.x)
      {
         FindObjectsInSphere(pNode->m_ppChildren[UPLEFT], pTraverseFunc, pSphere);              
      }
      // if it's in the upper right
      else if (pSphere->Center.x - pSphere->Radius > pNode->m_Center.x)
      {
         FindObjectsInSphere(pNode->m_ppChildren[UPRIGHT], pTraverseFunc, pSphere);              
      }
      // if it's in the upper right and upper left
      else
      {
         FindObjectsInSphere(pNode->m_ppChildren[UPLEFT], pTraverseFunc, pSphere);                            
         FindObjectsInSphere(pNode->m_ppChildren[UPRIGHT], pTraverseFunc, pSphere);              
      }
   }
   // if it's in one of the lower nodes
   else if (pSphere->Center.z + pSphere->Radius < pNode->m_Center.y)
   {
      // if it's in the lower left
      if (pSphere->Center.x + pSphere->Radius < pNode->m_Center.x)
      {
         FindObjectsInSphere(pNode->m_ppChildren[LOWLEFT], pTraverseFunc, pSphere);              
      }
      // if it's in the lower right
      else if (pSphere->Center.x - pSphere->Radius > pNode->m_Center.x)
      {
         FindObjectsInSphere(pNode->m_ppChildren[LOWRIGHT], pTraverseFunc, pSphere);              
      }
      // if it's in the lower right and lower left
      else
      {
         FindObjectsInSphere(pNode->m_ppChildren[LOWLEFT], pTraverseFunc, pSphere);
         FindObjectsInSphere(pNode->m_ppChildren[LOWRIGHT], pTraverseFunc, pSphere);              
      }
   }
   // if its in the upper and lower nodes
   else
   {
      // if it's in the upper and lower left
      if (pSphere->Center.x + pSphere->Radius < pNode->m_Center.x)
      {
         FindObjectsInSphere(pNode->m_ppChildren[UPLEFT], pTraverseFunc, pSphere);
         FindObjectsInSphere(pNode->m_ppChildren[LOWLEFT], pTraverseFunc, pSphere);
      }
      // if it's in the lower and upper right
      else if (pSphere->Center.x - pSphere->Radius > pNode->m_Center.x)
      {
         FindObjectsInSphere(pNode->m_ppChildren[LOWRIGHT], pTraverseFunc, pSphere);
         FindObjectsInSphere(pNode->m_ppChildren[UPRIGHT], pTraverseFunc, pSphere);
      }
      // if it's in all four nodes
      else
      {
         FindObjectsInSphere(pNode->m_ppChildren[LOWLEFT], pTraverseFunc, pSphere);
         FindObjectsInSphere(pNode->m_ppChildren[LOWRIGHT], pTraverseFunc, pSphere);
         FindObjectsInSphere(pNode->m_ppChildren[UPLEFT], pTraverseFunc, pSphere);
         FindObjectsInSphere(pNode->m_ppChildren[UPRIGHT], pTraverseFunc, pSphere);
      }   
   }
}

// constructor to zero out members
CQuadtree::CNode::CNode()
{
   memset(m_ppChildren, 0, sizeof(CNode *) * NUMCHILDREN); 
}

// create a node's children
void CQuadtree::CNode::CreateChildren()
{
   // set up upper left node
   m_ppChildren[UPLEFT] = new CNode;
   
   m_ppChildren[UPLEFT]->m_pParent = this;
   m_ppChildren[UPLEFT]->m_Min.x = m_Min.x;
   m_ppChildren[UPLEFT]->m_Min.y = m_Center.y;
   m_ppChildren[UPLEFT]->m_Max.x = m_Center.x;
   m_ppChildren[UPLEFT]->m_Max.y = m_Max.y;

   m_ppChildren[UPLEFT]->m_Center.x = (m_ppChildren[UPLEFT]->m_Min.x +
                                      m_ppChildren[UPLEFT]->m_Max.x) * .5f;
   m_ppChildren[UPLEFT]->m_Center.y = (m_ppChildren[UPLEFT]->m_Min.y +
                                      m_ppChildren[UPLEFT]->m_Max.y) * .5f;

   // set up upper right CNode
   m_ppChildren[UPRIGHT] = new CNode;
   
   
   m_ppChildren[UPRIGHT]->m_pParent = this;
   m_ppChildren[UPRIGHT]->m_Min.x = m_Center.x;
   m_ppChildren[UPRIGHT]->m_Min.y = m_Center.y;
   m_ppChildren[UPRIGHT]->m_Max.x = m_Max.x;
   m_ppChildren[UPRIGHT]->m_Max.y = m_Max.y;

   m_ppChildren[UPRIGHT]->m_Center.x = (m_ppChildren[UPRIGHT]->m_Min.x +
                                      m_ppChildren[UPRIGHT]->m_Max.x) * .5f;
   m_ppChildren[UPRIGHT]->m_Center.y = (m_ppChildren[UPRIGHT]->m_Min.y +
                                      m_ppChildren[UPRIGHT]->m_Max.y) * .5f;

   // set up lower left CNode
   m_ppChildren[LOWLEFT] = new CNode;
   
   m_ppChildren[LOWLEFT]->m_pParent = this;
   m_ppChildren[LOWLEFT]->m_Min.x = m_Min.x;
   m_ppChildren[LOWLEFT]->m_Min.y = m_Min.y;
   m_ppChildren[LOWLEFT]->m_Max.x = m_Center.x;
   m_ppChildren[LOWLEFT]->m_Max.y = m_Center.y;

   m_ppChildren[LOWLEFT]->m_Center.x = (m_ppChildren[LOWLEFT]->m_Min.x +
                                      m_ppChildren[LOWLEFT]->m_Max.x) * .5f;
   m_ppChildren[LOWLEFT]->m_Center.y = (m_ppChildren[LOWLEFT]->m_Min.y +
                                      m_ppChildren[LOWLEFT]->m_Max.y) * .5f;

   // set up lower right CNode
   m_ppChildren[LOWRIGHT] = new CNode;
   
   m_ppChildren[LOWRIGHT]->m_pParent = this;
   m_ppChildren[LOWRIGHT]->m_Min.x = m_Center.x;
   m_ppChildren[LOWRIGHT]->m_Min.y = m_Min.y;
   m_ppChildren[LOWRIGHT]->m_Max.x = m_Max.x;
   m_ppChildren[LOWRIGHT]->m_Max.y = m_Center.y;

   m_ppChildren[LOWRIGHT]->m_Center.x = (m_ppChildren[LOWRIGHT]->m_Min.x +
                                      m_ppChildren[LOWRIGHT]->m_Max.x) * .5f;
   m_ppChildren[LOWRIGHT]->m_Center.y = (m_ppChildren[LOWRIGHT]->m_Min.y +
                                      m_ppChildren[LOWRIGHT]->m_Max.y) * .5f;
}

// remove an object from a node
void CQuadtree::CNode::RemoveObject(CObject* pObject)
{
   for (unsigned int i = 0; i < m_ObjectList.size(); ++i)
   {
      if (m_ObjectList[i] == pObject)
      {
         m_ObjectList.erase(&m_ObjectList[i]);
         return;
      }
   }
}

// create a list of visible objects
void CQuadtree::CreateVisibleList()
{
   FindVisible(m_pRoot);   
}

// recursive find visible function
void CQuadtree::FindVisible(CNode* pNode)
{
   // if we're at a leaf then render the terrain and add others to the visibility list
   if (!pNode->m_ppChildren[0])
   {
      // go through all objects in this node
      unsigned int Size = (unsigned int)pNode->m_ObjectList.size();
      for (unsigned int i = 0; i < Size; ++i)
      {
         CObject* pObj = pNode->m_ObjectList[i];
         
         // if it's hasn't been flagged as visible yet
         if (!pNode->m_ObjectList[i]->IsVisible())
         {
            
            pNode->m_ObjectList[i]->SetVisible(true);
            
            // if it's terrain, render it, otherwise add it to the render list 
            if (pNode->m_ObjectList[i]->IsTerrain())
               pNode->m_ObjectList[i]->Render();
            else
            {
               m_pVisibleList->AddRenderObj(pNode->m_ObjectList[i]);
            }
         }
      }      
   }
   // otherwise recurse
   else
   {
      for (unsigned int i = 0; i < NUMCHILDREN; ++i)
      {
         int Visibility = GetVisibility(pNode->m_ppChildren[i]);
         
         // if we're interesctioin with the frustum, find visible
         if (Visibility == FRUSTUM_INTERSECT)
            FindVisible(pNode->m_ppChildren[i]);
         // if we're completely in the frustum, render all objects
         else if (Visibility == FRUSTUM_IN)
            RenderAll(pNode->m_ppChildren[i]);    
      }
   }      
}

// move an object in the quaedtree
void CQuadtree::MoveObject(CObject* pObject, CNode* pCurrentNode)
{
   pCurrentNode->RemoveObject(pObject);

   PlaceObject(pObject, m_pRoot);
}

// recursive render all objects
void CQuadtree::RenderAll(CNode* pNode)
{
   // if we're at a leaf then render the terrain and add others to the visibility list
   if (!pNode->m_ppChildren[0])
   {
      for (unsigned int i = 0; i < pNode->m_ObjectList.size(); ++i)
      {
         if (!pNode->m_ObjectList[i]->IsVisible())
         {
            pNode->m_ObjectList[i]->SetVisible(true);
            
            // if it's terrain, render it, otherwise add it to the render list
            if (pNode->m_ObjectList[i]->IsTerrain())
               pNode->m_ObjectList[i]->Render();
            else
            {
               m_pVisibleList->AddRenderObj(pNode->m_ObjectList[i]);
            }
         }
      }      
   }
   // otherwise recurse
   else
   {
      for (unsigned int i = 0; i < NUMCHILDREN; ++i)
      {
         RenderAll(pNode->m_ppChildren[i]);    
      }
   }
}

// find out if a node is completely in, partly in, or outside of the frustum
int CQuadtree::GetVisibility(CNode* pNode)
{
   CVector3f Max(pNode->m_Max.x, m_MaxY, pNode->m_Max.y);
   CVector3f Min(pNode->m_Min.x, m_MinY, pNode->m_Min.y);
   return m_pFrustum->TestAABB(&Max, &Min);
}

// calculate the relative distance of an object to the camera
void CQuadtree::CObject::CalcDist()
{
   CVector3f CameraPos;
   g_Camera->GetPosition(&CameraPos);
   Vec3fSubtract(&CameraPos, &m_BoundSphere.Center, &CameraPos);
   m_Dist = Vec3fDotProduct(&CameraPos, &CameraPos);
}

// render a render object
void CQuadtreeRenderObj::Render()
{
   // if we're lit, use the materials, otherwise use the diffuse as the color
   if (m_HasLighting)
      m_Material.Activate();
   else
      glColor4f(m_Material.m_Diffuse.r, m_Material.m_Diffuse.g, m_Material.m_Diffuse.b,
                m_Material.m_Diffuse.a);


   glPushMatrix();
      
      // apply the frame transform
      float Matrix[16];
      m_pFrames[0].GetOpenGLMatrix(Matrix);
      glMultMatrixf(Matrix);

      // render the first part
      g_GeometryManager.RenderPart(m_GeometryIndex, 0);
      
      // render all other child parts
      for (unsigned int i = 1; i < m_NumParts; i++)
      {
         m_pFrames[i].GetOpenGLMatrix(Matrix);
         glPushMatrix();
            glMultMatrixf(Matrix);
            g_GeometryManager.RenderPart(m_GeometryIndex, i);
         glPopMatrix();
      }

   glPopMatrix();

   // reset visibility flag
   m_Visible = false;
}

// move an object forward
void CQuadtreeRenderObj::MoveForward(float Dist, unsigned int PartIndex)
{
   m_pFrames[PartIndex].MoveForward(Dist);
   
   if (PartIndex == 0)
   {
      m_pFrames[PartIndex].GetPosition(&m_BoundSphere.Center);
   
      m_Moved = true;
   }   
}

// move an object right
void CQuadtreeRenderObj::MoveRight(float Dist, unsigned int PartIndex)
{
   m_pFrames[PartIndex].MoveRight(Dist);
   
   if (PartIndex == 0)
   {
      m_pFrames[PartIndex].GetPosition(&m_BoundSphere.Center);
   
      m_Moved = true;
   }   
}

// move an object up
void CQuadtreeRenderObj::MoveUp(float Dist, unsigned int PartIndex)
{
   m_pFrames[PartIndex].MoveUp(Dist);

   if (PartIndex == 0)
   {
      m_pFrames[PartIndex].GetPosition(&m_BoundSphere.Center);
   
      m_Moved = true;
   }
}

// rotate an object around its local x axis
void CQuadtreeRenderObj::RotateLocalX(float Angle, unsigned int PartIndex)
{
   m_pFrames[PartIndex].RotateLocalX(Angle);
}

// rotate around its local y axis
void CQuadtreeRenderObj::RotateLocalY(float Angle, unsigned int PartIndex)
{
   m_pFrames[PartIndex].RotateLocalY(Angle);
}

// rotate around its local z axis
void CQuadtreeRenderObj::RotateLocalZ(float Angle, unsigned int PartIndex)
{
   m_pFrames[PartIndex].RotateLocalZ(Angle);
}

// add a quadtree node to this object's list of nodes it is in
void CQuadtreeRenderObj::AddQuadtreeNode(void* pQuadtreeNode)
{
   m_QuadtreeNodes.push_back((CQuadtree::CNode*)pQuadtreeNode);
}

// clear our node list
void CQuadtreeRenderObj::ClearNodes()
{
   m_QuadtreeNodes.clear();
}

// move this object in the quadtree
void CQuadtreeRenderObj::MoveInQuadtree()
{
   RemoveFromQuadtree();

   g_Quadtree.AddObject(this);
}

// set the orientation of this object
void CQuadtreeRenderObj::SetOrientation(const CQuaternion* pQuaternion,
                                        unsigned int PartIndex)
{
   m_pFrames[PartIndex].SetOrientation(pQuaternion);
}

// initialize the object
void CQuadtreeRenderObj::Initialize(const Sphere* pSphere, unsigned int GeometryIndex,
                                    unsigned int NumParts, CVector3f* pTranslations, 
                                    int TextureIndex)
{
   CObject::Initialize(pSphere);

   m_GeometryIndex = GeometryIndex;
   m_TextureIndex = TextureIndex;

   m_NumParts = NumParts;
   m_pFrames = new CObjFrame[NumParts];

   // set up the frames based on the translations
   for (unsigned int i = 0; i < NumParts; ++i)
   {
      m_pFrames[i].TranslateLocal(&(pTranslations[i]));
   }

   m_HasLighting = true;

   m_pOwner = 0;

   m_Moved = false;
}

// free memory
CQuadtreeRenderObj::~CQuadtreeRenderObj()
{
   Unload();
}

void CQuadtreeRenderObj::Unload()
{
    delete [] m_pFrames;
    m_pFrames = 0;
}

// get the position
void CQuadtreeRenderObj::GetPosition(CVector3f* pPosition, unsigned int PartIndex)
{
   m_pFrames[PartIndex].GetPosition(pPosition);
}

// copy another object to initialize it
void CQuadtreeRenderObj::Initialize(const CQuadtreeRenderObj& Obj)
{
   m_BoundSphere = Obj.m_BoundSphere;
   m_GeometryIndex = Obj.m_GeometryIndex;
   m_TextureIndex = Obj.m_TextureIndex;

   m_NumParts = Obj.m_NumParts;

   m_pFrames = new CObjFrame[m_NumParts];

   for (unsigned int i = 0; i < m_NumParts; ++i)
   {
      m_pFrames[i] = Obj.m_pFrames[i];
   }

   m_Visible = 0;

   m_HasLighting = Obj.m_HasLighting;

   m_Material = Obj.m_Material;

   m_pOwner = 0;

   m_Moved = false;
}

// remove this object from the quadtree
void CQuadtreeRenderObj::RemoveFromQuadtree()
{
   for (unsigned int i = 0; i < m_QuadtreeNodes.size(); ++i)
   {
      m_QuadtreeNodes[i]->RemoveObject(this);
   }

   m_QuadtreeNodes.clear();

   m_Moved = false;
}

// set the position
void CQuadtreeRenderObj::SetPosition(const CVector3f* pPosition, unsigned int PartIndex)
{
   m_pFrames[PartIndex].SetPosition(pPosition);

   if (PartIndex == 0)
   {
      m_BoundSphere.Center = *pPosition;
      m_Moved = true;
   }
}

// set the frame
void CQuadtreeRenderObj::SetFrame(const CObjFrame* pFrame, unsigned int PartIndex)
{
   m_pFrames[PartIndex] = *pFrame;

   if (PartIndex == 0)
   {
      pFrame->GetPosition(&m_BoundSphere.Center);   
      m_Moved = true;
   }
}

// update it
void CQuadtreeRenderObj::Update()
{
   if (m_Moved)
   {      
      MoveInQuadtree();
   }
}