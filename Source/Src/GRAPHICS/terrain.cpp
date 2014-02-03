// File: "terrain.cpp"
// Author(s): Jesse Edmunds

#include <math.h>

#include "graphics/terrain.h"
#include "graphics/texturemanager.h"
#include "graphics/pttfile.h"
#include <gl/glu.h>
#include "core/oglstatemanager.h"
#include "math3d\intersection.h"
#include "graphics\vertext2fc4ubv3f.h"
#include "graphics\drawshape.h" 

#include "utils\mmgr.h"

// external globals
extern CTextureManager g_TextureManager;
extern COpenGLStateManager* pGLSTATE;
extern CTerrain* g_pCurrLevel;
extern CQuadtree g_Quadtree;

// draw the normals of the terrain triangle faces
void CTerrain::DrawNormals(float Scale)
{
   // loop through all the faces
   int CurrNormal = 0;
   for (int z = 0; z < m_VertsPerSide - 1; z++)
   {
      for (int x = 0; x < m_VertsPerSide - 1; x++)
      {
         // find the current vert index
         unsigned int CurrentVert = unsigned int(x + z * m_VertsPerSide);

         // set up the triangle for this face
         Triangle Tri;
         Tri.Vertices[0] = m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position;
         Tri.Vertices[1] = m_pVertices[CurrentVert + 1].m_Position;
         Tri.Vertices[2] = m_pVertices[CurrentVert].m_Position;                              
         Tri.Normal = m_pNormals[CurrNormal];
         
         // draw the triangle
         DrawTriangle(&Tri, Scale);

         // go to the next face
         CurrNormal++;

         // set up the triangle for this face
         Tri.Vertices[0] = m_pVertices[CurrentVert + m_VertsPerSide].m_Position;
         Tri.Vertices[1] = m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position;
         Tri.Vertices[2] = m_pVertices[CurrentVert].m_Position;
         Tri.Normal = m_pNormals[CurrNormal];
         
         // draw the triangle
         DrawTriangle(&Tri, Scale);

         // go to the next face
         CurrNormal++;
      }
   }
}

// deform a circle of terrain
void CTerrain::DeformCircle(const CVector2f* pCenter, float Radius, float PeakHeight)
{
   // make sure we don't go outside of the terrain bounds          
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
   float RadiusSq = Radius * Radius;
   int VertIdx;
   float dx;
   float dz;
   float distsq;
   PeakHeight /= Radius;
   
   // loop through the vertices that are colliding with the box   
   for (int x = MinX; x < MaxX; ++x)
   {
      for (z = MinZ; z < MaxZ; ++z)
      {
         VertIdx = x + z * m_VertsPerSide;
         
         dx = pCenter->x - m_pVertices[VertIdx].m_Position.x;
         dz = pCenter->y - m_pVertices[VertIdx].m_Position.z;
         distsq = dx * dx + dz * dz;
         
         // if we're in the circle
         if (distsq <= RadiusSq)
         {
            m_pVertices[VertIdx].m_Position.y += PeakHeight * sqrtf(RadiusSq - (
            powf(m_pVertices[VertIdx].m_Position.x - pCenter->x, 2) +
            powf(m_pVertices[VertIdx].m_Position.z - pCenter->y, 2)));

            // make sure we don't go over our quadtree limits
            if (m_pVertices[VertIdx].m_Position.y > g_Quadtree.GetMaxY())
               m_pVertices[VertIdx].m_Position.y = g_Quadtree.GetMaxY();
            else if (m_pVertices[VertIdx].m_Position.y < g_Quadtree.GetMinY())
               m_pVertices[VertIdx].m_Position.y = g_Quadtree.GetMinY();
               
            // recompute the normals
            RecomputeFaceNormals(x, z);    
         }
      }             
   }

   // expand our box by one all around to compute affected vertex normals
   if (MinX > 0)
      MinX--;
   
   if (MinZ > 0)
      MinZ--;

   if (MaxX < m_VertsPerSide)
      MaxX++;
   
   if (MaxZ < m_VertsPerSide)
      MaxZ++;
      
   // recompute affected vertex normals
   for (x = MinX; x < MaxX; ++x)
   {   
      for (z = MinZ; z < MaxZ; ++z)
      {         
         RecomputeVertexNormals(x, z);
      }
   }
}

