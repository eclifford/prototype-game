#pragma once
#include "math3d\objframe.h"

class CRenderObj
{
   public:
      CRenderObj() : m_pNext(0) { }
      virtual void Render() = 0;
      virtual float GetDist() { return m_Dist; }
      virtual void CalcDist() { }
      virtual unsigned int GetGeometryIndex() { return 0; }
      virtual int GetTextureIndex() { return 0; }
      virtual bool IsTransparent() { return false; }
      virtual bool HasLighting() { return true; }
      
      CRenderObj* m_pNext;
   
   protected:
      float m_Dist;      
};

// used for interface
class CFrameRenderObj : public CRenderObj
{
   public:
      void Initialize(unsigned int GeometryIndex, unsigned int TextureIndex, float XScale,
                      float YScale, const CVector3f* pPosition)
      {         
         m_GeometryIndex = GeometryIndex;
         m_TextureIndex = TextureIndex;
         
      }
      void Render();
      float GetDist();
      unsigned int GetGeometryIndex() { return m_GeometryIndex; }
      int GetTextureIndex() { return m_TextureIndex; }
      bool IsTransparent() { return false; }
      void MoveForward(float Dist);
	   void MoveRight(float Dist);
	   void MoveUp(float Dist);
      void RotateLocalX(float Angle);
      void RotateLocalY(float Angle);
      void RotateLocalZ(float Angle);      

   protected:
      CObjFrame m_Frame;
      unsigned int m_GeometryIndex;
      unsigned int m_TextureIndex;
};