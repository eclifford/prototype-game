#include <math.h>

#include "graphics\pttfile.h"
#include "math3d\math3d.h"
#include "graphics\vertext2fc4fn3fv3f.h"
#include "graphics\heightmap.h"
#include "math3d\primitives.h"
#include <cstdio>
#include "graphics\ptgfile.h"

#include "utils\mmgr.h"

void HeightMapToPTT(const char* pHeightMapFilePath, const char* pTextureFilePath,
                    const char* pPTTFilePath, unsigned int PatchSize, float TextureTileWidth,
                    float TextureTileHeight, float XScale, float YScale, float ZScale,
                    const CVector2f* pSunDir, float ShadowFactor, float MaxBrightness,
                    float MinBrightness)
{         
   CHeightMap HeightMap;
   HeightMap.Initialize(pHeightMapFilePath);
   
   unsigned int HeightMapSize = HeightMap.GetSize();
   unsigned int VertsPerSide = HeightMapSize;
   float HalfXSize;
   float HalfZSize;
   float HalfSize = HalfXSize = HalfZSize = HeightMapSize * .5f;
   HalfXSize *= XScale;
   HalfZSize *= ZScale;

   // figure out the # of patches per side

   //if (HeightMapSize % PatchSize + HeightMapSize / PatchSize == PatchSize)
   unsigned int PatchesPerSide = 1 + (HeightMapSize - PatchSize) / (PatchSize - 1);
   //PatchesPerSide = PatchesPerSide;

   /*
   // figure out how many LOD levels we can generate based on the patch size
   unsigned int Divisor = PatchSize - 1;
	unsigned int LODLevels = 0;
	while (Divisor > 2)
	{
		Divisor >>= 1;
		LODLevels++;
	}*/

   // allocate memory for the verts and patches
   unsigned int NumVertices = HeightMapSize * HeightMapSize;
   CVertexT2FC4FN3FV3F *pVertices = new CVertexT2FC4FN3FV3F[NumVertices];
   unsigned int NumPatches = PatchesPerSide * PatchesPerSide;
   //pPatches = new Patch[NumPatches];
   Sphere* pSpheres = new Sphere[NumPatches];

   unsigned int IndicesPerPatch = (((PatchSize - 1) * (PatchSize - 1)) << 1) + 2 + 
                                  (PatchSize - 2) * 4;
   
   unsigned int** ppIndices = new unsigned int*[NumPatches];
   // allocate memory for each of the patchs' indices
   for (unsigned int i = 0; i < NumPatches; i++)
   {
      ppIndices[i] = new unsigned int[IndicesPerPatch];
   } 

   // create the vertices for the heightmap (centered around 0, 128, 0)
   float x, z;
   unsigned int CurrentVert = 0;
   bool SwitchSides = false;
   unsigned int CurrentPatch = 0;
   unsigned int CurrentIndex = 0;
   
   float VertDistX = XScale;
   float VertDistZ = ZScale;

   float LargestY = -127.0f * YScale;
   float SmallestY = 128.0f * YScale;
   
   float HalfPatchSizeX = (PatchSize - 1) * VertDistX * .5f;
   float HalfPatchSizeZ = (PatchSize - 1) * VertDistZ * .5f;
   float HalfPatchSizeY = (LargestY - SmallestY) * .5f;

   for (z = -HalfSize; z < HalfSize; z++)
   {
      for (x = -HalfSize; x < HalfSize; x++)
      {
         pVertices[CurrentVert].m_Position.x = x * XScale;
         pVertices[CurrentVert].m_Position.y = ((float)HeightMap.GetHeight(int(x + HalfSize),
                                                                   int(z + HalfSize)) - 127)
                                                                   * YScale;
         
         if (pVertices[CurrentVert].m_Position.y > LargestY)
            LargestY = pVertices[CurrentVert].m_Position.y;

         if (pVertices[CurrentVert].m_Position.y < SmallestY)
            SmallestY = pVertices[CurrentVert].m_Position.y;
         
         pVertices[CurrentVert].m_Position.z = z * ZScale;
         
         pVertices[CurrentVert].m_TextureCoord.x = (pVertices[CurrentVert].m_Position.x + HalfSize
                                                    * XScale) / TextureTileWidth;
         pVertices[CurrentVert].m_TextureCoord.y = (pVertices[CurrentVert].m_Position.z + HalfSize
                                                    * ZScale) / TextureTileHeight;                  

         CurrentVert++;
      }
   }

   // do the lighting
   //int ShadowVert;
   //float Color;
   for (CurrentVert = 0; CurrentVert < NumVertices; CurrentVert++)
   {
      /*
      ShadowVert = unsigned int(CurrentVert - pSunDir->x - pSunDir->y * HeightMapSize); 
      
      if (ShadowVert < 0 || ShadowVert > (int)NumVertices)
         Color = MaxBrightness - MinBrightness;
      else
         Color = 1 - (pVertices[ShadowVert].m_Position.y - pVertices[CurrentVert].m_Position.y) *
                      ShadowFactor;

      if (Color > MaxBrightness)
         Color = MaxBrightness;
      else if (Color < MinBrightness)
         Color = MinBrightness;

      Color *= 255;

      pVertices[CurrentVert].m_Color.r = (unsigned int)Color;
      pVertices[CurrentVert].m_Color.g = (unsigned int)Color;
      pVertices[CurrentVert].m_Color.b = (unsigned int)Color;
      pVertices[CurrentVert].m_Color.a = 255;
      */

      pVertices[CurrentVert].m_Color.r = 1.0f;
      pVertices[CurrentVert].m_Color.g = 1.0f;
      pVertices[CurrentVert].m_Color.b = 1.0f;
      pVertices[CurrentVert].m_Color.a = 1.0f;      
   }

   // determine triangle normals
   
   /*
   VertDistZ = 1.0f * ZScale;
   VertDistX = 1.0f * XScale;

   // scale them
   if (XScale != 1.0f || YScale != 1.0f || ZScale != 1.0f)
   {
      HalfXSize *= XScale;
      HalfZSize *= ZScale;

      for (CurrentVert = 0; CurrentVert < NumVertices; CurrentVert++)
      {
         if (XScale != 1.0f)
            pVertices[CurrentVert].Vertex.x *= XScale;
            
         if (YScale != 1.0f)
            pVertices[CurrentVert].Vertex.y *= YScale;
            
         if (ZScale != 1.0f)
            pVertices[CurrentVert].Vertex.z *= ZScale; 
      }
   }*/

   // generate face normals
   unsigned int NumNormals = (HeightMapSize - 1) * (HeightMapSize - 1) * 2;
   CVector3f* pNormals = new CVector3f[NumNormals];
   unsigned int CurrNormal = 0;
   CVector3f vec1;
   CVector3f vec2;
   for (z = 0; z < VertsPerSide - 1; z++)
   {
      for (x = 0; x < VertsPerSide - 1; x++)
      {
         CurrentVert = unsigned int(x + z * VertsPerSide);

         Vec3fSubtract(&vec1, &pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                       &pVertices[CurrentVert + 1].m_Position);
         Vec3fSubtract(&vec2, &pVertices[CurrentVert].m_Position,
                       &pVertices[CurrentVert + 1].m_Position);
         Vec3fCrossProduct(&pNormals[CurrNormal], &vec2, &vec1);
         Vec3fNormalize(&pNormals[CurrNormal], &pNormals[CurrNormal]);
         CurrNormal++;

         Vec3fSubtract(&vec1, &pVertices[CurrentVert + HeightMapSize].m_Position,
                       &pVertices[CurrentVert].m_Position);
         Vec3fSubtract(&vec2, &pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                       &pVertices[CurrentVert].m_Position);
         Vec3fCrossProduct(&pNormals[CurrNormal], &vec1, &vec2);
         Vec3fNormalize(&pNormals[CurrNormal], &pNormals[CurrNormal]);
         CurrNormal++;
      }
   }

   unsigned int ZIndex;
   unsigned int XIndex;
   unsigned int NumQuads = HeightMapSize - 1;
   
   // generate vertex normals
   for (ZIndex = 0; ZIndex < VertsPerSide - 1; ZIndex++)
   {
      for (XIndex = 0; XIndex < VertsPerSide - 1; XIndex++)
      {
         // go through all the triangles that this vertex is adjacent to
         // and recompute the normals
         unsigned int VertIndex = XIndex + ZIndex * HeightMapSize;

         unsigned int NormIndex = ((HeightMapSize - 1) * ZIndex + XIndex) * 2;         

         // if this vertex isn't at the very top of the map
         if (ZIndex < NumQuads)
         {
            // if this vertex isn't at the very right of the map
            if (XIndex < NumQuads)
            {
               // recompute the normals for this quad              

               // NormIndex  

               // NormIndex + 1;
      
               if (ZIndex > 0)
               {
                  //CurrentVert = VertIndex - HeightMapSize;
                  //CurrentNorm = NormIndex - ((HeightMapSize - 1) * 2); 
                  
                  // upper left tri of quad below
                  /*
                  ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize].m_Position,
                                &m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                                &m_pVertices[CurrentVert].m_Position,
                                &m_pNormals[CurrentNorm + 1]);
                  */
                  if (XIndex > 0)
                  {
                     //CurrentVert = VertIndex - 1;
                     //CurrentNorm = NormIndex - 2; 
      
                     /*
                     // bottom right tri of quad to the left
                     ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                                   &m_pVertices[CurrentVert + 1].m_Position,
                                   &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);  
                     */
      
                     // both tris in the quad below and to the left
                     //CurrentVert = VertIndex - HeightMapSize - 1;
                     //CurrentNorm = NormIndex - (HeightMapSize - 1) * 2 - 2;

                     /*
                     // bottom right tri first           
                     ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                                   &m_pVertices[CurrentVert + 1].m_Position,
                                   &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);  

                     // upper left tri
                     ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize].m_Position,
                                   &m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                                   &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm + 1]);
                  
                     */

                     Vec3fAdd(&pVertices[VertIndex].m_Normal, &pNormals[NormIndex],
                              &pNormals[NormIndex + 1]);
                     Vec3fAdd(&pVertices[VertIndex].m_Normal,
                              &pVertices[VertIndex].m_Normal,
                              &pNormals[NormIndex - ((HeightMapSize - 1) * 2) + 1]);
                     Vec3fAdd(&pVertices[VertIndex].m_Normal,
                              &pVertices[VertIndex].m_Normal,
                              &pNormals[NormIndex - 2]);
                     Vec3fAdd(&pVertices[VertIndex].m_Normal,
                              &pVertices[VertIndex].m_Normal,
                              &pNormals[NormIndex - (HeightMapSize - 1) * 2 - 2]);
                     Vec3fAdd(&pVertices[VertIndex].m_Normal,
                              &pVertices[VertIndex].m_Normal,
                              &pNormals[NormIndex - (HeightMapSize - 1) * 2 - 1]);
                     Vec3fScale(&pVertices[VertIndex].m_Normal,
                                &pVertices[VertIndex].m_Normal, 0.16666666666666666666666666666667f);
                  }
                  else
                  {
                     Vec3fAdd(&pVertices[VertIndex].m_Normal, &pNormals[NormIndex],
                              &pNormals[NormIndex + 1]);
                     Vec3fAdd(&pVertices[VertIndex].m_Normal,
                              &pVertices[VertIndex].m_Normal,
                              &pNormals[NormIndex - ((HeightMapSize - 1) * 2) + 1]);
                     Vec3fScale(&pVertices[VertIndex].m_Normal,
                                &pVertices[VertIndex].m_Normal, .33333333333333333333333333333333f);
                  }
      
               }
               else if (XIndex > 0)
               {
                  // bottom right tri of quad to the left
                  //CurrentVert = VertIndex - 1;
                  //CurrentNorm = NormIndex - 2; 

                  /*
                  // bottom right tri of quad to the left
                  ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                                &m_pVertices[CurrentVert + 1].m_Position,
                                &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);
                  */

                  Vec3fAdd(&pVertices[VertIndex].m_Normal, &pNormals[NormIndex],
                           &pNormals[NormIndex + 1]);
                  Vec3fAdd(&pVertices[VertIndex].m_Normal,
                           &pVertices[VertIndex].m_Normal,
                           &pNormals[NormIndex - 2]);
                  Vec3fScale(&pVertices[VertIndex].m_Normal,
                             &pVertices[VertIndex].m_Normal, .33333333333333333333333333333333f);
               }
               else
               {
                  Vec3fAdd(&pVertices[VertIndex].m_Normal, &pNormals[NormIndex],
                           &pNormals[NormIndex + 1]);
                  Vec3fScale(&pVertices[VertIndex].m_Normal,
                             &pVertices[VertIndex].m_Normal, .5f);
               }


            }
            else
            {
               // bottom right tri of quad to the left
               //CurrentVert = VertIndex - 1;
               //CurrentNorm = NormIndex - 2; 

               /*
               // bottom right tri of quad to the left
               ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                             &m_pVertices[CurrentVert + 1].m_Position,
                             &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);
   
               */
               if (ZIndex > 0)
               {
                  // both tris in the quad below and to the left
                  //CurrentVert = VertIndex - HeightMapSize - 1;
                  //CurrentNorm = NormIndex - HeightMapSize - 1 - 2;

                  /*
                  // bottom right tri first           
                  ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                                &m_pVertices[CurrentVert + 1].m_Position,
                                &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);  

                  // upper left tri
                  ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize].m_Position,
                                &m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                                &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm + 1]);
                  */

                  Vec3fAdd(&pVertices[VertIndex].m_Normal, &pNormals[NormIndex - 2],
                           &pNormals[NormIndex - HeightMapSize - 3]);
                  Vec3fAdd(&pVertices[VertIndex].m_Normal,
                           &pVertices[VertIndex].m_Normal,
                           &pNormals[NormIndex - HeightMapSize - 2]);
                  Vec3fScale(&pVertices[VertIndex].m_Normal,
                             &pVertices[VertIndex].m_Normal, .33333333333333333333333333333333f);
               }
               else
               {
                  pVertices[VertIndex].m_Normal = pNormals[NormIndex - 2];   
               }
            }
         }
         else
         {
            if (XIndex > 0)
            {
               // both tris in the quad below and to the left
               //CurrentVert = VertIndex - HeightMapSize - 1;
               //CurrentNorm = NormIndex - (HeightMapSize - 1) * 2 - 2;

               /*
               // bottom right tri first           
               ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                             &m_pVertices[CurrentVert + 1].m_Position,
                             &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm]);  

               // upper left tri
               ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize].m_Position,
                             &m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                             &m_pVertices[CurrentVert].m_Position, &m_pNormals[CurrentNorm + 1]);
               */
               
               if (XIndex < NumQuads)
               {
                  //CurrentVert = VertIndex - HeightMapSize;
                  //CurrentNorm = NormIndex - (HeightMapSize - 1) * 2; 
                  
                  /*
                  // upper left tri of quad below
                  ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize].m_Position,
                                &m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                                &m_pVertices[CurrentVert].m_Position,
                                &m_pNormals[CurrentNorm + 1]);
                  */

                  Vec3fAdd(&pVertices[VertIndex].m_Normal,
                           &pNormals[NormIndex - (HeightMapSize - 1) * 2 - 2],
                           &pNormals[NormIndex - (HeightMapSize - 1) * 2 - 1]);
                  Vec3fAdd(&pVertices[VertIndex].m_Normal,
                           &pVertices[VertIndex].m_Normal,
                           &pNormals[NormIndex - (HeightMapSize - 1) * 2 + 1]);
                  Vec3fScale(&pVertices[VertIndex].m_Normal,
                             &pVertices[VertIndex].m_Normal, .33333333333333333333333333333333f);
               }
               else
               {
                  Vec3fAdd(&pVertices[VertIndex].m_Normal,
                           &pNormals[NormIndex - (HeightMapSize - 1) * 2 - 2],
                           &pNormals[NormIndex - (HeightMapSize - 1) * 2 - 1]);
                  Vec3fScale(&pVertices[VertIndex].m_Normal,
                             &pVertices[VertIndex].m_Normal, .5f);
               }
            }
            else
            {
               //CurrentVert = VertIndex - HeightMapSize;
               //CurrentNorm = NormIndex - (HeightMapSize - 1) * 2; 
               
               /*   
               // upper left tri of quad below
               ComputeNormal(&m_pVertices[CurrentVert + HeightMapSize].m_Position,
                             &m_pVertices[CurrentVert + HeightMapSize + 1].m_Position,
                             &m_pVertices[CurrentVert].m_Position,
                             &m_pNormals[CurrentNorm + 1]);
               */

               pVertices[VertIndex].m_Normal = pNormals[NormIndex - (HeightMapSize - 1) * 2 + 1];
            }
         }   
      }
   }

   //XScale = XScale;
   //YScale = YScale;
   //ZScale = ZScale;
  
   // create the indices
   float PatchX, PatchZ;
   float MiddleY = 0.0f;

   for (PatchX = 0; PatchX < HeightMapSize - 1; PatchX += PatchSize - 1)
   {   
      for (PatchZ = 0; PatchZ < HeightMapSize - 1; PatchZ += PatchSize - 1)
      {
         SwitchSides = false;
         CurrentIndex = 0;

         for (x = PatchX; x < PatchX + PatchSize - 1; x++)
         {            

            if (SwitchSides)
            {

               // since we're starting a new row, insert degenerate triangles
               ppIndices[CurrentPatch][CurrentIndex] = unsigned int((PatchSize + PatchZ - 1) *
                                                                    HeightMapSize + x);
               CurrentIndex++;
               ppIndices[CurrentPatch][CurrentIndex] = unsigned int((PatchSize + PatchZ - 1) *
                                                                    HeightMapSize + x);
               CurrentIndex++;

               for (z = (float)PatchSize + PatchZ - 1; z >= PatchZ; z--)
               {                  
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(z * HeightMapSize + x);
                  MiddleY += pVertices[unsigned int(z * HeightMapSize + x)].m_Position.y;
                  CurrentIndex++;
            
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(z * HeightMapSize + x + 1);
                  MiddleY += pVertices[unsigned int(z * HeightMapSize + x + 1)].m_Position.y;
                  CurrentIndex++;
               }
            }
            else
            {
               // if we're not the first row, then insert degenerate triangles
               if (x != PatchX)
               {
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(PatchZ * HeightMapSize 
                                                                       + x);
                  CurrentIndex++;
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(PatchZ * HeightMapSize 
                                                                       + x);
                  CurrentIndex++;
               }

               for (z = PatchZ; z < PatchSize + PatchZ; z++)
               {
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(z * HeightMapSize + x + 1);
                  MiddleY += pVertices[unsigned int(z * HeightMapSize + x + 1)].m_Position.y;                  
                  CurrentIndex++;
            
                  ppIndices[CurrentPatch][CurrentIndex] = unsigned int(z * HeightMapSize + x);
                  MiddleY += pVertices[unsigned int(z * HeightMapSize + x)].m_Position.y;                  
                  CurrentIndex++;
            
               }
            }

            SwitchSides = !SwitchSides;
         }

         pSpheres[CurrentPatch].Center.x = (PatchX / (PatchSize - 1) * (HalfPatchSizeX * 2)) + HalfPatchSizeX - HalfXSize;
         pSpheres[CurrentPatch].Center.z = (PatchZ / (PatchSize - 1) * (HalfPatchSizeZ * 2)) + HalfPatchSizeZ - HalfZSize;
         
         pSpheres[CurrentPatch].Center.y = MiddleY / (float)CurrentIndex; 
         
         // find the vert furthest away from the center to get the radius
         pSpheres[CurrentPatch].Radius = 0.0f;
         for (unsigned int i = 0; i < IndicesPerPatch; i++)
         {
            float Dist = powf(pVertices[ppIndices[CurrentPatch][i]].m_Position.x -
                              pSpheres[CurrentPatch].Center.x, 2) +
                         powf(pVertices[ppIndices[CurrentPatch][i]].m_Position.y -
                              pSpheres[CurrentPatch].Center.y, 2) +
                         powf(pVertices[ppIndices[CurrentPatch][i]].m_Position.z -
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
   
   unsigned int** ppMiniMapIndices = new unsigned int*[NumPatches];
   unsigned int MiniMapPatchSize = (PatchSize - 1) / VertSkip + 1;
   unsigned int MiniMapIndicesPerPatch = (((MiniMapPatchSize - 1) *
                                         (MiniMapPatchSize - 1)) << 1) + 2 + 
                                         (MiniMapPatchSize - 2) * 4;
   // allocate memory for each of the patchs' indices
   for (i = 0; i < NumPatches; i++)
   {
      ppMiniMapIndices[i] = new unsigned int[MiniMapIndicesPerPatch];
   }

   // create simplified patch indices
   for (PatchX = 0; PatchX < HeightMapSize - 1; PatchX += PatchSize - 1)
   {   
      for (PatchZ = 0; PatchZ < HeightMapSize - 1; PatchZ += PatchSize - 1)
      {
         SwitchSides = false;
         CurrentIndex = 0;

         for (x = PatchX; x < PatchX + PatchSize - 1; x += VertSkip)
         {            

            if (SwitchSides)
            {

               // since we're starting a new row, insert degenerate triangles
               ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int((PatchSize + PatchZ - VertSkip) *
                                                                    HeightMapSize + x);
               CurrentIndex++;
               ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int((PatchSize + PatchZ - VertSkip) *
                                                                    HeightMapSize + x);
               CurrentIndex++;

               for (z = (float)PatchSize + PatchZ - 1; z >= PatchZ; z -= VertSkip)
               {                  
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(z * HeightMapSize + x);
                  //MiddleY += pVertices[unsigned int(z * HeightMapSize + x)].m_Position.y;
                  CurrentIndex++;
            
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(z * HeightMapSize + x + VertSkip);
                  //MiddleY += pVertices[unsigned int(z * HeightMapSize + x + VertSkip)].m_Position.y;
                  CurrentIndex++;
               }
            }
            else
            {
               // if we're not the first row, then insert degenerate triangles
               if (x != PatchX)
               {
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(PatchZ * HeightMapSize 
                                                                       + x);
                  CurrentIndex++;
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = unsigned int(PatchZ * HeightMapSize 
                                                                       + x);
                  CurrentIndex++;
               }

               for (z = PatchZ; z < PatchSize + PatchZ; z += VertSkip)
               {
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = 
                  unsigned int(z * HeightMapSize + x + VertSkip);
                  //MiddleY += pVertices[unsigned int(z * HeightMapSize + x + VertSkip)].m_Position.y;                  
                  CurrentIndex++;
            
                  ppMiniMapIndices[CurrentPatch][CurrentIndex] = 
                  unsigned int(z * HeightMapSize + x);
                  //MiddleY += pVertices[unsigned int(z * HeightMapSize + x)].m_Position.y;                  
                  CurrentIndex++;
            
               }
            }

            SwitchSides = !SwitchSides;
         }
         
         /*
         pSpheres[CurrentPatch].Center.x = (PatchX / (PatchSize - 1) * (HalfPatchSizeX * 2)) + HalfPatchSizeX - HalfXSize;
         pSpheres[CurrentPatch].Center.z = (PatchZ / (PatchSize - 1) * (HalfPatchSizeZ * 2)) + HalfPatchSizeZ - HalfZSize;
         
         pSpheres[CurrentPatch].Center.y = MiddleY / (float)CurrentIndex; 
         
         // find the vert furthest away from the center to get the radius
         pSpheres[CurrentPatch].Radius = 0.0f;
         for (unsigned int i = 0; i < IndicesPerPatch; i++)
         {
            float Dist = powf(pVertices[ppIndices[CurrentPatch][i]].m_Position.x -
                              pSpheres[CurrentPatch].Center.x, 2) +
                         powf(pVertices[ppIndices[CurrentPatch][i]].m_Position.y -
                              pSpheres[CurrentPatch].Center.y, 2) +
                         powf(pVertices[ppIndices[CurrentPatch][i]].m_Position.z -
                              pSpheres[CurrentPatch].Center.z, 2);
            
            if (Dist > pSpheres[CurrentPatch].Radius)
               pSpheres[CurrentPatch].Radius = Dist; 
         }
         pSpheres[CurrentPatch].Radius = sqrtf(pSpheres[CurrentPatch].Radius);
         */

         CurrentPatch++;
         //MiddleY = 0.0f;  
      }
   }
   
   // load in the texture
   //int TextureIndex = g_TextureManager.StoreTextureFilePath(pTextureFilePath);
   //g_TextureManager.LoadTexture(TextureIndex, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT,
   //                             GL_REPEAT);


   //Sphere fakesphere;
   //ZERO_OBJECT(fakesphere);
   //GeometryIndex = g_GeometryManager.CreateGeometry(pVertices, GL_T2F_V3F, NumVertices,
   //                                                  pMeshes, NumPatches, TextureIndex,
   //                                                  &fakesphere);
   
   //g_GeometryManager.LoadPTGFile(pFilePath, &GeometryIndex, &TextureIndex, NULL);

   /*
   for (i = 0; i < NumPatches; i++)
   {
      unsigned int* pMeshIndices = new unsigned int[1];
      pMeshIndices[0] = i;
      pPatches[i].Renderable.Initialize(GeometryIndex, 1, pMeshIndices, TextureIndex,
                                            &pPatches[i].BoundSphere);
   }*/

   

   // save out the PTT File

   FILE* pPTTFile = fopen(pPTTFilePath, "wb");

   // file header
   unsigned int FileHeader = PTT_FILE_HEADER_VER2;
   fwrite(&FileHeader, sizeof(FileHeader), 1, pPTTFile);

   // num vertices
   fwrite(&NumVertices, sizeof(NumVertices), 1, pPTTFile);
   // save the verts
   fwrite(pVertices, sizeof(*pVertices) * NumVertices, 1, pPTTFile);
   // num normals
   fwrite(&NumNormals, sizeof(NumNormals), 1, pPTTFile);
   // save the normals
   fwrite(pNormals, sizeof(*pNormals) * NumNormals, 1, pPTTFile);

   
   // save the info
   fwrite(&VertsPerSide, sizeof(VertsPerSide), 1, pPTTFile);
   fwrite(&HalfXSize, sizeof(HalfXSize), 1, pPTTFile);
   fwrite(&HalfZSize, sizeof(HalfZSize), 1, pPTTFile);
   fwrite(&SmallestY, sizeof(SmallestY), 1, pPTTFile);
   fwrite(&LargestY, sizeof(LargestY), 1, pPTTFile);

   // num patches
   fwrite(&NumPatches, sizeof(NumPatches), 1, pPTTFile);

   // patch spheres
   fwrite(pSpheres, sizeof(*pSpheres) * NumPatches, 1, pPTTFile); 

   // number of indices per patch
   fwrite(&IndicesPerPatch, sizeof(IndicesPerPatch), 1, pPTTFile);

   // patch indices
   for (i = 0; i < NumPatches; i++)
   {
      fwrite(ppIndices[i], sizeof(unsigned int) * IndicesPerPatch, 1, pPTTFile);
   }

   // number of mini-map indices per patch
   fwrite(&MiniMapIndicesPerPatch, sizeof(MiniMapIndicesPerPatch), 1, pPTTFile);

   // mini-map indices
   for (i = 0; i < NumPatches; i++)
   {
      fwrite(ppMiniMapIndices[i], sizeof(unsigned int) * MiniMapIndicesPerPatch, 1, pPTTFile);
   }


   // save the texture file path
   char pSizedTextureFilePath[PT_FILE_PATH_LEN];
   strcpy(pSizedTextureFilePath, pTextureFilePath);
   fwrite(pSizedTextureFilePath, PT_FILE_PATH_LEN, 1, pPTTFile);

   // save out slope lighting info
   fwrite(pSunDir, sizeof(*pSunDir), 1, pPTTFile);
   fwrite(&ShadowFactor, sizeof(ShadowFactor), 1, pPTTFile);
   fwrite(&MaxBrightness, sizeof(MaxBrightness), 1, pPTTFile);
   fwrite(&MinBrightness, sizeof(MinBrightness), 1, pPTTFile);

   // save out patch info
   fwrite(&HalfPatchSizeX, sizeof(HalfPatchSizeX), 1, pPTTFile);
   fwrite(&HalfPatchSizeY, sizeof(HalfPatchSizeY), 1, pPTTFile);
   fwrite(&HalfPatchSizeZ, sizeof(HalfPatchSizeZ), 1, pPTTFile);

   fclose(pPTTFile);
   // free up memory
   delete [] pSpheres;
   delete [] pNormals;
   delete [] pVertices;
   
   for (i = 0; i < NumPatches; i++)
   {
      delete [] ppIndices[i];
   }

   for (i = 0; i < NumPatches; i++)
   {
      delete [] ppMiniMapIndices[i];
   }

   delete [] ppIndices;
   delete [] ppMiniMapIndices;
}