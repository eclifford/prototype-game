#pragma once

#include "graphics\color4f.h"
#include "math3d\math3d.h"
#include "math3d\primitives.h"

class CLight
{
   public:
      virtual void Initialize(int GLLightIndex, const Color4f* pAmbient,
                                     const Color4f* pDiffuse, const Color4f* pSpecular);
      virtual void InitializeDefaults(int GLLightIndex);
      void Enable();
      void Disable();
      virtual void Update() const = 0;
      void SetColors(const Color4f* pAmbient, const Color4f* pDiffuse,
                     const Color4f* pSpecular);
      
      
   protected:
      int m_GLLightIndex;
      Color4f m_Ambient;
      Color4f m_Diffuse;
      Color4f m_Specular;
      bool m_Enabled;      
};

class CDirectionalLight : public CLight
{
   public:
      void Initialize(int GLLightIndex, const Color4f* pAmbient,
                      const Color4f* pDiffuse, const Color4f* pSpecular,
                      const CVector3f* pDirection);
      void InitializeDefaults(int GLLightIndex);
      void Update() const;
   private:
      CVector3f m_Direction;         
};

class CPositionalLight : public CLight
{
   public:      
      void Initialize(int GLLightIndex, const Color4f* pAmbient,
                      const Color4f* pDiffuse, const Color4f* pSpecular,
                      const Point3f* pPosition, float ConstantAttenuation,
                      float LinearAttenuation, float QuadraticAttenuation);   
      void InitializeDefaults(int GLLightIndex);   
      void Update() const;
      inline void SetPosition(const Point3f* pPosition)
      {
         m_Position = *pPosition;
      }
   private:
      Point3f m_Position;      
      float m_ConstantAttenuation;
      float m_LinearAttenuation;
      float m_QuadraticAttenuation;      
};

class CSpotlight : public CLight
{
   public:
      void Initialize(int GLLightIndex, const Color4f* pAmbient,
                      const Color4f* pDiffuse, const Color4f* pSpecular,
                      const CObjFrame* pFrame, float ConstantAttenuation,
                      float LinearAttenuation, float QuadraticAttenuation, float Cutoff,
                      float Exponent);
      void InitializeDefaults(int GLLightIndex);
      void Update() const;

      CObjFrame m_Frame;

   private:
      float m_ConstantAttenuation;
      float m_LinearAttenuation;
      float m_QuadraticAttenuation;      
      float m_Cutoff;
      float m_Exponent;
};