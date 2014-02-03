#pragma once

#include "graphics\color3f.h"
#include "graphics\interleavedarray.h"
#include "graphics\indexarray.h"
#include "math3d\primitives.h"

extern float g_time;

class CFader
{
   public:
      void Initialize();
      inline void SetFadeTime(float Seconds)
      {
         m_FadeTime = Seconds;
      }
      inline void SetColor(const Color3F* pColor)
      {
         m_Color = *pColor;
      }
      void Render();
      inline void Start() { m_Enabled = true; m_StartTime = g_time; }
      inline void SetFadeDirection(bool FadeIn) { m_FadeIn = FadeIn; }
      inline void Stop() { m_Enabled = false; }
      inline bool IsDone()
      {
         return (m_StartTime + m_FadeTime < g_time);
      }
   private:
      enum EFaderCorner { FADER_LOWLEFT, FADER_LOWRIGHT, FADER_UPLEFT, FADER_UPRIGHT };


      float m_FadeTime;

      float m_StartTime;
      Color3F m_Color;

      bool m_FadeIn;

      bool m_Enabled;

      CInterleavedArray m_InterleavedArray;
      CIndexArray m_IndexArray;
};