// recompute the face normals affected by the vert at x, z
void CTerrain::RecomputeFaceNormals(unsigned int XIndex, unsigned int ZIndex)
{
   // go through all the triangles that this vertex is adjacent to
   // and recompute the normals
   unsigned int VertIndex = XIndex + ZIndex * m_VertsPerSide;

   unsigned int NormIndex = ((m_VertsPerSide - 1) * ZIndex + XIndex) * 2;

   unsigned int NumQuads = m_VertsPerSide - 1;
   unsigned int CurrentVert;
   unsigned int CurrentNorm;
   
   // if this vertex isn't at the very top of the map
   if (ZIndex < NumQuads)
   {
      // if this vertex isn't at the very right of the map
      if (XIndex < NumQuads)
      {
         // recompute the normals for this quad
      

         // bottom right tri first           
         ComputeNormal(&m_pVertices[VertIndex + m_VertsPerSide + 1].m_Position,
                       &m_pVertices[VertIndex + 1].m_Position,
                       &m_pVertices[VertIndex].m_Position, &m_pNormals[NormIndex]);  
   
         // upper left tri
         ComputeNormal(&m_pVertices[VertIndex + m_VertsPerSide].m_Position,
                       &m_pVertices[VertIndex + m_VertsPerSide + 1].m_Position,
                       &m_pVertices[VertIndex].m_Position, &m_pNormals[NormIndex + 1]);
         
         if (ZIndex > 0)
         {
            CurrentVert = VertIndex - m_VertsPerSide;
            CurrentNorm = NormIndex - ((m_VertsPerSide - 1) * 2); 
                     
            // upper left tri of quad below
            ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide].m_Position,
                          &m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                          &m_pVertices[CurrentVert].m_Position,
                          &m_pNormals[CurrentNorm + 1]);

            if (XIndex > 0)
            {
               CurrentVert = VertIndex - 1;
               CurrentNorm = NormIndex - 2; 
         
               // bottom right tri of quad to the left
               ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                             &m_pVertices[CurrentVert + 1].m_Position,
                             &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);  
   
         
               // both tris in the quad below and to the left
               CurrentVert = VertIndex - m_VertsPerSide - 1;
               CurrentNorm = NormIndex - (m_VertsPerSide - 1) * 2 - 2;

               // bottom right tri first           
               ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                             &m_pVertices[CurrentVert + 1].m_Position,
                             &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);  
   
               // upper left tri
               ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide].m_Position,
                             &m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                             &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm + 1]);
            }
         }
         else if (XIndex > 0)
         {
            // bottom right tri of quad to the left
            CurrentVert = VertIndex - 1;
            CurrentNorm = NormIndex - 2; 

            // bottom right tri of quad to the left
            ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                          &m_pVertices[CurrentVert + 1].m_Position,
                          &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);
         }
      }
      else
      {
         // bottom right tri of quad to the left
         CurrentVert = VertIndex - 1;
         CurrentNorm = NormIndex - 2; 

         // bottom right tri of quad to the left
         ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                       &m_pVertices[CurrentVert + 1].m_Position,
                       &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);
      
         if (ZIndex > 0)
         {
            // both tris in the quad below and to the left
            CurrentVert = VertIndex - m_VertsPerSide - 1;
            CurrentNorm = NormIndex - m_VertsPerSide - 1 - 2;

            // bottom right tri first           
            ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                          &m_pVertices[CurrentVert + 1].m_Position,
                          &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);  

            // upper left tri
            ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide].m_Position,
                          &m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                          &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm + 1]);     
         }
      }
   }
   else
   {
      if (XIndex > 0)
      {
         // both tris in the quad below and to the left
         CurrentVert = VertIndex - m_VertsPerSide - 1;
         CurrentNorm = NormIndex - (m_VertsPerSide - 1) * 2 - 2;

         // bottom right tri first           
         ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                       &m_pVertices[CurrentVert + 1].m_Position,
                       &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);  

         // upper left tri
         ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide].m_Position,
                       &m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                       &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm + 1]);
      
         if (XIndex < NumQuads)
         {
            CurrentVert = VertIndex - m_VertsPerSide;
            CurrentNorm = NormIndex - (m_VertsPerSide - 1) * 2; 
                     
            // upper left tri of quad below
            ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide].m_Position,
                          &m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                          &m_pVertices[CurrentVert].m_Position,
                          &m_pNormals[CurrentNorm + 1]);
         }
      }
      else
      {
         CurrentVert = VertIndex - m_VertsPerSide;
         CurrentNorm = NormIndex - (m_VertsPerSide - 1) * 2; 
                     
         // upper left tri of quad below
         ComputeNormal(&m_pVertices[CurrentVert + m_VertsPerSide].m_Position,
                       &m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                       &m_pVertices[CurrentVert].m_Position,
                       &m_pNormals[CurrentNorm + 1]);
      }
   }   
}

// flatten a square area of the terrain based on the height of the center of
// the square
void CTerrain::FlattenSquare(const CVector2f* pMin, const CVector2f* pMax)
{
   // make sure we don't go over the terrain limits
   int MinX = int((pMin->x + m_HalfXSize) / m_VertDistX);
   if (MinX < 0)
      MinX = 0;
   else if (MinX >= m_VertsPerSide)
      return;
   
   int MinZ = int((pMin->y + m_HalfZSize) / m_VertDistZ);
   if (MinZ < 0)
      MinZ = 0;
   else if (MinZ >= m_VertsPerSide)
      return;

   int MaxX = int((pMax->x + m_HalfXSize) / m_VertDistX);
   if (MaxX < 0)
      return;
   else if (MaxX > m_VertsPerSide)
      MaxX = m_VertsPerSide;
   
   int MaxZ = int((pMax->y + m_HalfZSize) / m_VertDistZ);
   if (MaxZ < 0)
      return;
   else if (MaxZ > m_VertsPerSide)
      MaxZ = m_VertsPerSide;
   
   if (MaxX == MinX || MinZ == MaxZ)
      return;

   // find the height of the vert in the middle
   float Height = m_pVertices[MinX + ((MaxX - MinX) >> 1) + (MinZ + ((MaxZ - MinZ) >> 1))
                              * m_VertsPerSide].m_Position.y;

   // loop through the vertices that are colliding with the box
   int z;
   int VertIdx;
   for (int x = MinX; x < MaxX; ++x)
   {
      for (z = MinZ; z < MaxZ; ++z)
      {
         VertIdx = x + z * m_VertsPerSide;

         m_pVertices[VertIdx].m_Position.y = Height;

         // make sure we don't go over our quadtree limits
         if (m_pVertices[VertIdx].m_Position.y > g_Quadtree.GetMaxY())
            m_pVertices[VertIdx].m_Position.y = g_Quadtree.GetMaxY();
         else if (m_pVertices[VertIdx].m_Position.y < g_Quadtree.GetMinY())
            m_pVertices[VertIdx].m_Position.y = g_Quadtree.GetMinY();
         
         RecomputeFaceNormals(x, z);                 
      } 
   }

   // expand the box by one all around to recompute the vert normals
   if (MinX > 0)
      MinX--;

   if (MinZ > 0)
      MinZ--;

   if (MaxX < m_VertsPerSide)
      MaxX++;

   if (MaxZ < m_VertsPerSide)
      MaxZ++;


   // recompute the vert normals for lighting
   for (x = MinX; x < MaxX; ++x)
   {
      for (z = MinZ; z < MaxZ; ++z)
      {
         RecomputeVertexNormals(x, z);
      } 
   }
}

// recompute the vertex normals of the whole terrain
void CTerrain::RecomputeVertexNormals()
{
   int X = 0;
   int Z = 0;

   for (Z = 0; Z < m_VertsPerSide; ++Z)
   {
      for (X = 0; X < m_VertsPerSide; ++X)
      {
         RecomputeVertexNormals(X, Z);
      }
   }
}

