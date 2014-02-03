#pragma once

#include "math3d\math3d.h"
#include "graphics\interleavedarray.h"
#include "graphics\indexarray.h"

class CSkyBox
{
	private:
		unsigned int m_pTextures[6];
      float m_HalfSize;

	public:
      CSkyBox(const char *pBackImage, const char *pFrontImage, const char *pLeftImage,
              const char *pRightImage, const char *pTopImage, const char *pBottomImage,
              float Size);
		void Initialize(const char *pBackImage, const char *pFrontImage, const char *pLeftImage,
                      const char *pRightImage, const char *pTopImage, const char *pBottomImage,
                      float Size);
		void Render();		
};