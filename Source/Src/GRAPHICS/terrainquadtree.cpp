/*#include "terrainquadtree.h"
#include <windows.h>
#include <gl/GL.h>

CTerrainQuadtree::CTerrainQuadtree()
{
   ZERO_OBJECT(*this);
}

void CTerrainQuadtree::DrawDebugLines(const float Height)
{
   DrawTree(Height, &m_Root);   
}

CTerrainQuadtree::CNode::~CNode()
{
   if (m_pChildren)
   {
      for (int i = 0; i < NUMCHILDREN; i++)
      {
         delete m_pChildren[i];
      }
   }
}

void CTerrainQuadtree::DrawTree(const float Height, CNode *pNode)
{
   // if we've reached a leaf then draw it
   if (!pNode->m_pChildren[0])
   {
      pNode->Draw(Height);
      return;
   }
   else
   {
      for (int i = 0; i < NUMCHILDREN; i++)
      {
         DrawTree(Height, pNode->m_pChildren[i]);
      }
   }

}

void CTerrainQuadtree::CNode::Draw(const float Height) const
{
   glColor3f(1.0f, 0.0f, 0.0f);
   glBegin(GL_LINE_LOOP);
   
   // bottom left
   glVertex3f(m_Min.x, Height, m_Min.y);

   // bottom right
   glVertex3f(m_Max.x, Height, m_Min.y);

   // top right
   glVertex3f(m_Max.x, Height, m_Max.y);

   // top left
   glVertex3f(m_Min.x, Height, m_Max.y);

   // line loop goes back to bottom left

   glEnd();
   
   if (m_pPatch)
   {
      glColor3f(0.0f, 0.0f, 1.0f);
      // draw the center as a point
      glBegin(GL_POINTS);

      glVertex3f(m_Center.x, Height, m_Center.y);

      glEnd();
   }

   glColor3f(.5f, .5f, .5f);
}

void CTerrainQuadtree::Initialize(CTerrain* pTerrain)
{
   // figure out the max depth we have to go to create this tree with the patches as leaves
   unsigned int MaxDepth = 0;
   unsigned int PatchesPerSide = pTerrain->GetPatchesPerSide();

   while (PatchesPerSide >= 2)
   {
      PatchesPerSide >>= 1;
      MaxDepth++;
   }

   // initialize the root
   m_Root.m_Max.x = pTerrain->GetVertsPerSide() * .5f;
   m_Root.m_Max.y = m_Root.m_Max.x;
   m_Root.m_Min.x = -m_Root.m_Max.x;
   m_Root.m_Min.y = -m_Root.m_Max.x;

   // create the tree
   SubdivideTree(MaxDepth, 0, &m_Root);

   
   // add all the patches to the tree
   const unsigned int NumPatches = pTerrain->GetNumPatches();
   CTerrain::Patch* pPatches = pTerrain->GetPatches();
   for (unsigned int i = 0; i < NumPatches; i++)
   {
      PlacePatch(&pPatches[i], &m_Root);         
   }
}

void CTerrainQuadtree::SubdivideTree(const unsigned int MaxDepth, const unsigned int CurrentDepth,
                                     CNode* pNode)
{
   static unsigned int Depth = 0;

   if (CurrentDepth == MaxDepth)
   {
      return;
   }

   pNode->CreateChildren();

   for (int i = 0; i < NUMCHILDREN; i++)
   {
      SubdivideTree(MaxDepth, CurrentDepth + 1, pNode->m_pChildren[i]);
   }
}

void CTerrainQuadtree::PlacePatch(CTerrain::Patch* pPatch, CNode* pNode)
{
   // if we've reached a leaf then put it here and return
   if (!pNode->m_pChildren[0])
   {
      pNode->m_pPatch = pPatch;
      return;
   }      
         
   // if it's in one of the upper nodes
   if (pPatch->m_Center.y > pNode->m_Center.y)
   {
      // if it's in the upper left
      if (pPatch->m_Center.x < pNode->m_Center.x)
      {
         PlacePatch(pPatch, pNode->m_pChildren[UPLEFT]);              
      }
      // if it's in the upper right
      else if (pPatch->m_Center.x > pNode->m_Center.x)
      {
         PlacePatch(pPatch, pNode->m_pChildren[UPRIGHT]);              
      }
      // if it's in the upper right and upper left
      else
      {
         PlacePatch(pPatch, pNode->m_pChildren[UPLEFT]);                            
         PlacePatch(pPatch, pNode->m_pChildren[UPRIGHT]);              
      }
   }
   // if it's in one of the lower nodes
   else if (pPatch->m_Center.y < pNode->m_Center.y)
   {
      // if it's in the lower left
      if (pPatch->m_Center.x < pNode->m_Center.x)
      {
         PlacePatch(pPatch, pNode->m_pChildren[LOWLEFT]);              
      }
      // if it's in the lower right
      else if (pPatch->m_Center.x > pNode->m_Center.x)
      {
         PlacePatch(pPatch, pNode->m_pChildren[LOWRIGHT]);              
      }
      // if it's in the lower right and lower left
      else
      {
         PlacePatch(pPatch, pNode->m_pChildren[LOWLEFT]);
         PlacePatch(pPatch, pNode->m_pChildren[LOWRIGHT]);              
      }
   }
   // if its in the upper and lower nodes
   else
   {
      // if it's in the upper and lower left
      if (pPatch->m_Center.x < pNode->m_Center.x)
      {
         PlacePatch(pPatch, pNode->m_pChildren[UPLEFT]);
         PlacePatch(pPatch, pNode->m_pChildren[LOWLEFT]);
      }
      // if it's in the lower and upper right
      else if (pPatch->m_Center.x > pNode->m_Center.x)
      {
         PlacePatch(pPatch, pNode->m_pChildren[LOWRIGHT]);
         PlacePatch(pPatch, pNode->m_pChildren[UPRIGHT]);
      }
      // if it's in all four nodes
      else
      {
         PlacePatch(pPatch, pNode->m_pChildren[LOWLEFT]);
         PlacePatch(pPatch, pNode->m_pChildren[LOWRIGHT]);
         PlacePatch(pPatch, pNode->m_pChildren[UPLEFT]);
         PlacePatch(pPatch, pNode->m_pChildren[UPRIGHT]);
      }   
   }   
}

CTerrainQuadtree::CNode::CNode()
{
   ZERO_OBJECT(*this);
}

void CTerrainQuadtree::CNode::CreateChildren()
{
   // set up upper left node
   m_pChildren[UPLEFT] = new CNode;
   
   m_pChildren[UPLEFT]->m_Min.x = m_Min.x;
   m_pChildren[UPLEFT]->m_Min.y = m_Center.y;
   m_pChildren[UPLEFT]->m_Max.x = m_Center.x;
   m_pChildren[UPLEFT]->m_Max.y = m_Max.y;

   m_pChildren[UPLEFT]->m_Center.x = (m_pChildren[UPLEFT]->m_Min.x +
                                      m_pChildren[UPLEFT]->m_Max.x) * .5f;
   m_pChildren[UPLEFT]->m_Center.y = (m_pChildren[UPLEFT]->m_Min.y +
                                      m_pChildren[UPLEFT]->m_Max.y) * .5f;

   // set up upper right CNode
   m_pChildren[UPRIGHT] = new CNode;
   m_pChildren[UPRIGHT]->m_Min.x = m_Center.x;
   m_pChildren[UPRIGHT]->m_Min.y = m_Center.y;
   m_pChildren[UPRIGHT]->m_Max.x = m_Max.x;
   m_pChildren[UPRIGHT]->m_Max.y = m_Max.y;

   m_pChildren[UPRIGHT]->m_Center.x = (m_pChildren[UPRIGHT]->m_Min.x +
                                      m_pChildren[UPRIGHT]->m_Max.x) * .5f;
   m_pChildren[UPRIGHT]->m_Center.y = (m_pChildren[UPRIGHT]->m_Min.y +
                                      m_pChildren[UPRIGHT]->m_Max.y) * .5f;

   // set up lower left CNode
   m_pChildren[LOWLEFT] = new CNode;
   m_pChildren[LOWLEFT]->m_Min.x = m_Min.x;
   m_pChildren[LOWLEFT]->m_Min.y = m_Min.y;
   m_pChildren[LOWLEFT]->m_Max.x = m_Center.x;
   m_pChildren[LOWLEFT]->m_Max.y = m_Center.y;

   m_pChildren[LOWLEFT]->m_Center.x = (m_pChildren[LOWLEFT]->m_Min.x +
                                      m_pChildren[LOWLEFT]->m_Max.x) * .5f;
   m_pChildren[LOWLEFT]->m_Center.y = (m_pChildren[LOWLEFT]->m_Min.y +
                                      m_pChildren[LOWLEFT]->m_Max.y) * .5f;

   // set up lower right CNode
   m_pChildren[LOWRIGHT] = new CNode;
   m_pChildren[LOWRIGHT]->m_Min.x = m_Center.x;
   m_pChildren[LOWRIGHT]->m_Min.y = m_Min.y;
   m_pChildren[LOWRIGHT]->m_Max.x = m_Max.x;
   m_pChildren[LOWRIGHT]->m_Max.y = m_Center.y;

   m_pChildren[LOWRIGHT]->m_Center.x = (m_pChildren[LOWRIGHT]->m_Min.x +
                                      m_pChildren[LOWRIGHT]->m_Max.x) * .5f;
   m_pChildren[LOWRIGHT]->m_Center.y = (m_pChildren[LOWRIGHT]->m_Min.y +
                                      m_pChildren[LOWRIGHT]->m_Max.y) * .5f;
}*/