// recompute the vertex normal of the vert at x, z
void CTerrain::RecomputeVertexNormals(unsigned int XIndex, unsigned int ZIndex)
{
   // go through all the triangles that this vertex is adjacent to
   // and recompute the normals
   unsigned int VertIndex = XIndex + ZIndex * m_VertsPerSide;

   unsigned int NormIndex = ((m_VertsPerSide - 1) * ZIndex + XIndex) * 2;

   unsigned int NumQuads = m_VertsPerSide - 1;
   
   // if this vertex isn't at the very top of the map
   if (ZIndex < NumQuads)
   {
      // if this vertex isn't at the very right of the map
      if (XIndex < NumQuads)
      {
         // recompute the normals for this quad    
         if (ZIndex > 0)
         {
            if (XIndex > 0)
            {
               // add all adjacent face normals and scale to get the average normal 
               Vec3fAdd(&m_pVertices[VertIndex].m_Normal, &m_pNormals[NormIndex],
                        &m_pNormals[NormIndex + 1]);
               Vec3fAdd(&m_pVertices[VertIndex].m_Normal,
                        &m_pVertices[VertIndex].m_Normal,
                        &m_pNormals[NormIndex - ((m_VertsPerSide - 1) * 2) + 1]);
               Vec3fAdd(&m_pVertices[VertIndex].m_Normal,
                        &m_pVertices[VertIndex].m_Normal,
                        &m_pNormals[NormIndex - 2]);
               Vec3fAdd(&m_pVertices[VertIndex].m_Normal,
                        &m_pVertices[VertIndex].m_Normal,
                        &m_pNormals[NormIndex - (m_VertsPerSide - 1) * 2 - 2]);
               Vec3fAdd(&m_pVertices[VertIndex].m_Normal,
                        &m_pVertices[VertIndex].m_Normal,
                        &m_pNormals[NormIndex - (m_VertsPerSide - 1) * 2 - 1]);
               Vec3fScale(&m_pVertices[VertIndex].m_Normal,
                          &m_pVertices[VertIndex].m_Normal, .1666667f);
            }
            else
            {
               // add all adjacent face normals and scale to get the average normal
               Vec3fAdd(&m_pVertices[VertIndex].m_Normal, &m_pNormals[NormIndex],
                        &m_pNormals[NormIndex + 1]);
               Vec3fAdd(&m_pVertices[VertIndex].m_Normal,
                        &m_pVertices[VertIndex].m_Normal,
                        &m_pNormals[NormIndex - ((m_VertsPerSide - 1) * 2) + 1]);
               Vec3fScale(&m_pVertices[VertIndex].m_Normal,
                          &m_pVertices[VertIndex].m_Normal, .3333333f);
            }
         
         }
         else if (XIndex > 0)
         {
            // bottom right tri of quad to the left
            
            // add all adjacent face normals and scale to get the average normal  
            Vec3fAdd(&m_pVertices[VertIndex].m_Normal, &m_pNormals[NormIndex],
                     &m_pNormals[NormIndex + 1]);
            Vec3fAdd(&m_pVertices[VertIndex].m_Normal,
                     &m_pVertices[VertIndex].m_Normal,
                     &m_pNormals[NormIndex - 2]);
            Vec3fScale(&m_pVertices[VertIndex].m_Normal,
                       &m_pVertices[VertIndex].m_Normal, .3333333f);
         }
         else
         {
            // add all adjacent face normals and scale to get the average normal
            Vec3fAdd(&m_pVertices[VertIndex].m_Normal, &m_pNormals[NormIndex],
                     &m_pNormals[NormIndex + 1]);
            Vec3fScale(&m_pVertices[VertIndex].m_Normal,
                       &m_pVertices[VertIndex].m_Normal, .5f);
         }
      }
      else
      {
         // bottom right tri of quad to the left 
         if (ZIndex > 0)
         {
            // both tris in the quad below and to the left       
            
            // add all adjacent face normals and scale to get the average normal
            Vec3fAdd(&m_pVertices[VertIndex].m_Normal, &m_pNormals[NormIndex - 2],
                     &m_pNormals[NormIndex - m_VertsPerSide - 3]);
            Vec3fAdd(&m_pVertices[VertIndex].m_Normal,
                     &m_pVertices[VertIndex].m_Normal,
                     &m_pNormals[NormIndex - m_VertsPerSide - 2]);
            Vec3fScale(&m_pVertices[VertIndex].m_Normal,
                       &m_pVertices[VertIndex].m_Normal, .3333333f);         
         }
         else
         {
            m_pVertices[VertIndex].m_Normal = m_pNormals[NormIndex - 2];
         }
      }
   }
   else
   {
      if (XIndex > 0)
      {
         // both tris in the quad below and to the left    
         if (XIndex < NumQuads)
         {                  
            // upper left tri of quad below

            // add all adjacent face normals and scale to get the average normal
            Vec3fAdd(&m_pVertices[VertIndex].m_Normal,
                     &m_pNormals[NormIndex - (m_VertsPerSide - 1) * 2 - 2],
                     &m_pNormals[NormIndex - (m_VertsPerSide - 1) * 2 - 1]);
            Vec3fAdd(&m_pVertices[VertIndex].m_Normal,
                     &m_pVertices[VertIndex].m_Normal,
                     &m_pNormals[NormIndex - (m_VertsPerSide - 1) * 2 + 1]);
            Vec3fScale(&m_pVertices[VertIndex].m_Normal,
                       &m_pVertices[VertIndex].m_Normal, .3333333f);
         }
         else
         {
            // add all adjacent face normals and scale to get the average normal
            Vec3fAdd(&m_pVertices[VertIndex].m_Normal,
                     &m_pNormals[NormIndex - (m_VertsPerSide - 1) * 2 - 2],
                     &m_pNormals[NormIndex - (m_VertsPerSide - 1) * 2 - 1]);
            Vec3fScale(&m_pVertices[VertIndex].m_Normal,
                       &m_pVertices[VertIndex].m_Normal, .5f);
         }
      }
      else
      {                 
         // upper left tri of quad below
         m_pVertices[VertIndex].m_Normal = m_pNormals[NormIndex - (m_VertsPerSide - 1) * 2 + 1];
      }
   }   
}

// free the memory in the terrain
void CTerrain::Unload()
{
   delete [] m_pNormals;
   m_pNormals = 0;
   delete [] m_pPatches;
   m_pPatches = 0;
   delete [] m_pMiniMapIndices;
   m_pMiniMapIndices = 0;

   m_InterleavedArray.Unload();
}

// get the axis aligned bounding box of a patch
void CPatch::GetAABB(AABB* pAABB)
{
   pAABB->Max.y = g_Quadtree.GetMaxY();
   pAABB->Min.y = g_Quadtree.GetMinY();
   
   pAABB->Max.x = g_pCurrLevel->GetHalfPatchXSize() + m_BoundSphere.Center.x;
   pAABB->Min.x = -g_pCurrLevel->GetHalfPatchXSize() + m_BoundSphere.Center.x;
   
   pAABB->Max.z = g_pCurrLevel->GetHalfPatchZSize() + m_BoundSphere.Center.z;
   pAABB->Min.z = -g_pCurrLevel->GetHalfPatchZSize() + m_BoundSphere.Center.z;      
}

// render a patch's indices
void CPatch::Render()
{
   m_IndexArray.Render();      
}

