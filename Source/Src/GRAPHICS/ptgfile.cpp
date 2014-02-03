#include "graphics\ptgfile.h"
#include "graphics\vertext2fn3fv3f.h"
#include "graphics\vertext2fv3f.h"
#include "graphics\texturemanager.h"
#include "graphics\glinclude.h"
#include <vector>
using namespace std;

#include "utils\mmgr.h"

extern CTextureManager g_TextureManager;

void OBJToPTG(const char* pOBJFilePath, const char* pTextureFilePath, const char* pPTGFilePath,
              bool CenterOnOrigin, float XScale, float YScale, float ZScale)
{
   FILE* pFile = fopen(pOBJFilePath, "r");

   char Token[255];
   vector<char*> UniqueVertexList;
   vector<CVector3f*> VertexList;
   vector<CVector3f*> NormalList;
   vector<CVector2f*> TexCoordList;
   vector<int> NumIndicesList;

   unsigned int NumVertices = 0;
   
   // get the data from the file
   while (fscanf(pFile, "%s", Token) != EOF)
   {
      // if its an index		
      if (!strcmp("f", Token))
      {
         // get the vertex info
         for (int i = 0; i < 3; i++)
         {
            fscanf(pFile, " %s", Token);

            for (int j = 0; j < (int)UniqueVertexList.size(); j++)
            {
               if (!strcmp(Token, UniqueVertexList[j]))
                  break;
            }

            if (j == UniqueVertexList.size())
            {
               char* pNewVertex = new char[255];
               strcpy(pNewVertex, Token);
               UniqueVertexList.push_back(pNewVertex);               
            }
         }

         NumIndicesList.back() += 3;       
      }
      // if its a vertex
      else if (!strcmp("v", Token))
      {
         CVector3f* pVertex = new CVector3f;
         fscanf(pFile, " %f %f %f", &pVertex->x,
                                    &pVertex->y,
                                    &pVertex->z);

         pVertex->x *= XScale;
         pVertex->y *= YScale;
         pVertex->z *= ZScale;

         VertexList.push_back(pVertex);      
      }
      // if its a texture coord
      else if (!strcmp("vt", Token))
      {
         CVector2f* pTexCoord = new CVector2f;
         fscanf(pFile, " %f %f", &pTexCoord->x, &pTexCoord->y);

         TexCoordList.push_back(pTexCoord);
      }
      // if its a normal
      else if (!strcmp("vn", Token))
      {
         CVector3f* pNormal = new CVector3f;
         fscanf(pFile, " %f %f %f", &pNormal->x,
                                    &pNormal->y,
                                    &pNormal->z);
         
         NormalList.push_back(pNormal);
      }
      // if its a group
      else if (!strcmp("g", Token))
      {
         fscanf(pFile, "%s", Token);
         if (strcmp("default", Token))
            NumIndicesList.push_back(0);         
      }
   }

   NumVertices = (unsigned int)UniqueVertexList.size();
  
   // clear out the unique vertex list
   for (unsigned int i = 0; i < NumVertices; i++)
   {
      delete [] UniqueVertexList[i];
   }

   UniqueVertexList.clear();

   CVertexT2FN3FV3F* pVertices = new CVertexT2FN3FV3F[NumVertices];
   
   // allocate and zero out the translations
   CVector3f* pTranslations = new CVector3f[NumIndicesList.size()];
   memset(pTranslations, 0, sizeof(CVector3f) * NumIndicesList.size());

   // allocate memory for the index arrays for each mesh
   unsigned int** ppIndices = new unsigned int*[NumIndicesList.size()];
   for (i = 0; i < NumIndicesList.size(); i++)
   {
      ppIndices[i] = new unsigned int[NumIndicesList[i]];   
   }
   
   // find the bounding box
   CVector3f Max = *VertexList[0];
   CVector3f Min = *VertexList[0];
   for (i = 0; i < VertexList.size(); i++)
   {
      if (VertexList[i]->x > Max.x)
         Max.x = VertexList[i]->x;
      else if (VertexList[i]->x < Min.x)
         Min.x = VertexList[i]->x;

      if (VertexList[i]->y > Max.y)
         Max.y = VertexList[i]->y;
      else if (VertexList[i]->y < Min.y)
         Min.y = VertexList[i]->y;

      if (VertexList[i]->z > Max.z)
         Max.z = VertexList[i]->z;
      else if (VertexList[i]->z < Min.z)
         Min.z = VertexList[i]->z;
   }
 
   // get the center
   Sphere BoundingSphere;
   BoundingSphere.Center.x = (Max.x + Min.x) * .5f;
   BoundingSphere.Center.y = (Max.y + Min.y) * .5f;
   BoundingSphere.Center.z = (Max.z + Min.z) * .5f;
   
   // center the vertices around 0, 0, 0
   if (CenterOnOrigin)
   {
      Vec3fSubtract(&Max, &Max, &BoundingSphere.Center);
      Vec3fSubtract(&Min, &Min, &BoundingSphere.Center);
      // offset all the verts by the inverse of the center to move it to 0, 0, 0
      for (i = 0; i < VertexList.size(); i++)
      {
         Vec3fSubtract(VertexList[i], VertexList[i], &BoundingSphere.Center);
      }

      BoundingSphere.Center.x = 0.0f;
      BoundingSphere.Center.y = 0.0f;
      BoundingSphere.Center.z = 0.0f;
   
      BoundingSphere.Radius = Vec3fMagnitude(&Min);
   }
   else
   {
      // find the radius
      CVector3f temp;
      Vec3fSubtract(&temp, &Min, &BoundingSphere.Center);      
      BoundingSphere.Radius = Vec3fMagnitude(&temp);  
   }

   fseek(pFile, 0, SEEK_SET);

   unsigned int CurrentIndex = 0;

   unsigned int CurrentVertex = 0;
   int CurrentGroup = -1;   
   // go through the faces
   while (fscanf(pFile, "%s", Token) != EOF)
   {
      // if its a new group
      if (!strcmp("g", Token))
      {
         fscanf(pFile, "%s", Token);
         if (strcmp("default", Token))
         {
            CurrentGroup++;
            CurrentIndex = 0;
         }
      }
      else if (!strcmp("f", Token))
      {
         for (int i = 0; i < 3; i++)
         {
            int Vert, Tex, Norm;
            fscanf(pFile, " %d/%d/%d", &Vert, &Tex, &Norm);
         
            // check to see if we already have this combination in our arrays
            for (unsigned int j = 0; j < CurrentVertex; j++)
            {
               if (pVertices[j].m_Vertex == *VertexList[Vert - 1] && 
                   pVertices[j].m_Normal == *NormalList[Norm - 1] &&
                   pVertices[j].m_TextureCoord == *TexCoordList[Tex - 1])
               {
                  break;
               }  
            }
            if (j == CurrentVertex)
            {
               pVertices[CurrentVertex].m_Vertex = *VertexList[Vert - 1]; 
               pVertices[CurrentVertex].m_Normal = *NormalList[Norm - 1];
               pVertices[CurrentVertex].m_TextureCoord = *TexCoordList[Tex - 1];   
         
               ppIndices[CurrentGroup][CurrentIndex] = CurrentVertex;

               CurrentVertex++;               
            }
            else
            {
               ppIndices[CurrentGroup][CurrentIndex] = j;         
            }
         
            CurrentIndex++;              
         }
      }
      else if (!strcmp("t", Token))
      {
         fscanf(pFile, " %f %f %f", &(pTranslations[CurrentGroup].x),
                &(pTranslations[CurrentGroup].y), &(pTranslations[CurrentGroup].z));
         pTranslations[CurrentGroup].x *= XScale;
         pTranslations[CurrentGroup].y *= YScale;
         pTranslations[CurrentGroup].z *= ZScale;
         
      }
   } 

   
   fclose(pFile);

   // START SAVING OUT TO PTG HERE
   pFile = fopen(pPTGFilePath, "wb");
   
   // write out the PTG file header
   unsigned int FileHeader = PTG_FILE_HEADER;
   fwrite(&FileHeader, sizeof(FileHeader), 1, pFile);

   // write out the vertices
   fwrite(&NumVertices, sizeof(NumVertices), 1, pFile);
   fwrite(pVertices, sizeof(*pVertices) * NumVertices, 1, pFile);

   // write out indices and translations
   unsigned int NumGroups = (unsigned int)NumIndicesList.size();
   fwrite(&NumGroups, sizeof(NumGroups), 1, pFile);
   for (i = 0; i < NumGroups; i++)
   {      
      fwrite(&(NumIndicesList[i]), sizeof(unsigned int), 1, pFile);
      fwrite(ppIndices[i], sizeof(unsigned int) * NumIndicesList[i], 1, pFile);
      fwrite(&(pTranslations[i]), sizeof(pTranslations[i]), 1, pFile);
   }

   // write out bounding sphere
   fwrite(&BoundingSphere, sizeof(BoundingSphere), 1, pFile);

   // write out half lengths bounding box
   fwrite(&Min, sizeof(Min), 1, pFile);
   fwrite(&Max, sizeof(Max), 1, pFile);

   // write out texture file path
   fwrite(pTextureFilePath, PT_FILE_PATH_LEN, 1, pFile); 
   
   fclose(pFile);
   // END PTG WRITING

   // clear out the vertex list
   delete [] pTranslations;   
   
   for (i = 0; i < VertexList.size(); i++)
   {
      delete VertexList[i];
   }

   VertexList.clear();

   for (i = 0; i < NormalList.size(); i++)
   {
      delete NormalList[i];
   }

   NormalList.clear();

   for (i = 0; i < TexCoordList.size(); i++)
   {
      delete TexCoordList[i];
   }

   TexCoordList.clear();
   

   for (i = 0; i < NumIndicesList.size(); i++)
   {
      delete [] ppIndices[i];
   }

   delete [] ppIndices;

   NumIndicesList.clear();

   delete [] pVertices;
}
