#include <math.h>

#include "level editor/patchlist.h"
#include "graphics/geometrymanager.h"
#include "graphics/texturemanager.h"
#include "graphics\terrain.h"

#include "utils\mmgr.h"

extern CGeometryManager g_GeometryManager;
extern CTextureManager g_TextureManager;

float CPatchList::GetDistanceSquared(float x1, float z1, float x2, float z2)
{
   return powf(x2 - x1, 2) + powf(z2 - z1, 2);
}

bool CPatchList::RayCollisionPoint(const Ray* pRay, Point3f* pPoint)
{
   Node* pCurr = m_pBack;

   while (pCurr)
   {
      if (pCurr->m_pPatch->RayCollisionPoint(pRay, pPoint))
         return true;

      pCurr = pCurr->m_pNext;
   }

   return false;   
}

void CPatchList::Clear()
{
   Node* pCurr = m_pBack;
   Node* pTemp;

   while (pCurr)
   {
      pTemp = pCurr->m_pNext;
      delete pCurr;
      pCurr = pTemp;
   }

   m_pBack = 0;
}

void CPatchList::AddPatch(CPatch* pPatch)
{
   if (!m_pBack)
   {
      m_pBack = new Node;
      m_pBack->m_pNext = 0;
      m_pBack->m_pPatch = pPatch;
      m_pBack->m_DistanceSquared = GetDistanceSquared(m_pCameraPos->x, m_pCameraPos->z,
                                                      pPatch->GetSphere()->Center.x,
                                                      pPatch->GetSphere()->Center.z);
   }
   else
   {     
      float DistanceSquared = GetDistanceSquared(m_pCameraPos->x, m_pCameraPos->z,
                                                      pPatch->GetSphere()->Center.x,
                                                      pPatch->GetSphere()->Center.z);

      // check if its better than the root
      if (DistanceSquared <= m_pBack->m_DistanceSquared)
      {
         Node* NewNode = new Node;
         NewNode->m_pNext = m_pBack;
         NewNode->m_pPatch = pPatch;
         NewNode->m_DistanceSquared = DistanceSquared;
         m_pBack = NewNode;
      }
      else
      {
         Node* pCurr = m_pBack;

         while (pCurr->m_pNext)
         {         
            if (DistanceSquared <= pCurr->m_pNext->m_DistanceSquared)
            {
               Node* NewNode = new Node;
               NewNode->m_pNext = pCurr->m_pNext;
               pCurr->m_pNext = NewNode;
               NewNode->m_pPatch = pPatch;
               NewNode->m_DistanceSquared = DistanceSquared;
               return;
            }

            pCurr = pCurr->m_pNext;
         }


         // we got to the end
         pCurr->m_pNext = new Node;
         pCurr->m_pNext->m_pNext = 0;
         pCurr->m_pNext->m_pPatch = pPatch;
         pCurr->m_pNext->m_DistanceSquared = DistanceSquared;
      }
   }            
}