// find the collision point of a ray hitting this patch
bool CPatch::RayCollisionPoint(const Ray* pRay, Point3f* pPoint)
{
   Triangle Tri;
   
   const unsigned int* pIndices = m_IndexArray.GetIndices();
   unsigned int NumIndices = m_IndexArray.GetNumIndices();
   const CVertexT2FC4FN3FV3F* pVertices = g_pCurrLevel->GetVertices();
   
   bool FoundPoint = false;
   float BestT;      
   float T;
   Triangle BestTri;

   bool SwitchSides = false;
   unsigned int NumDegenerates = 0;

   // go through all the triangles in this patch 
   for (unsigned int i = 2; i < NumIndices; ++i)
   {
      // check for degenerate triangles
      if (pIndices[i - 2] == pIndices[i - 1] || pIndices[i] == pIndices[i - 2] ||
          pIndices[i - 1] == pIndices[i])
      {
         NumDegenerates++;

         if (NumDegenerates == 4)
         {
            NumDegenerates = 0;
            SwitchSides = !SwitchSides;
         }

         continue;
      }

      // get the tri
      Tri.Vertices[0] = pVertices[pIndices[i - 2]].m_Position;
      Tri.Vertices[1] = pVertices[pIndices[i - 1]].m_Position;
      Tri.Vertices[2] = pVertices[pIndices[i]].m_Position;
      
      // compute the normal
      if (SwitchSides)
         ComputeNormal(&Tri.Vertices[0], &Tri.Vertices[2], &Tri.Vertices[1], &Tri.Normal);
      else
         ComputeNormal(&Tri.Vertices[0], &Tri.Vertices[1], &Tri.Vertices[2], &Tri.Normal);      

      // find the closest collision dist
      T = RayToTriangle(pRay, &Tri);
      if (T != -1 && (!FoundPoint || T < BestT))
      {      
         FoundPoint = true;
         BestT = T;
         BestTri = Tri;
      }
   }
   
   // compute the point
   if (FoundPoint)
   {
      Vec3fScale(pPoint, &pRay->Direction, BestT);
      Vec3fAdd(pPoint, &pRay->Origin, pPoint);

      return true;
   }
   else
      return false;       
}

// reset the visibility of the terrain patches
void CTerrain::ResetPatchVisibility()
{
   for (unsigned int i = 0; i < m_NumPatches; ++i)
      m_pPatches[i].SetVisible(false);   
}

// reset the opengl states used to render the terrain
void CTerrain::ResetStates()
{
   pGLSTATE->SetState(LIGHTING, true);
   pGLSTATE->SetState(COLOR_MATERIAL, false);
}

// render all the terrain patches
void CTerrain::Render()
{
   for (unsigned int i = 0; i < m_NumPatches; i++)
   {
      m_pPatches[i].Render();
   }
}

// free memory
CTerrain::~CTerrain()
{
   delete [] m_pNormals;
   delete [] m_pPatches;
   delete [] m_pMiniMapIndices;
}

// add all terrain patches to the quadtree
void CTerrain::AddPatchesToQuadtree(CQuadtree* pQuadtree)
{
   for (unsigned int i = 0; i < m_NumPatches; i++)
   {
      pQuadtree->AddObject(&m_pPatches[i]);
   }
}

// save the terrain to a file
bool CTerrain::SaveToFile(FILE* pTerrainFile)
{      
   unsigned int Token = PTT_FILE_HEADER_VER2;
   
   // write the file header
   fwrite(&Token, sizeof(Token), 1, pTerrainFile);
   
   // write out the vertices
   fwrite(&m_NumVertices, sizeof(m_NumVertices), 1, pTerrainFile);
   fwrite(m_pVertices, sizeof(*m_pVertices) * m_NumVertices, 1, pTerrainFile);

   // write out the normals
   unsigned int NumNormals = (m_VertsPerSide - 1) * (m_VertsPerSide - 1) * 2;
   fwrite(&NumNormals, sizeof(NumNormals), 1, pTerrainFile);
   fwrite(m_pNormals, sizeof(*m_pNormals) * NumNormals, 1, pTerrainFile); 
   
   // write out map info
   fwrite(&m_VertsPerSide, sizeof(m_VertsPerSide), 1, pTerrainFile);
   fwrite(&m_HalfXSize, sizeof(m_HalfXSize), 1, pTerrainFile);
   fwrite(&m_HalfZSize, sizeof(m_HalfZSize), 1, pTerrainFile);
   fwrite(&m_SmallestY, sizeof(m_SmallestY), 1, pTerrainFile);
   fwrite(&m_LargestY, sizeof(m_LargestY), 1, pTerrainFile);

   // write out patches
   fwrite(&m_NumPatches, sizeof(m_NumPatches), 1, pTerrainFile);
   // patch spheres
   for (Token = 0; Token < m_NumPatches; ++Token)
   {
      fwrite(m_pPatches[Token].GetSphere(), sizeof(Sphere), 1, pTerrainFile);
   }

   // write out number of indices per patch
   Token = m_pPatches[0].m_IndexArray.GetNumIndices();
   fwrite(&Token, sizeof(Token), 1, pTerrainFile);

   // write out patch indices
   for (unsigned int i = 0; i < m_NumPatches; ++i)
   {      
      fwrite(m_pPatches[i].m_IndexArray.GetIndices(), sizeof(unsigned int) * Token, 1, pTerrainFile);
   }

   // write out number of indices per MINIMAP patch
   Token = m_pMiniMapIndices[0].GetNumIndices();
   fwrite(&Token, sizeof(Token), 1, pTerrainFile);

   // write out MINIMAP patch indices
   for (i = 0; i < m_NumPatches; ++i)
   {      
      fwrite(m_pMiniMapIndices[i].GetIndices(), sizeof(unsigned int) * Token, 1, pTerrainFile);
   }

   // write out texture file path
   fwrite(m_pTextureFilePath, PT_FILE_PATH_LEN, 1, pTerrainFile);

   // write out slope lighting info (no longer used)
   CVector2f temp;

   fwrite(&temp, sizeof(temp), 1, pTerrainFile);   
   fwrite(&Token, sizeof(Token), 1, pTerrainFile);
   fwrite(&Token, sizeof(Token), 1, pTerrainFile);
   fwrite(&Token, sizeof(Token), 1, pTerrainFile);
   
   // write out patch info
   fwrite(&m_HalfPatchXSize, sizeof(m_HalfPatchXSize), 1, pTerrainFile);
   fwrite(&m_HalfPatchYSize, sizeof(m_HalfPatchYSize), 1, pTerrainFile);
   fwrite(&m_HalfPatchZSize, sizeof(m_HalfPatchZSize), 1, pTerrainFile);

   return true;
}

