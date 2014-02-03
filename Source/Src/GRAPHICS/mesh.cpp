#include "graphics/mesh.h"

#include "utils\mmgr.h"

void CMesh::Render()
{
   m_Material.Activate();
  
   m_IndexArray.Render();
}

void CMesh::Initialize(unsigned int* pIndices, unsigned int NumIndices, const int PrimitiveType,
                       CMaterial* pMaterial)
{
   m_IndexArray.Initialize(pIndices, NumIndices, PrimitiveType);
   m_Material = *pMaterial;
}

/*
void CMesh::SaveToFile(FILE *pFile)
{
   m_IndexArray.SaveToFile(pFile);
   m_pMaterial->SaveToFile(pFile);  
}*/