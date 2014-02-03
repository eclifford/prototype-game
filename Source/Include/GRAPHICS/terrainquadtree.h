/// \file terrainquadtree.h
/// CQuadtree and CQuadtreeNode class declaration and inline implementation
////////////////////////////////////////////////////////////////////////////////
/*
#pragma once

#include "math3d/math3d.h"
#include "terrain.h"

class CTerrainQuadtree
{
   public:
      CTerrainQuadtree();
      void Initialize(CTerrain* pTerrain);
      enum NodeQuadrant { UPLEFT, UPRIGHT, LOWLEFT, LOWRIGHT, NUMCHILDREN};
      void DrawDebugLines(const float Height);

   private:
      class CNode
      {            
         public:
            CNode();
            ~CNode();
            CVector2f m_Min;
            CVector2f m_Max;
            void CreateChildren();
            void Draw(const float Height) const;
            CNode* m_pChildren[NUMCHILDREN];
            CVector2f m_Center;
            CTerrain::Patch* m_pPatch;                                         
      };
      
      void DrawTree(const float Height, CNode *pNode);
      void SubdivideTree(const unsigned int MaxDepth, const unsigned int CurrentDepth,
                         CNode* pNode);
      void PlacePatch(CTerrain::Patch* pPatch, CNode* pNode);      
      CNode m_Root;  
};*/