// File: "minimap.h"
// Author(s): Jesse Edmunds

#pragma once

#include "graphics\terrain.h"
#include "math3d\objframe.h"
#include "graphics\texturefonts.h"

// 3D mini-map to show the terrain and objects in the game
class CMiniMap
{
   public:

      // initialize the minimap
      void Initialize(float Radius);

      // render the minimap
      void Render();
   private:

      // the sphere of visibility
      Sphere m_VisibilitySphere;

      // the transformation frame used to display the minimap
      CObjFrame m_Frame;

      // how far to scale everything down
      float m_Scale;
};