// initialize the terrain from height values
void CTerrain::Initialize(const float* pHeightValues, unsigned int Size,
                          const char* pTextureFilePath, unsigned int PatchSize,
                          float TextureTileWidth, float TextureTileHeight, float XScale,
                          float YScale, float ZScale)
{  
   m_VertsPerSide = Size;
   
   m_HalfXSize = m_VertsPerSide * XScale * .5f;
   m_HalfZSize = m_VertsPerSide * ZScale * .5f;

   // figure out the # of patches per side
   unsigned int PatchesPerSide = 1 + (m_VertsPerSide - PatchSize) / (PatchSize - 1);

   // allocate memory for the verts and patches
   m_NumVertices = m_VertsPerSide * m_VertsPerSide;
   m_pVertices = new CVertexT2FC4FN3FV3F[m_NumVertices];
   m_NumPatches = PatchesPerSide * PatchesPerSide;
   Sphere* pSpheres = new Sphere[m_NumPatches];

   unsigned int IndicesPerPatch = (((PatchSize - 1) * (PatchSize - 1)) << 1) + 2 + 
                                  (PatchSize - 2) * 4;
   
   unsigned int** ppIndices = new unsigned int*[m_NumPatches];
   // allocate memory for each of the patchs' indices
   for (unsigned int i = 0; i < m_NumPatches; i++)
   {
      ppIndices[i] = new unsigned int[IndicesPerPatch];
   } 

   // create the vertices for the heightmap (centered around 0, 128, 0)
   float x, z;
   unsigned int CurrentVert = 0;
   bool SwitchSides = false;
   unsigned int CurrentPatch = 0;
   unsigned int CurrentIndex = 0;
   
   m_VertDistX = XScale;
   m_VertDistZ = ZScale;

   m_LargestY = pHeightValues[0] * YScale;
   m_SmallestY = pHeightValues[0] * YScale;
   
   m_HalfPatchXSize = (PatchSize - 1) * m_VertDistX * .5f;
   m_HalfPatchZSize = (PatchSize - 1) * m_VertDistZ * .5f;
   m_HalfPatchYSize = (m_LargestY - m_SmallestY) * .5f;

   for (z = -m_HalfZSize; z < m_HalfZSize; z += m_VertDistZ)
   {
      for (x = -m_HalfXSize; x < m_HalfXSize; x+= m_VertDistX)
      {
         m_pVertices[CurrentVert].m_Position.x = x;
         m_pVertices[CurrentVert].m_Position.y = pHeightValues[CurrentVert] * YScale;
      
         if (m_pVertices[CurrentVert].m_Position.y > m_LargestY)
            m_LargestY = m_pVertices[CurrentVert].m_Position.y;

         if (m_pVertices[CurrentVert].m_Position.y < m_SmallestY)
            m_SmallestY = m_pVertices[CurrentVert].m_Position.y;
      
         m_pVertices[CurrentVert].m_Position.z = z;
      
         m_pVertices[CurrentVert].m_TextureCoord.x = (m_pVertices[CurrentVert].m_Position.x + m_HalfXSize)
                                                     / TextureTileWidth;
         m_pVertices[CurrentVert].m_TextureCoord.y = (m_pVertices[CurrentVert].m_Position.z + m_HalfZSize)
                                                     / TextureTileHeight;
         // set up default vertex colors
         m_pVertices[CurrentVert].m_Color.r = 1.0f;
         m_pVertices[CurrentVert].m_Color.g = 1.0f;
         m_pVertices[CurrentVert].m_Color.b = 1.0f;
         m_pVertices[CurrentVert].m_Color.a = 1.0f;

         ++CurrentVert;
      }
   }

   // generate face normals
   unsigned int NumNormals = (m_VertsPerSide - 1) * (m_VertsPerSide - 1) * 2;
   m_pNormals = new CVector3f[NumNormals];
   unsigned int CurrNormal = 0;
   CVector3f vec1;
   CVector3f vec2;
   for (z = 0; z < m_VertsPerSide - 1; ++z)
   {
      for (x = 0; x < m_VertsPerSide - 1; ++x)
      {
         CurrentVert = unsigned int(x + z * m_VertsPerSide);

         Vec3fSubtract(&vec1, &m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                       &m_pVertices[CurrentVert + 1].m_Position);
         Vec3fSubtract(&vec2, &m_pVertices[CurrentVert].m_Position,
                       &m_pVertices[CurrentVert + 1].m_Position);
         Vec3fCrossProduct(&m_pNormals[CurrNormal], &vec2, &vec1);
         Vec3fNormalize(&m_pNormals[CurrNormal], &m_pNormals[CurrNormal]);
         ++CurrNormal;

         Vec3fSubtract(&vec1, &m_pVertices[CurrentVert + m_VertsPerSide].m_Position,
                       &m_pVertices[CurrentVert].m_Position);
         Vec3fSubtract(&vec2, &m_pVertices[CurrentVert + m_VertsPerSide + 1].m_Position,
                       &m_pVertices[CurrentVert].m_Position);
         Vec3fCrossProduct(&m_pNormals[CurrNormal], &vec1, &vec2);
         Vec3fNormalize(&m_pNormals[CurrNormal], &m_pNormals[CurrNormal]);
         ++CurrNormal;
      }
   }
   
   // compute vertex normals
   RecomputeVertexNormals();

   // create the indices
   float PatchX, PatchZ;
   float MiddleY = 0.0f;
   for (PatchX = 0; PatchX < m_VertsPerSide - 1; PatchX += PatchSize - 1)
   {   
      for (PatchZ = 0; PatchZ < m_VertsPerSide - 1; PatchZ += PatchSize - 1)
      {
         SwitchSides = false;
         CurrentIndex = 0;

         for (x = PatchX; x < PatchX + PatchSize - 1; x++)
         {            

            if (SwitchSides)
            {

               // since we're starting a new row, insert degenerate triangles
               ppIndices[CurrentPatch][CurrentIndex] = unsigned int((PatchSize + PatchZ - 1) *
                                                                    m_VertsPerSide + x);
               CurrentIndex++;
               ppIndices[CurrentPatch][CurrentIndex] = unsigned int((PatchSize + PatchZ - 1) *
                                                                    m_VertsPerSide + x);
               CurrentIndex++;

               for (z = (float)PatchSize + PatchZ - 1; z >= PatchZ; z--)
               {                  
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(z * m_VertsPerSide + x);
                  MiddleY += m_pVertices[unsigned int(z * m_VertsPerSide + x)].m_Position.y;
                  CurrentIndex++;
            
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(z * m_VertsPerSide + x + 1);
                  MiddleY += m_pVertices[unsigned int(z * m_VertsPerSide + x + 1)].m_Position.y;
                  CurrentIndex++;
               }
            }
            else
            {
               // if we're not the first row, then insert degenerate triangles
               if (x != PatchX)
               {
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(PatchZ * m_VertsPerSide 
                                                                       + x);
                  CurrentIndex++;
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(PatchZ * m_VertsPerSide 
                                                                       + x);
                  CurrentIndex++;
               }

               for (z = PatchZ; z < PatchSize + PatchZ; z++)
               {
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(z * m_VertsPerSide + x + 1);
                  MiddleY += m_pVertices[unsigned int(z * m_VertsPerSide + x + 1)].m_Position.y;                  
                  CurrentIndex++;
            
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(z * m_VertsPerSide + x);
                  MiddleY += m_pVertices[unsigned int(z * m_VertsPerSide + x)].m_Position.y;                  
                  CurrentIndex++;
            
               }
            }

            SwitchSides = !SwitchSides;
         }

         pSpheres[CurrentPatch].Center.x = (PatchX / (PatchSize - 1) * (m_HalfPatchXSize * 2)) + m_HalfPatchXSize - m_HalfXSize;
         pSpheres[CurrentPatch].Center.z = (PatchZ / (PatchSize - 1) * (m_HalfPatchZSize * 2)) + m_HalfPatchZSize - m_HalfZSize;
         
         pSpheres[CurrentPatch].Center.y = MiddleY / (float)CurrentIndex; 
         
         // find the vert furthest away from the center to get the radius
         pSpheres[CurrentPatch].Radius = 0.0f;
         for (unsigned int i = 0; i < IndicesPerPatch; i++)
         {
            float Dist = powf(m_pVertices[ppIndices[CurrentPatch][i]].m_Position.x -
                              pSpheres[CurrentPatch].Center.x, 2) +
                         powf(m_pVertices[ppIndices[CurrentPatch][i]].m_Position.y -
                              pSpheres[CurrentPatch].Center.y, 2) +
                         powf(m_pVertices[ppIndices[CurrentPatch][i]].m_Position.z -
                              pSpheres[CurrentPatch].Center.z, 2);
            
            if (Dist > pSpheres[CurrentPatch].Radius)
               pSpheres[CurrentPatch].Radius = Dist; 
         }
         pSpheres[CurrentPatch].Radius = sqrtf(pSpheres[CurrentPatch].Radius);
         
         CurrentPatch++;
         MiddleY = 0.0f;
         
      }
   }

   
   unsigned int VertSkip = 16;
   CurrentPatch = 0;
   
   unsigned int** ppMiniMapIndices = new unsigned int*[m_NumPatches];
   unsigned int MiniMapPatchSize = (PatchSize - 1) / VertSkip + 1;
   unsigned int MiniMapIndicesPerPatch = (((MiniMapPatchSize - 1) *
                                         (MiniMapPatchSize - 1)) << 1) + 2 + 
                                         (MiniMapPatchSize - 2) * 4;
   // allocate memory for each of the patchs' indices
   for (i = 0; i < m_NumPatches; i++)
   {
      ppMiniMapIndices[i] = new unsigned int[MiniMapIndicesPerPatch];
   }

   // create simplified patch indices
   for (PatchX = 0; PatchX < m_VertsPerSide - 1; PatchX += PatchSize - 1)
   {   
      for (PatchZ = 0; PatchZ < m_VertsPerSide - 1; PatchZ += PatchSize - 1)
      {
         SwitchSides = false;
         CurrentIndex = 0;

         for (x = PatchX; x < PatchX + PatchSize - 1; x += VertSkip)
         {            

            if (SwitchSides)
            {

               // since we're starting a new row, insert degenerate triangles
               ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int((PatchSize + PatchZ - VertSkip) *
                                                                    m_VertsPerSide + x);
               CurrentIndex++;
               ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int((PatchSize + PatchZ - VertSkip) *
                                                                    m_VertsPerSide + x);
               CurrentIndex++;

               for (z = (float)PatchSize + PatchZ - 1; z >= PatchZ; z -= VertSkip)
               {                  
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(z * m_VertsPerSide + x);
                  CurrentIndex++;
            
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(z * m_VertsPerSide + x + VertSkip);
                  CurrentIndex++;
               }
            }
            else
            {
               // if we're not the first row, then insert degenerate triangles
               if (x != PatchX)
               {
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(PatchZ * m_VertsPerSide 
                                                                       + x);
                  CurrentIndex++;
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(PatchZ * m_VertsPerSide 
                                                                       + x);
                  CurrentIndex++;
               }

               for (z = PatchZ; z < PatchSize + PatchZ; z += VertSkip)
               {
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = 
                  unsigned int(z * m_VertsPerSide + x + VertSkip);
                  CurrentIndex++;
            
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = 
                  unsigned int(z * m_VertsPerSide + x);
                  CurrentIndex++;
            
               }
            }

            SwitchSides = !SwitchSides;
         }

         CurrentPatch++;
      }
   }   



   // put into terrain
   m_InterleavedArray.Initialize(m_pVertices, m_NumVertices, GL_T2F_C4F_N3F_V3F);
   
   m_pPatches = new CPatch[m_NumPatches];
   m_pMiniMapIndices = new CIndexArray[m_NumPatches];

   for (i = 0; i < m_NumPatches; ++i)
   {
      m_pPatches[i].Initialize(&pSpheres[i], ppIndices[i], IndicesPerPatch,
                               GL_TRIANGLE_STRIP, i);
      m_pMiniMapIndices[i].Initialize(ppMiniMapIndices[i], MiniMapIndicesPerPatch,
                                      GL_TRIANGLE_STRIP);      
   }

   // set up lighting
   m_AmbientMaterial.a = 1.0f;
   m_AmbientMaterial.r = .2f;
   m_AmbientMaterial.g = .2f;
   m_AmbientMaterial.b = .2f;

   m_SpecularMaterial.a = 1.0f;
   m_SpecularMaterial.r = 0.0f;
   m_SpecularMaterial.g = 0.0f;
   m_SpecularMaterial.b = 0.0f;

   // load the texture
   m_TextureIndex = g_TextureManager.LoadTexture(pTextureFilePath,
                                                 GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR,
                                                 GL_REPEAT, GL_REPEAT, 0, 0, 0, 0,
                                                 false);

   // free up memory
   delete [] pSpheres;

   delete [] ppIndices;
   delete [] ppMiniMapIndices;      
}

