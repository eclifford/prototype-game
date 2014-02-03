/// \file hud.h
/// The heads up display HUD
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "graphics/texturefonts.h"
#include "graphics\interfacerenderable.h"
#include "graphics/bitmapfont.h"

#define HUD_TEXTURES 20

class CHud
{
	private:
      CInterfaceRenderable m_HudInterfaces[18];

   protected:

	public:
	   CHud();
		~CHud();	
      FloatRect OrthoRect;
      
      unsigned int Textures[HUD_TEXTURES];

      bool DrawMothership;
      bool DrawConvoy;
      bool DrawBuilding;
      CBitmapFont * g_pSmallFont;

      void Initialize(void);
      void RenderHud(void);
      void Shutdown(void);
      void DrawHullBar();
      void DrawShieldBar();
      void DrawTextOnHud(float x, float y, float scale, float rowsize, char * text);
      void DrawIonCannonIcon(void);
      void DrawMissileIcon(void);
      void DrawTrkMissileIcon(void);
      void DrawSelectionIcon(void);
      void DrawQuitMenu(void);
      void DrawMissionSuccess(void);
      void DrawMissionFailure(void);
};