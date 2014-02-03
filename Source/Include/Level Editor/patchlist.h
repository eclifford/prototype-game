#pragma once

#include "math3d\primitives.h"

// forward declarations
class CPatch;

class CPatchList
{
   public:
      void Clear();
      CPatchList() : m_pBack(0) { }
      void AddPatch(CPatch* pPatch);
      void SetCameraPos(CVector3f* pCameraPos) { m_pCameraPos = pCameraPos; }
      bool RayCollisionPoint(const Ray* pRay, Point3f* pPoint);
      bool IsEmpty() { return !m_pBack; }

   private:       
      
      struct Node
      {
         Node* m_pNext;
         CPatch* m_pPatch;
         float m_DistanceSquared;
      };

      Node* m_pBack;
      CVector3f *m_pCameraPos;
      CVector3f m_Temp;

      float GetDistanceSquared(float x1, float z1, float x2, float z2);
};