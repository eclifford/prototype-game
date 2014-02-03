#include "graphics\drawshape.h"
#include "graphics\glinclude.h"
#include <gl\GLU.h>

#include "utils\mmgr.h"

void DrawAABB(const AABB* pAABB)
{
   glBegin(GL_LINE_LOOP);

   // draw the Top first
   glVertex3f(pAABB->Max.x, pAABB->Max.y, pAABB->Max.z);
   glVertex3f(pAABB->Max.x, pAABB->Max.y, pAABB->Min.z);
   glVertex3f(pAABB->Min.x, pAABB->Max.y, pAABB->Min.z);
   glVertex3f(pAABB->Min.x, pAABB->Max.y, pAABB->Max.z);
   glEnd();

   glBegin(GL_LINE_LOOP);
   // draw the Bottom
   glVertex3f(pAABB->Max.x, pAABB->Min.y, pAABB->Max.z);
   glVertex3f(pAABB->Max.x, pAABB->Min.y, pAABB->Min.z);
   glVertex3f(pAABB->Min.x, pAABB->Min.y, pAABB->Min.z);
   glVertex3f(pAABB->Min.x, pAABB->Min.y, pAABB->Max.z);
   glEnd();

   // draw the vertical lines
   glBegin(GL_LINES);

   glVertex3f(pAABB->Max.x, pAABB->Min.y, pAABB->Max.z);
   glVertex3f(pAABB->Max.x, pAABB->Max.y, pAABB->Max.z);

   glVertex3f(pAABB->Min.x, pAABB->Min.y, pAABB->Max.z);
   glVertex3f(pAABB->Min.x, pAABB->Max.y, pAABB->Max.z);

   glVertex3f(pAABB->Min.x, pAABB->Min.y, pAABB->Min.z);
   glVertex3f(pAABB->Min.x, pAABB->Max.y, pAABB->Min.z);

   glVertex3f(pAABB->Max.x, pAABB->Min.y, pAABB->Min.z);
   glVertex3f(pAABB->Max.x, pAABB->Max.y, pAABB->Min.z);

   glEnd();
}

void DrawTriangle(const Triangle* pTriangle, float NormalScale)
{
   // draw the triangle
   glBegin(GL_TRIANGLES);

      glVertex3f(pTriangle->Vertices[0].x, pTriangle->Vertices[0].y, pTriangle->Vertices[0].z);
      glVertex3f(pTriangle->Vertices[1].x, pTriangle->Vertices[1].y, pTriangle->Vertices[1].z);
      glVertex3f(pTriangle->Vertices[2].x, pTriangle->Vertices[2].y, pTriangle->Vertices[2].z);

   glEnd();

   // draw a point in a different color for each point
   glBegin(GL_POINTS);
      
      glColor3f(1.0f, .5f, .5f);
      glVertex3f(pTriangle->Vertices[0].x, pTriangle->Vertices[0].y, pTriangle->Vertices[0].z);
      
      glColor3f(.5f, .5f, 1.0f);
      glVertex3f(pTriangle->Vertices[1].x, pTriangle->Vertices[1].y, pTriangle->Vertices[1].z);
      
      glColor3f(.5f, 1.0f, .5f);
      glVertex3f(pTriangle->Vertices[2].x, pTriangle->Vertices[2].y, pTriangle->Vertices[2].z);


      
   glEnd();
   
   // find the center of the triangle to get the origin of the normal ray
   Point3f Origin;
   Vec3fAdd(&Origin, &pTriangle->Vertices[0], &pTriangle->Vertices[1]);
   Vec3fAdd(&Origin, &Origin, &pTriangle->Vertices[2]);
   Vec3fScale(&Origin, &Origin, 1.0f/3.0f);

   // add the normal to the origin to get the end point of the normal ray
   CVector3f Normal = pTriangle->Normal;
   Vec3fScale(&Normal, &Normal, NormalScale);
   Vec3fAdd(&Normal, &Origin, &Normal); 
   
   // draw points of the normal
   glBegin(GL_POINTS);

   glColor3f(1.0f, 0.0f, 1.0f);
   glVertex3f(Origin.x, Origin.y, Origin.z);

   glColor3f(.5f, 0.0f, .5f);
   glVertex3f(Normal.x, Normal.y, Normal.z);


   glEnd();

   // draw the normal
   glBegin(GL_LINES);
   
      glVertex3f(Origin.x, Origin.y, Origin.z);
      glVertex3f(Normal.x, Normal.y, Normal.z);

   glEnd();
}

void DrawPoint(const Point3f* pPoint)
{
   glBegin(GL_POINTS);

   glVertex3f(pPoint->x, pPoint->y, pPoint->z);

   glEnd();
}

