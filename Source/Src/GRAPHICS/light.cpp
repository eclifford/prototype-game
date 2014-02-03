#include "graphics\light.h"
#include "graphics\glinclude.h"

#include "utils\mmgr.h"

void CLight::Initialize(int GLLightIndex, const Color4f* pAmbient,
                        const Color4f* pDiffuse, const Color4f* pSpecular)
{
   m_GLLightIndex = GLLightIndex;

   m_Ambient = *pAmbient;
   m_Diffuse = *pDiffuse;
   m_Specular = *pSpecular;

   m_Enabled = false;

   glLightfv(m_GLLightIndex, GL_AMBIENT, (float*)&m_Ambient);
   glLightfv(m_GLLightIndex, GL_DIFFUSE, (float*)&m_Diffuse);
   glLightfv(m_GLLightIndex, GL_SPECULAR, (float*)&m_Specular);
}

void CLight::InitializeDefaults(int GLLightIndex)
{
   m_GLLightIndex = GLLightIndex;

   m_Ambient.r = 0.0f;
   m_Ambient.g = 0.0f;
   m_Ambient.b = 0.0f;
   m_Ambient.a = 1.0f;

   m_Diffuse.r = 1.0f;
   m_Diffuse.g = 1.0f;
   m_Diffuse.b = 1.0f;
   m_Diffuse.a = 1.0f;

   m_Specular.r = 1.0f;
   m_Specular.g = 1.0f;
   m_Specular.b = 1.0f;
   m_Specular.a = 1.0f;

   m_Enabled = false;

   glLightfv(m_GLLightIndex, GL_AMBIENT, (float*)&m_Ambient);
   glLightfv(m_GLLightIndex, GL_DIFFUSE, (float*)&m_Diffuse);
   glLightfv(m_GLLightIndex, GL_SPECULAR, (float*)&m_Specular);
}

void CLight::Enable()
{
   if (!m_Enabled)
   {
      glEnable(m_GLLightIndex);
      m_Enabled = true;
   }
}

void CLight::Disable()
{
   if (m_Enabled)
   {
      glDisable(m_GLLightIndex);
      m_Enabled = false;
   }
}

void CLight::SetColors(const Color4f* pAmbient, const Color4f* pDiffuse,
               const Color4f* pSpecular)
{
   m_Ambient = *pAmbient;
   m_Diffuse = *pDiffuse;
   m_Specular = *pSpecular;

   glLightfv(m_GLLightIndex, GL_AMBIENT, (float*)&m_Ambient);
   glLightfv(m_GLLightIndex, GL_DIFFUSE, (float*)&m_Diffuse);
   glLightfv(m_GLLightIndex, GL_SPECULAR, (float*)&m_Specular);
}

void CDirectionalLight::Initialize(int GLLightIndex, const Color4f* pAmbient,
                                   const Color4f* pDiffuse, const Color4f* pSpecular,
                                   const CVector3f* pDirection)
{
   CLight::Initialize(GLLightIndex, pAmbient, pDiffuse, pSpecular);

   m_Direction = *pDirection;
}

void CDirectionalLight::InitializeDefaults(int GLLightIndex)
{
   CLight::InitializeDefaults(GLLightIndex);
   
   m_Direction.x = 0.0f;
   m_Direction.y = 0.0f;
   m_Direction.z = 1.0f;   
}

void CDirectionalLight::Update() const
{
   if (!m_Enabled)
      return;

   float Direction[4] = { m_Direction.x, m_Direction.y, m_Direction.z, 0.0f };

   glLightfv(m_GLLightIndex, GL_POSITION, Direction);    
}

void CPositionalLight::Initialize(int GLLightIndex, const Color4f* pAmbient,
                                  const Color4f* pDiffuse, const Color4f* pSpecular,
                                  const CVector3f* pPosition, float ConstantAttenuation,
                                  float LinearAttenuation, float QuadraticAttenuation)
{
   CLight::Initialize(GLLightIndex, pAmbient, pDiffuse, pSpecular);      
   
   m_Position = *pPosition;

   m_ConstantAttenuation = ConstantAttenuation;
   m_LinearAttenuation = LinearAttenuation;
   m_QuadraticAttenuation = QuadraticAttenuation;
   
   glLightf(m_GLLightIndex, GL_CONSTANT_ATTENUATION, m_ConstantAttenuation);
   glLightf(m_GLLightIndex, GL_LINEAR_ATTENUATION, m_LinearAttenuation);
   glLightf(m_GLLightIndex, GL_QUADRATIC_ATTENUATION, m_QuadraticAttenuation);   
}

