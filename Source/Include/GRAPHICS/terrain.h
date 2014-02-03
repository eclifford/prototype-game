// File: "terrain.h"
// Author(s): Jesse Edmunds

#pragma once

#include "graphics\vertext2fc4fn3fv3f.h"
#include "graphics\indexarray.h"
#include "graphics\quadtree.h"
#include "graphics\interleavedarray.h"
#include "graphics\color3f.h"
#include "graphics\glinclude.h"
#include "graphics\ptgfile.h"
#include "math3d\geomath.h"

 // A patch of terrain
class CPatch : public CQuadtree::CObject
{
    public:
    // set up a patch
    inline void Initialize(const Sphere* pSphere, unsigned int* pIndices,
                    unsigned int NumIndices, unsigned int PrimitiveType,
                    unsigned int Index)
    {
        CObject::Initialize(pSphere);

        m_IndexArray.Initialize(pIndices, NumIndices, PrimitiveType);

        m_Index = Index;
    }
    // render a patch
    void Render();
    // overriden function for quadtree object class
    inline bool IsTerrain() { return true; }
    // check for ray collision
    bool RayCollisionPoint(const Ray* pRay, Point3f* pPoint);
    // get the axis-aligned bounding box for this patch
    void GetAABB(AABB* pAABB);            
    
    // the index array for rendering this patch
    CIndexArray m_IndexArray;

    // the index of this patch in the array (used to link with mini-map indices)
    unsigned int m_Index;
};

// Handles terrain loading, rendering and interaction
class CTerrain
{  
   public:
     
      
      // set up defaults for member variables      
      CTerrain() : m_pNormals(0), m_pPatches(0), m_pMiniMapIndices(0) { }
      
      // destructor to free memory
      ~CTerrain();

      // reset the visibility flags on the terrain patches
      void ResetPatchVisibility();
      
      // initialize the terrain from a terrain file
      void Initialize(FILE* pTerrainFile);

      // initialize the terrain from height values
      void Initialize(const float* pHeightValues, unsigned int Size,
                      const char* pTextureFilePath, unsigned int PatchSize,
                      float TextureTileWidth, float TextureTileHeight, float XScale,
                      float YScale, float ZScale);

      // save the terrain to a terrain file
      bool SaveToFile(FILE* pTerrainFile);

      // free all memory
      void Unload();

      // set up states to begin rendering the terrain patches
      void BeginRendering();

      // enable the terrain vertex array
      void EnableVerts();

      // reset states used for terrain rendering
      void ResetStates();

      // render all the terrain patches at once
      void Render();

      // add all terrain patches to the quadtree
      void AddPatchesToQuadtree(CQuadtree* pQuadtree);

      // miscellanneous accessor functions
      float GetHalfPatchYSize() { return m_HalfPatchYSize; }
      float GetHalfPatchZSize() { return m_HalfPatchZSize; }
      float GetHalfPatchXSize() { return m_HalfPatchXSize; }
      float GetSmallestY() { return m_SmallestY; }
      float GetLargestY() { return m_LargestY; }
      const CVertexT2FC4FN3FV3F* GetVertices() { return m_pVertices; }
      unsigned int GetNumPatches() const { return m_NumPatches; } 
      CPatch* GetPatches() const { return m_pPatches; }
      const CVector3f* GetNormals() const { return m_pNormals; }
      unsigned int GetVertsPerSide() const { return m_VertsPerSide; }

      // get the bounding box of the whole terrain
      inline void GetAABB(AABB* pAABB)
      {
         pAABB->Max.x = m_HalfXSize;
         pAABB->Max.y = m_LargestY;
         pAABB->Max.z = m_HalfZSize;

         pAABB->Min.x = -m_HalfXSize;
         pAABB->Min.y = m_SmallestY;
         pAABB->Min.z = -m_HalfZSize;
      }
      
      // draw the normals of the terrain triangle faces
      void DrawNormals(float Scale);

