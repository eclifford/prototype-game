#pragma once

#include "math3d\primitives.h"

void DrawAABB(const AABB* pAABB);

void DrawTriangle(const Triangle* pTriangle, float NormalScale);

void DrawOBB(const OBB* pOBB);

void DrawRay(const Ray* pRay, float Magnitude);

void DrawSphere(const Sphere* pSphere, unsigned int Slices, unsigned int Stacks);

void DrawPoint(const Point3f* pPoint);