void CTerrain::Initialize(FILE *pTerrainFile)
{
   // set up lighting
   m_AmbientMaterial.a = 1.0f;
   m_AmbientMaterial.r = .2f;
   m_AmbientMaterial.g = .2f;
   m_AmbientMaterial.b = .2f;

   m_SpecularMaterial.a = 1.0f;
   m_SpecularMaterial.r = 0.0f;
   m_SpecularMaterial.g = 0.0f;
   m_SpecularMaterial.b = 0.0f;

   unsigned int Header;
   // file header
   fread(&Header, sizeof(Header), 1, pTerrainFile);
   
   if (Header == PTT_FILE_HEADER_VER2)
   {
      unsigned int Token;
      unsigned int NumPatches;
      unsigned int NumNormals;   

      // num vertices
      fread(&Token, sizeof(Token), 1, pTerrainFile);
      m_pVertices = new CVertexT2FC4FN3FV3F[Token];
      // the verts
      fread(m_pVertices, Token * sizeof(*m_pVertices), 1, pTerrainFile);
      m_InterleavedArray.Initialize(m_pVertices, Token, GL_T2F_C4F_N3F_V3F);

      // num normals
      fread(&NumNormals, sizeof(NumNormals), 1, pTerrainFile);
      m_pNormals = new CVector3f[NumNormals];
      fread(m_pNormals, NumNormals * sizeof(*m_pNormals), 1, pTerrainFile); 
   
      // map info
      fread(&m_VertsPerSide, sizeof(m_VertsPerSide), 1, pTerrainFile);
      fread(&m_HalfXSize, sizeof(m_HalfXSize), 1, pTerrainFile);
      fread(&m_HalfZSize, sizeof(m_HalfZSize), 1, pTerrainFile);
      fread(&m_SmallestY, sizeof(m_SmallestY), 1, pTerrainFile);
      fread(&m_LargestY, sizeof(m_LargestY), 1, pTerrainFile);
   
      m_VertDistX = (m_HalfXSize * 2.0f) / m_VertsPerSide;
      m_VertDistZ = (m_HalfZSize * 2.0f) / m_VertsPerSide;

      // num patches
      fread(&NumPatches, sizeof(NumPatches), 1, pTerrainFile);
      m_pPatches = new CPatch[NumPatches];
      m_pMiniMapIndices = new CIndexArray[NumPatches];

      // patch spheres
      Sphere* pSpheres = new Sphere[NumPatches];
      fread(pSpheres, sizeof(*pSpheres) * NumPatches, 1, pTerrainFile); 

      // number of indices per patch
      fread(&Token, sizeof(Token), 1, pTerrainFile);

      // patch indices
      unsigned int* pIndices;
      for (unsigned int i = 0; i < NumPatches; ++i)
      {
         pIndices = new unsigned int[Token];
         fread(pIndices, sizeof(unsigned int) * Token, 1, pTerrainFile);
         m_pPatches[i].Initialize(&pSpheres[i], pIndices, Token, GL_TRIANGLE_STRIP, i);
      }
   
      // number of indices per minimap patch
      fread(&Token, sizeof(Token), 1, pTerrainFile);

      // minimap patch indices
      for (i = 0; i < NumPatches; ++i)
      {
         pIndices = new unsigned int[Token];
         fread(pIndices, sizeof(unsigned int) * Token, 1, pTerrainFile);
         m_pMiniMapIndices[i].Initialize(pIndices, Token, GL_TRIANGLE_STRIP);
      }

      // texture file path
      fread(m_pTextureFilePath, PT_FILE_PATH_LEN, 1, pTerrainFile);

      m_TextureIndex = g_TextureManager.LoadTexture(m_pTextureFilePath, GL_LINEAR_MIPMAP_LINEAR,
                                                    GL_LINEAR, GL_REPEAT, GL_REPEAT, 0, 0, 0, 0,
                                                    false);

      // read in slope lighting info (no longer used for this terrain version)
      CVector2f temp;
      
      fread(&temp, sizeof(temp), 1, pTerrainFile);
      fread(&Token, sizeof(Token), 1, pTerrainFile);
      fread(&Token, sizeof(Token), 1, pTerrainFile);
      fread(&Token, sizeof(Token), 1, pTerrainFile);
   
      // get patch info
      fread(&m_HalfPatchXSize, sizeof(m_HalfPatchXSize), 1, pTerrainFile);
      fread(&m_HalfPatchYSize, sizeof(m_HalfPatchYSize), 1, pTerrainFile);
      fread(&m_HalfPatchZSize, sizeof(m_HalfPatchZSize), 1, pTerrainFile);

      m_NumPatches = NumPatches;
   
      fclose(pTerrainFile);

      m_NumVertices = m_VertsPerSide * m_VertsPerSide;
   
      delete [] pSpheres;

   }
   // OLD VERSION
   else if (Header == PTT_FILE_HEADER)
   {     
      unsigned int Token;
      unsigned int NumPatches;
      unsigned int NumNormals;

      // num vertices
      fread(&Token, sizeof(Token), 1, pTerrainFile);

      CVertexT2FC4UBV3F* pOldVertices = new CVertexT2FC4UBV3F[Token];
      
      // the verts
      fread(pOldVertices, Token * sizeof(*pOldVertices), 1, pTerrainFile);
      

      // CONVERT THE VERTS TO THE NEW FORMAT
      m_pVertices = new CVertexT2FC4FN3FV3F[Token];

      for (unsigned int i = 0; i < Token; i++)
      {
         m_pVertices[i].m_Position = pOldVertices[i].m_Position;
         m_pVertices[i].m_TextureCoord = pOldVertices[i].m_TextureCoord;
         m_pVertices[i].m_Color.r = 1.0f;
         m_pVertices[i].m_Color.g = 1.0f;
         m_pVertices[i].m_Color.b = 1.0f;
         m_pVertices[i].m_Color.a = 1.0f;
      }
      delete [] pOldVertices;
      m_InterleavedArray.Initialize(m_pVertices, Token, GL_T2F_C4F_N3F_V3F);

      // num normals
      fread(&NumNormals, sizeof(NumNormals), 1, pTerrainFile);
      m_pNormals = new CVector3f[NumNormals];
      fread(m_pNormals, NumNormals * sizeof(*m_pNormals), 1, pTerrainFile); 
   
      // map info
      fread(&m_VertsPerSide, sizeof(m_VertsPerSide), 1, pTerrainFile);
      fread(&m_HalfXSize, sizeof(m_HalfXSize), 1, pTerrainFile);
      fread(&m_HalfZSize, sizeof(m_HalfZSize), 1, pTerrainFile);
      fread(&m_SmallestY, sizeof(m_SmallestY), 1, pTerrainFile);
      fread(&m_LargestY, sizeof(m_LargestY), 1, pTerrainFile);
   
      m_VertDistX = (m_HalfXSize * 2.0f) / m_VertsPerSide;
      m_VertDistZ = (m_HalfZSize * 2.0f) / m_VertsPerSide;

      // num patches
      fread(&NumPatches, sizeof(NumPatches), 1, pTerrainFile);
      m_pPatches = new CPatch[NumPatches];
      m_pMiniMapIndices = new CIndexArray[NumPatches];

      // patch spheres
      Sphere* pSpheres = new Sphere[NumPatches];
      fread(pSpheres, sizeof(*pSpheres) * NumPatches, 1, pTerrainFile); 

      // number of indices per patch
      fread(&Token, sizeof(Token), 1, pTerrainFile);

      // patch indices
      unsigned int* pIndices;
      for (i = 0; i < NumPatches; ++i)
      {
         pIndices = new unsigned int[Token];
         fread(pIndices, sizeof(unsigned int) * Token, 1, pTerrainFile);
         m_pPatches[i].Initialize(&pSpheres[i], pIndices, Token, GL_TRIANGLE_STRIP, i);
      }
      // texture file path
      fread(m_pTextureFilePath, PT_FILE_PATH_LEN, 1, pTerrainFile);

      m_TextureIndex = g_TextureManager.LoadTexture(m_pTextureFilePath, GL_LINEAR_MIPMAP_LINEAR,
                                                    GL_LINEAR, GL_REPEAT, GL_REPEAT, 0, 0, 0, 0,
                                                    false);

      // read in slope lighting info (no longer used for this terrain version)
      CVector2f temp;
      
      fread(&temp, sizeof(temp), 1, pTerrainFile);
      fread(&Token, sizeof(Token), 1, pTerrainFile);
      fread(&Token, sizeof(Token), 1, pTerrainFile);
      fread(&Token, sizeof(Token), 1, pTerrainFile);
   
      // get patch info
      fread(&m_HalfPatchXSize, sizeof(m_HalfPatchXSize), 1, pTerrainFile);
      fread(&m_HalfPatchYSize, sizeof(m_HalfPatchYSize), 1, pTerrainFile);
      fread(&m_HalfPatchZSize, sizeof(m_HalfPatchZSize), 1, pTerrainFile);

      m_NumPatches = NumPatches;
   
      fclose(pTerrainFile);

      m_NumVertices = m_VertsPerSide * m_VertsPerSide;
   
      delete [] pSpheres;
   
      // COMPUTE NORMALS
      RecomputeVertexNormals(); 

      // GENERATE MINIMAP INDICES
      unsigned int VertSkip = 16;
      unsigned int CurrentPatch = 0;
      unsigned int CurrentIndex;
      unsigned int PatchSize = 17;
      unsigned int** ppMiniMapIndices = new unsigned int*[NumPatches];
      unsigned int MiniMapPatchSize = (PatchSize - 1) / VertSkip + 1;
      unsigned int MiniMapIndicesPerPatch = (((MiniMapPatchSize - 1) *
                                            (MiniMapPatchSize - 1)) << 1) + 2 + 
                                            (MiniMapPatchSize - 2) * 4;;
      
      // allocate memory for each of the patchs' indices
      for (i = 0; i < NumPatches; i++)
      {
         ppMiniMapIndices[i] = new unsigned int[MiniMapIndicesPerPatch];
      }
      float PatchX;
      float PatchZ;
      float x;
      float z;
      bool SwitchSides;
      // create simplified patch indices
      for (PatchX = 0; PatchX < m_VertsPerSide - 1; PatchX += PatchSize - 1)
      {   
         for (PatchZ = 0; PatchZ < m_VertsPerSide - 1; PatchZ += PatchSize - 1)
         {
            SwitchSides = false;
            CurrentIndex = 0;

            for (x = PatchX; x < PatchX + PatchSize - 1; x += VertSkip)
            {            

               if (SwitchSides)
               {

                  // since we're starting a new row, insert degenerate triangles
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int((PatchSize + PatchZ - VertSkip) *
                                                                       m_VertsPerSide + x);
                  CurrentIndex++;
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int((PatchSize + PatchZ - VertSkip) *
                                                                       m_VertsPerSide + x);
                  CurrentIndex++;

                  for (z = (float)PatchSize + PatchZ - 1; z >= PatchZ; z -= VertSkip)
                  {                  
                     ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(z * m_VertsPerSide + x);
                     CurrentIndex++;
            
                     ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(z * m_VertsPerSide + x + VertSkip);
                     CurrentIndex++;
                  }
               }
               else
               {
                  // if we're not the first row, then insert degenerate triangles
                  if (x != PatchX)
                  {
                     ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(PatchZ * m_VertsPerSide 
                                                                          + x);
                     CurrentIndex++;
                     ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(PatchZ * m_VertsPerSide 
                                                                          + x);
                     CurrentIndex++;
                  }

                  for (z = PatchZ; z < PatchSize + PatchZ; z += VertSkip)
                  {
                     ppMiniMapIndices[CurrentPatch][CurrentIndex] = 
                     unsigned int(z * m_VertsPerSide + x + VertSkip);
                     CurrentIndex++;
            
                     ppMiniMapIndices[CurrentPatch][CurrentIndex] = 
                     unsigned int(z * m_VertsPerSide + x);
                     CurrentIndex++;
            
                  }
               }

               SwitchSides = !SwitchSides;
            }

            CurrentPatch++;

         }
      }

      // initialize minimap patch indices
      for (i = 0; i < NumPatches; ++i)
      {
         m_pMiniMapIndices[i].Initialize(ppMiniMapIndices[i], MiniMapIndicesPerPatch, GL_TRIANGLE_STRIP);
      }

   
      delete [] ppMiniMapIndices;
   
   }
   else
      // INVALID VERSION
      return; 
}

// set up states to begin rendering the terrain
void CTerrain::BeginRendering()
{
   pGLSTATE->SetState(TEXTURE_2D, true);
   m_InterleavedArray.Enable();

   g_TextureManager.ActivateTexture(m_TextureIndex);

   glMaterialfv(GL_FRONT, GL_AMBIENT, (float *)&m_AmbientMaterial);
   glMaterialfv(GL_FRONT, GL_SPECULAR, (float *)&m_SpecularMaterial);
   
   pGLSTATE->SetState(LIGHTING, true);
   pGLSTATE->SetState(COLOR_MATERIAL, true);
}

// enable the terrain vertex array
void CTerrain::EnableVerts()
{
   m_InterleavedArray.Enable();   
}