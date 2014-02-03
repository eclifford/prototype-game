#include "graphics/heightmap.h"
#include <cstdio>
#include <io.h>
#include <cmath>

#include "utils\mmgr.h"

void CHeightMap::Initialize(const char* pFilePath)
{
   FILE *pFile = fopen(pFilePath, "rb");

   const unsigned int Size = _filelength(_fileno(pFile));

	// allocate memory for the map
	m_pHeights = new unsigned char[Size];
	
	m_Size = (unsigned int)sqrt((float)Size);

	fread(m_pHeights, Size, 1, pFile);

   fclose(pFile);
}

CHeightMap::~CHeightMap()
{
	delete [] m_pHeights;
}

void CFloatHeightMap::Initialize(const float* pHeights, unsigned int Size)
{
   m_pHeights = new float[Size * Size];

   memcpy(m_pHeights, pHeights, Size * sizeof(float));

   m_Size = Size;
}

CFloatHeightMap::~CFloatHeightMap()
{
   delete [] m_pHeights;
}