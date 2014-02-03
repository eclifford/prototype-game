// File: "quadtree.h"
// Author(s): Jesse Edmunds

#pragma once

#include <vector>

#include "math3d\primitives.h"
#include "graphics\material.h"
#include "graphics\renderable.h"

// forward declarations
class CFrustum;
class CRenderList;
class CGameObject;

// forward declaration of render list
class CRenderList;

// quadtree class used for spatial partitioning of renderables
class CQuadtree
{
   public:
      // quadtree object
      class CObject : public CRenderObj
      {
         public:
            // set up default members
            CObject() : m_Visible(0) { }
            // initialize the sphere
            virtual void Initialize(const Sphere* pSphere) { m_BoundSphere = *pSphere; }
            
            // pure virtual functions for derived classes
            virtual void Render() = 0;
            virtual bool IsTerrain() = 0;

            // set and get functions
            bool IsVisible() { return m_Visible; }
            void SetVisible(bool Visible) { m_Visible = Visible; }            
            const Sphere* GetSphere() const { return &m_BoundSphere; }
            
            // add a node to this object's list of nodes (to be written by derived)                                   
            virtual void AddQuadtreeNode(void* pQuadtreeNode) { }
            
            // calculate the distance of this object to the camera
            void CalcDist();
            
            // the game object that controls this render object
            CGameObject* m_pOwner;

         protected:
            // the bounding sphere
            Sphere m_BoundSphere;

            // whether it's been flaged as visible or not
            bool m_Visible;              
      };

      // all four quadtree quadrants for each node
      enum NodeQuadrant { UPLEFT, UPRIGHT, LOWLEFT, LOWRIGHT, NUMCHILDREN};
      
      // a quadtree node
      class CNode
      {            
         public:
            CNode();
            ~CNode();

            // min and max extents
            CVector2f m_Min;
            CVector2f m_Max;

            // create the children for this node
            void CreateChildren();

            // draw this node
            void Draw(float MinY, float MaxY) const;
            
            // children and parent
            CNode* m_ppChildren[NUMCHILDREN];
            CNode* m_pParent;

            // center of the node
            CVector2f m_Center;

            // the list of objects in this node
            std::vector<CObject*> m_ObjectList;

            // remove an object from this node
            void RemoveObject(CObject* pObject);                                         
      };

      // destructor to free memory
      ~CQuadtree();
      
      // initialize the quadtree
      void Initialize(const CVector2f* pMin, const CVector2f* pMax, float MinY, float MaxY,
                      unsigned int MaxDepth, CFrustum* pFrustum, CRenderList* pVisibleList);
      
      // use a function applied to all objects within a sphere in the quadtree
      void TraverseObjectsInSphere(void (*pTraverseFunc)(CObject* pObject),
                                   const Sphere* pSphere);
      
      // draw the quadtree boxes
      void DrawDebugLines();

      // add an object
      void AddObject(CObject* pObject);
      
      // create the visible list of objects
      void CreateVisibleList();

      // move an object in the tree
      void MoveObject(CObject* pObject, CNode* pCurrentNode);
      
      // free all memory
      void Unload();
      
      // get min and max extents
      float GetMaxY() { return m_MaxY; }
      float GetMinY() { return m_MinY; }

   private:
      // min and max extents      
      float m_MaxY;
      float m_MinY;

      // the renderlist and frustum
      CRenderList* m_pVisibleList;
      CFrustum* m_pFrustum;

      // recursive draw function
      void DrawTree(CNode *pNode);

      // recursive subdicide function
      void SubdivideTree(const unsigned int MaxDepth, const unsigned int CurrentDepth,
                         CNode* pNode);

      // recursive find visible function
      void FindVisible(CNode* pNode);
      
      // recursive find objects in sphere function
      void FindObjectsInSphere(CNode* pNode, void (*pTraverseFunc)(CObject*),
                               const Sphere* pSphere);

      // find out if a node is completely in, partly in, or outside of the frustum
      int GetVisibility(CNode* pNode);
      
      // recursive render all function
      void RenderAll(CNode* pNode);

      // recursive place object function
      void PlaceObject(CObject* pObject, CNode* pNode);
      
      // the root of the tree      
      CNode* m_pRoot;  
};

// used for game models in the quadtree
class CQuadtreeRenderObj : public CQuadtree::CObject
{
   public:
      // destructor to free memory
      ~CQuadtreeRenderObj();
      void Unload();

      // set up an object
      void Initialize(const Sphere* pSphere, unsigned int GeometryIndex, unsigned int NumParts,
                      CVector3f* pTranslations, int TextureIndex);
      void Initialize(const CQuadtreeRenderObj& Obj);
      
      void Render();
      
      // check to see if it's transparent
      bool IsTransparent();

      
      // move this object's frames
      void MoveForward(float Dist, unsigned int PartIndex);
	   void MoveRight(float Dist, unsigned int PartIndex);
	   void MoveUp(float Dist, unsigned int PartIndex);
      void RotateLocalX(float Angle, unsigned int PartIndex);
      void RotateLocalY(float Angle, unsigned int PartIndex);
      void RotateLocalZ(float Angle, unsigned int PartIndex);      

      // set and get functions
      void SetPosition(const CVector3f* pPosition, unsigned int PartIndex);
      void SetOrientation(const CQuaternion* pQuaternion, unsigned int PartIndex);
      bool IsTerrain() { return false; }
      unsigned int GetGeometryIndex() { return m_GeometryIndex; }
      int GetTextureIndex() { return m_TextureIndex; }
      bool HasLighting() { return m_HasLighting; }
      void SetLighting(bool HasLighting) { m_HasLighting = HasLighting; }
      void SetFrame(const CObjFrame* pFrame, unsigned int PartIndex);
      void GetPosition(CVector3f* pPosition, unsigned int PartIndex);      
      inline CObjFrame* GetFrame(unsigned int PartIndex)
      {
         return &m_pFrames[PartIndex];
      }

      // remove this object from the tree
      void RemoveFromQuadtree();
      
      // add a node to this object's list of quadtree nodes it is in
      void AddQuadtreeNode(void* pQuadtreeNode);

      // clear all quadtree nodes in this object
      void ClearNodes();
      CQuadtreeRenderObj& operator=(CQuadtreeRenderObj& Obj);      
      
      // update this object
      void Update();

      // the material used for this object
      CMaterial m_Material;

   protected:

      // move this object in the quadtree
      void MoveInQuadtree();

      // whether we moved this frame
      bool m_Moved;
      
      // the frames in this object      
      CObjFrame *m_pFrames;

      // the number of parts in this object
      unsigned int m_NumParts;

      // the quadtree nodes we are currently in 
      std::vector<CQuadtree::CNode*> m_QuadtreeNodes;

      // our geometry index, texture index and lighting flag
      unsigned int m_GeometryIndex;
      int m_TextureIndex;
      bool m_HasLighting;      
};