void DrawOBB(const OBB* pOBB)
{
   Point3f TopUpLeft;
   Point3f TopLowLeft;
   Point3f TopLowRight;
   Point3f TopUpRight;

   Point3f BottomUpLeft;
   Point3f BottomLowLeft;
   Point3f BottomLowRight;
   Point3f BottomUpRight;
   
   // the Top

   // Upper Left
   TopUpLeft.x = -pOBB->x;
   TopUpLeft.y = pOBB->y;
   TopUpLeft.z = pOBB->z;
   
   // Lower Left
   TopLowLeft.x = -pOBB->x;
   TopLowLeft.y = pOBB->y;
   TopLowLeft.z = -pOBB->z;

   // Lower Right
   TopLowRight.x = pOBB->x;
   TopLowRight.y = pOBB->y;
   TopLowRight.z = -pOBB->z;

   // Upper Right
   TopUpRight.x = pOBB->x;
   TopUpRight.y = pOBB->y;
   TopUpRight.z = pOBB->z;

   // the Bottom
   
   // Upper Left
   BottomUpLeft.x = -pOBB->x;
   BottomUpLeft.y = -pOBB->y;
   BottomUpLeft.z = pOBB->z;
   
   // Lower Left
   BottomLowLeft.x = -pOBB->x;
   BottomLowLeft.y = -pOBB->y;
   BottomLowLeft.z = -pOBB->z;

   // Lower Right
   BottomLowRight.x = pOBB->x;
   BottomLowRight.y = -pOBB->y;
   BottomLowRight.z = -pOBB->z;

   // Upper Right
   BottomUpRight.x = pOBB->x;
   BottomUpRight.y = -pOBB->y;
   BottomUpRight.z = pOBB->z;

   /*
   pOBB->Frame->RotateToWorldPoint(&TopUpLeft, &TopUpLeft);
   pOBB->Frame->RotateToWorldPoint(&TopLowLeft, &TopLowLeft);
   pOBB->Frame->RotateToWorldPoint(&TopLowRight, &TopLowRight);
   pOBB->Frame->RotateToWorldPoint(&TopUpRight, &TopUpRight);

   pOBB->Frame->RotateToWorldPoint(&BottomUpLeft, &BottomUpLeft);
   pOBB->Frame->RotateToWorldPoint(&BottomLowLeft, &BottomLowLeft);
   pOBB->Frame->RotateToWorldPoint(&BottomLowRight, &BottomLowRight);
   pOBB->Frame->RotateToWorldPoint(&BottomUpRight, &BottomUpRight);
   
   // draw the points
   DrawPoint(&TopUpLeft);
   DrawPoint(&TopLowLeft);
   DrawPoint(&TopLowRight);
   DrawPoint(&TopUpRight);

   DrawPoint(&BottomUpLeft);
   DrawPoint(&BottomLowLeft);
   DrawPoint(&BottomLowRight);
   DrawPoint(&BottomUpRight);
   */

   float Matrix[16];
   pOBB->Frame->GetOpenGLMatrix(Matrix);
   glPushMatrix();
   glMultMatrixf(Matrix);
   
   // draw out the planes 
   glBegin(GL_QUADS);

   // draw the Top quad
   glVertex3f(TopLowLeft.x, TopLowLeft.y, TopLowLeft.z);
   glVertex3f(TopUpLeft.x, TopUpLeft.y, TopUpLeft.z);
   glVertex3f(TopUpRight.x, TopUpRight.y, TopUpRight.z);
   glVertex3f(TopLowRight.x, TopLowRight.y, TopLowRight.z);

   // draw the Bottom quad
   glVertex3f(BottomUpLeft.x, BottomUpLeft.y, BottomUpLeft.z);
   glVertex3f(BottomLowLeft.x, BottomLowLeft.y, BottomLowLeft.z);
   glVertex3f(BottomLowRight.x, BottomLowRight.y, BottomLowRight.z);
   glVertex3f(BottomUpRight.x, BottomUpRight.y, BottomUpRight.z);

   // draw the right quad
   glVertex3f(TopLowRight.x, TopLowRight.y, TopLowRight.z);
   glVertex3f(TopUpRight.x, TopUpRight.y, TopUpRight.z);
   glVertex3f(BottomUpRight.x, BottomUpRight.y, BottomUpRight.z);
   glVertex3f(BottomLowRight.x, BottomLowRight.y, BottomLowRight.z);

   // draw the left quad
   glVertex3f(TopUpLeft.x, TopUpLeft.y, TopUpLeft.z);
   glVertex3f(TopLowLeft.x, TopLowLeft.y, TopLowLeft.z);
   glVertex3f(BottomLowLeft.x, BottomLowLeft.y, BottomLowLeft.z);
   glVertex3f(BottomUpLeft.x, BottomUpLeft.y, BottomUpLeft.z);
 
   // draw the front quad
   glVertex3f(TopLowLeft.x, TopLowLeft.y, TopLowLeft.z);
   glVertex3f(TopLowRight.x, TopLowRight.y, TopLowRight.z);
   glVertex3f(BottomLowRight.x, BottomLowRight.y, BottomLowRight.z);
   glVertex3f(BottomLowLeft.x, BottomLowLeft.y, BottomLowLeft.z);

   // draw the back quad
   glVertex3f(TopUpRight.x, TopUpRight.y, TopUpRight.z);
   glVertex3f(TopUpLeft.x, TopUpLeft.y, TopUpLeft.z);
   glVertex3f(BottomUpLeft.x, BottomUpLeft.y, BottomUpLeft.z);
   glVertex3f(BottomUpRight.x, BottomUpRight.y, BottomUpRight.z);

   glEnd();

   glPopMatrix();
}

void DrawRay(const Ray* pRay, float Magnitude)
{     
   glBegin(GL_LINES);
      glVertex3f(pRay->Origin.x, pRay->Origin.y, pRay->Origin.z);
      glVertex3f(pRay->Origin.x + pRay->Direction.x * Magnitude,
                 pRay->Origin.y + pRay->Direction.y * Magnitude,
                 pRay->Origin.z + pRay->Direction.z * Magnitude);
   glEnd();
}

void DrawSphere(const Sphere* pSphere, unsigned int Slices, unsigned int Stacks)
{
   glPushMatrix();
   
      glTranslatef(pSphere->Center.x, pSphere->Center.y, pSphere->Center.z);
      GLUquadricObj* pQuadricSphere = gluNewQuadric();
      gluSphere(pQuadricSphere, pSphere->Radius, Slices, Stacks);

   glPopMatrix();

   gluDeleteQuadric(pQuadricSphere);
}