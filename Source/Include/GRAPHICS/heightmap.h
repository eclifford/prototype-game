#pragma once

#include <windows.h>
#include <gl/GL.h>


class CHeightMap
{
	private:      
		unsigned char* m_pHeights;
		int m_Size;
	public:
      CHeightMap() { }
		~CHeightMap();
	   void Initialize(const char* pFilePath);
		inline int GetSize() const { return m_Size; }
		inline unsigned char GetHeight(const int XPosition, const int ZPosition) const
		{
			return m_pHeights[ZPosition * m_Size + XPosition];
		}
};

class CFloatHeightMap
{
   public:
      void Initialize(const float* pHeights, unsigned int Size);
      inline int GetSize() const { return m_Size; }
      inline float GetHeight(const unsigned int XPosition,
                             const unsigned int ZPosition) const
      {
         return m_pHeights[ZPosition * m_Size + XPosition];
      }
      ~CFloatHeightMap();
   private:
      float* m_pHeights;
      unsigned int m_Size;      
};