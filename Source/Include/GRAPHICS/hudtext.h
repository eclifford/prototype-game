/// \file hudtext.h
/// Font displayed on the HUD
//////////////////////////////////////////////////////////////////////////

#include "math3d/math3d.h"
#include "graphics/color4f.h"
#include <list>
using namespace std;

// forward declarations
class CHudText;
class CBitmapFont;

class CHudTextManager
{
   public:
      CHudTextManager();
      ~CHudTextManager();

      void CreateMothershipCom(char * text, CVector2f loc, Color4f color, float time);
      void RenderMothershipCom();
      void UpdateMothershipCom();

      void CreateHudMessage(char * text, CVector2f loc, Color4f color, float time);
      void RenderHudMessage();
      void UpdateHudMessage();

      void CreateConvoyCom(char * text, CVector2f loc, Color4f color, float time);
      void RenderConvoyCom();
      void UpdateConvoyCom();

      void CreateBuildingCom(char * text, CVector2f loc, Color4f color, float time);
      void RenderBuildingCom();
      void UpdateBuildingCom();

      CBitmapFont * g_pFont;

      list<CHudText*> MotherShipCommunicationList;
      list<CHudText*> ConvoyCommunicationList;
      list<CHudText*> BuildingCommunicationList;
      list<CHudText*> MessagesList;
      list<CHudText*>::iterator iterator;
      list<CHudText*>::iterator next;
      void Initialize();
      void Shutdown();
};

class CHudText
{
   public:
      CHudText();
      ~CHudText();

      char * m_Text;
      CVector2f m_Location;
      Color4f m_Color;
      float m_DeathAge;
      float m_CreationAge;
      float m_AlphaStep;

   private:
   
};