void CPositionalLight::InitializeDefaults(int GLLightIndex)
{
   CLight::InitializeDefaults(GLLightIndex);

   m_Position.x = 0.0f;
   m_Position.y = 0.0f;
   m_Position.z = 1.0f;

   m_ConstantAttenuation = 1.0f;
   m_LinearAttenuation = 0.0f;
   m_QuadraticAttenuation = 0.0f;

   glLightf(m_GLLightIndex, GL_CONSTANT_ATTENUATION, m_ConstantAttenuation);
   glLightf(m_GLLightIndex, GL_LINEAR_ATTENUATION, m_LinearAttenuation);
   glLightf(m_GLLightIndex, GL_QUADRATIC_ATTENUATION, m_QuadraticAttenuation);
}

void CPositionalLight::Update() const
{
   if (!m_Enabled)
      return;

   float Position[4] = { m_Position.x, m_Position.y, m_Position.z, 1.0f };

   glLightfv(m_GLLightIndex, GL_POSITION, Position);
}

void CSpotlight::Initialize(int GLLightIndex, const Color4f* pAmbient,
                            const Color4f* pDiffuse, const Color4f* pSpecular,
                            const CObjFrame* pFrame, float ConstantAttenuation,
                            float LinearAttenuation, float QuadraticAttenuation,
                            float Cutoff, float Exponent)
{
   CLight::Initialize(GLLightIndex, pAmbient, pDiffuse, pSpecular);      
   
   m_Frame = *pFrame;

   m_ConstantAttenuation = ConstantAttenuation;
   m_LinearAttenuation = LinearAttenuation;
   m_QuadraticAttenuation = QuadraticAttenuation;

   glLightf(m_GLLightIndex, GL_CONSTANT_ATTENUATION, m_ConstantAttenuation);
   glLightf(m_GLLightIndex, GL_LINEAR_ATTENUATION, m_LinearAttenuation);
   glLightf(m_GLLightIndex, GL_QUADRATIC_ATTENUATION, m_QuadraticAttenuation);
   
   m_Cutoff = Cutoff;
   
   m_Exponent = Exponent;
   
   glLightf(m_GLLightIndex, GL_SPOT_CUTOFF, m_Cutoff);
   glLightf(m_GLLightIndex, GL_SPOT_EXPONENT, m_Exponent);   
}

void CSpotlight::InitializeDefaults(int GLLightIndex)
{
   CLight::InitializeDefaults(GLLightIndex);

   m_ConstantAttenuation = 1.0f;
   m_LinearAttenuation = 0.0f;
   m_QuadraticAttenuation = 0.0f;

   glLightf(m_GLLightIndex, GL_CONSTANT_ATTENUATION, m_ConstantAttenuation);
   glLightf(m_GLLightIndex, GL_LINEAR_ATTENUATION, m_LinearAttenuation);
   glLightf(m_GLLightIndex, GL_QUADRATIC_ATTENUATION, m_QuadraticAttenuation);   

   m_Cutoff = 0.0f;
   m_Exponent = 180.0f;

   glLightf(m_GLLightIndex, GL_SPOT_CUTOFF, m_Cutoff);
   glLightf(m_GLLightIndex, GL_SPOT_EXPONENT, m_Exponent);
}

void CSpotlight::Update() const
{
   if (!m_Enabled)
      return;

   float Vector[4];
   
   m_Frame.GetPosition((Point3f*)&Vector);
   Vector[3] = 1.0f;
   glLightfv(m_GLLightIndex, GL_POSITION, Vector);

   m_Frame.GetForward((Point3f*)&Vector);
   glLightfv(m_GLLightIndex, GL_SPOT_DIRECTION, Vector);
}