      // change the color of a circle of verts in the terrain
      inline void ColorCircle(const CVector2f* pCenter, float Radius,
                              const Color3F* pColor)
      {
         // keep our min and max values within the terrain
         int MinX = int((pCenter->x - Radius + m_HalfXSize) / m_VertDistX);
         if (MinX < 0)
            MinX = 0;
         else if (MinX >= m_VertsPerSide)
            return;
   
         int MinZ = int((pCenter->y - Radius +  m_HalfZSize) / m_VertDistZ);
         if (MinZ < 0)
            MinZ = 0;
         else if (MinZ >= m_VertsPerSide)
            return;

         int MaxX = int((pCenter->x + Radius + m_HalfXSize) / m_VertDistX);
         if (MaxX < 0)
            return;
         else if (MaxX > m_VertsPerSide)
            MaxX = m_VertsPerSide;
   
         int MaxZ = int((pCenter->y + Radius + m_HalfZSize) / m_VertDistZ);
         if (MaxZ < 0)
            return;
         else if (MaxZ > m_VertsPerSide)
         MaxZ = m_VertsPerSide;

                  
         int z;
         Radius *= Radius;
         int VertIdx;
         float dx;
         float dz;
         float distsq;
         
         // loop through the vertices that are colliding with the box
         for (int x = MinX; x < MaxX; ++x)
         {
            for (z = MinZ; z < MaxZ; ++z)
            {
               // compute the vert index
               VertIdx = x + z * m_VertsPerSide;

               // compute the distance squared
               dx = pCenter->x - m_pVertices[VertIdx].m_Position.x;
               dz = pCenter->y - m_pVertices[VertIdx].m_Position.z;
               distsq = dx * dx + dz * dz;

               // if it's in the circle, color it
               if (distsq <= Radius)
               {
                  m_pVertices[VertIdx].m_Color.r = pColor->r;

                  m_pVertices[VertIdx].m_Color.g = pColor->g;

                  m_pVertices[VertIdx].m_Color.b = pColor->b;     
               }
            }             
         }
      }     
      
      // deform a circle in the terrain
      void DeformCircle(const CVector2f* pCenter, float Radius, float PeakHeight);
      
      // recompute the vertex normals of the whole terrain
      void RecomputeVertexNormals();

      // flatten a square area of the terrain based on the height of the center of
      // the square
      void FlattenSquare(const CVector2f* pMin, const CVector2f* pMax);
      
      // get the plane of the triangle below the point x, z
      inline bool GetPlaneAtPoint(float X, float Z, Plane* pPlane) const
      {         
         // get index of lower left hand vertex of quad we are colliding with
         float QuadX = (float)int((X + m_HalfXSize) / m_VertDistX);
         float QuadZ = (float)int((Z + m_HalfZSize) / m_VertDistZ);

         // compute the normal index
         float NormIndex = ((m_VertsPerSide - 1) * QuadZ + QuadX) * 2;

         // make sure we are in the bounds
         if (NormIndex < 0 || NormIndex >= (m_VertsPerSide - 1) * (m_VertsPerSide - 1) * 2 - 1)
            return false;   

         // compute the vert index
         unsigned int VertIdx = unsigned int(QuadX + QuadZ * m_VertsPerSide);

         // make sure we are in the bounds
         if (VertIdx >= m_NumVertices)
            return false;

         // the plane's point is one of the verts of the quad they are hitting
         pPlane->Point = m_pVertices[VertIdx].m_Position;        

         // figure out which tri we are colliding with
         X = (X + m_HalfXSize - (QuadX * m_VertDistX)) / m_VertDistX;
         Z = (Z + m_HalfZSize - (QuadZ * m_VertDistZ)) / m_VertDistZ;

         // give them the right normal based on which triangle in the quad they are
         // colliding with
         if (X + (1 - Z) > 1.0f)
         {
            pPlane->Normal = m_pNormals[unsigned int(NormIndex)];
         }
         else
         {
            pPlane->Normal = m_pNormals[unsigned int(NormIndex) + 1]; 
         }
         
         return true;         
      }
      
      // the minimap index arrays for each minimap patch
      CIndexArray* m_pMiniMapIndices;

      // the terrain patches
      CPatch* m_pPatches;

   private:
      // the texture file path
      char m_pTextureFilePath[PT_FILE_PATH_LEN];      
      
      // the vertices
      CVertexT2FC4FN3FV3F* m_pVertices;
      
      // the normals
      CVector3f* m_pNormals; 
      
      // miscellaneous info about the terrain
      int m_VertsPerSide;
      unsigned int m_NumVertices;
      float m_VertDistX;
      float m_VertDistZ;
      float m_HalfXSize;
      float m_HalfZSize;
      float m_SmallestY;
      float m_LargestY;
      unsigned int m_TextureIndex;
      unsigned int m_NumPatches;
      float m_HalfPatchYSize;
      float m_HalfPatchZSize;
      float m_HalfPatchXSize;

      // lighting info
      Color4f m_AmbientMaterial;
      Color4f m_SpecularMaterial;
      
      // the vert array      
      CInterleavedArray m_InterleavedArray;         
           
      // recompute the face normals of the faces affected by the point at x, z
      void RecomputeFaceNormals(unsigned int XIndex, unsigned int ZIndex);

      // recompute the vertex normal of the point x, z
      void RecomputeVertexNormals(unsigned int XIndex, unsigned int ZIndex);
};