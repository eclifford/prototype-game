#include "math3d\geomath.h"

#include "utils\mmgr.h"

void ComputeNormal(const CVector3f* p1, const CVector3f* p2, const CVector3f* p3,
                   CVector3f* pNormal)
{
   CVector3f vec1;
   CVector3f vec2;

   Vec3fSubtract(&vec1, p2, p1);
   Vec3fSubtract(&vec2, p3, p1);

   Vec3fCrossProduct(pNormal, &vec1, &vec2);

   Vec3fNormalize(pNormal, pNormal);
}