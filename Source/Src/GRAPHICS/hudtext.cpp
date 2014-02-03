#include "graphics/hudtext.h"
#include "graphics/bitmapfont.h"
#include "graphics/hud.h"
#include "gamelogic/gameoptions.h"

#include "utils\mmgr.h"

extern CHud g_Hud;
extern float g_FrameTime;
extern CGameOptions g_GameOptions;

CHudTextManager::CHudTextManager()
{

}

CHudTextManager::~CHudTextManager()
{
    Shutdown();   
}

void CHudTextManager::Shutdown()
{
    // free all memory
    if (!MotherShipCommunicationList.empty())
    {
        for (iterator = MotherShipCommunicationList.begin(); iterator != MotherShipCommunicationList.end(); ++iterator)
        {
            delete *iterator;
        }
        MotherShipCommunicationList.clear();
    }
    
    if (!ConvoyCommunicationList.empty())
    {
        for (iterator = ConvoyCommunicationList.begin(); iterator != ConvoyCommunicationList.end(); ++iterator)
        {
            delete *iterator;
        }
        ConvoyCommunicationList.clear();
    }
    
    if (!BuildingCommunicationList.empty())
    {
        for (iterator = BuildingCommunicationList.begin(); iterator != BuildingCommunicationList.end(); ++iterator)
        {
            delete *iterator;
        }
        BuildingCommunicationList.clear();
    }
    
    if (!MessagesList.empty())
    {
        for (iterator = MessagesList.begin(); iterator != MessagesList.end(); ++iterator)
        {
            delete *iterator;
        }
        MessagesList.clear();   
    }
    
    delete g_pFont;
    g_pFont = 0;
}

void CHudTextManager::Initialize()
{
   FloatRect OrthoRect;
   OrthoRect.Max.x = 1.0f;
   OrthoRect.Min.x = -1.0f;
   OrthoRect.Max.y = 1.0f;
   OrthoRect.Min.y = -1.0f;
   
   g_pFont = new CBitmapFont;
   g_pFont->Initialize("resources//images//sanserif256.tga", "resources//images/sanserif256charwidth.dat", 1024, 768, &OrthoRect);
   
   /*switch(g_GameOptions.GetVideoMode())
   {
      case RES1600X1200:
         {
            g_Font.Initialize("resources//images//sanserif.tga", 32, 32, &OrthoRect);
         }
         break;
      case RES1024X768:
         {
            g_Font.Initialize("resources//images//sanserif1024.tga", 16, 16, &OrthoRect);
         }
         break;
      case RES800X600:
         {
            g_Font.Initialize("resources//images//sanserif800.tga", 8, 8, &OrthoRect);
         }
         break;
     case RES640X480:
         {
            g_Font.Initialize("resources//images//sanserif640.tga", 4, 4, &OrthoRect);
         }
         break;

               void Initialize(const char* pTextureFilePath, const char* pCharWidthsFilePath,
                      unsigned int DefaultWidth, unsigned int DefaultHeight,
                      const FloatRect* pOrthoRect);
   }
   */
}

void CHudTextManager::CreateHudMessage(char * text, CVector2f loc, Color4f color, float time)
{
   CHudText * hudtext = new CHudText();
   hudtext->m_Text = text;
   hudtext->m_Location = loc;
   hudtext->m_DeathAge = time;
   
   if(time == 0.0f)
      hudtext->m_AlphaStep = 0.0f;
   else
      hudtext->m_AlphaStep = color.a / hudtext->m_DeathAge;

   hudtext->m_Color = color;

   MessagesList.push_back(hudtext);
}

void CHudTextManager::RenderHudMessage()
{
   g_pFont->BeginRendering();

   iterator = MessagesList.begin();
   while(iterator != MessagesList.end())
   {
      g_pFont->Print((*iterator)->m_Text, &(*iterator)->m_Location, &(*iterator)->m_Color);
      ++iterator;
   }
  
   g_pFont->EndRendering();
}

void CHudTextManager::UpdateHudMessage()
{
   iterator = MessagesList.begin();
   while(iterator != MessagesList.end())
   {
      (*iterator)->m_Color.a -= ((*iterator)->m_AlphaStep * g_FrameTime);
      if((*iterator)->m_Color.a <= 0)
      {
         next = iterator;
         ++next;
         delete *iterator;
         MessagesList.erase(iterator);
         iterator = next;
      }
      else
         ++iterator;
   }
}

void CHudTextManager::CreateMothershipCom(char * text, CVector2f loc, Color4f color, float time)
{
   CHudText * hudtext = new CHudText();
   hudtext->m_Text = text;
   hudtext->m_Location = loc;
   hudtext->m_DeathAge = time;
   
   if(time == 0.0f)
      hudtext->m_AlphaStep = 0.0f;
   else
      hudtext->m_AlphaStep = color.a / hudtext->m_DeathAge;

   hudtext->m_Color = color;
   g_Hud.DrawMothership = true;

   MotherShipCommunicationList.push_back(hudtext);
}

void CHudTextManager::RenderMothershipCom()
{
   g_pFont->BeginRendering();

   iterator = MotherShipCommunicationList.begin();
   while(iterator != MotherShipCommunicationList.end())
   {
      g_pFont->Print((*iterator)->m_Text, &(*iterator)->m_Location, &(*iterator)->m_Color);
      ++iterator;
   }
  
   g_pFont->EndRendering();
}

void CHudTextManager::UpdateMothershipCom()
{
   iterator = MotherShipCommunicationList.begin();
   while(iterator != MotherShipCommunicationList.end())
   {
      (*iterator)->m_Color.a -= ((*iterator)->m_AlphaStep * g_FrameTime);
      if((*iterator)->m_Color.a <= 0)
      {
         next = iterator;
         ++next;
         delete *iterator;
         MotherShipCommunicationList.erase(iterator);
         iterator = next;
      }
      else
         ++iterator;
   }

   if(MotherShipCommunicationList.empty())
      g_Hud.DrawMothership = false;
}

void CHudTextManager::CreateConvoyCom(char * text, CVector2f loc, Color4f color, float time)
{
   CHudText * hudtext = new CHudText();
   hudtext->m_Text = text;
   hudtext->m_Location = loc;
   hudtext->m_DeathAge = time;
   
   if(time == 0.0f)
      hudtext->m_AlphaStep = 0.0f;
   else
      hudtext->m_AlphaStep = color.a / hudtext->m_DeathAge;

   hudtext->m_Color = color;
   g_Hud.DrawConvoy = true;

   ConvoyCommunicationList.push_back(hudtext);
}

void CHudTextManager::RenderConvoyCom()
{
   g_pFont->BeginRendering();

   iterator = ConvoyCommunicationList.begin();
   while(iterator != ConvoyCommunicationList.end())
   {
      g_pFont->Print((*iterator)->m_Text, &(*iterator)->m_Location, &(*iterator)->m_Color);
      ++iterator;
   }
  
   g_pFont->EndRendering();
}

void CHudTextManager::UpdateConvoyCom()
{
   iterator = ConvoyCommunicationList.begin();
   while(iterator != ConvoyCommunicationList.end())
   {
      (*iterator)->m_Color.a -= ((*iterator)->m_AlphaStep * g_FrameTime);
      if((*iterator)->m_Color.a <= 0)
      {
         next = iterator;
         ++next;
         delete *iterator;
         ConvoyCommunicationList.erase(iterator);
         iterator = next;
      }
      else
         ++iterator;
   }

   if(ConvoyCommunicationList.empty())
      g_Hud.DrawConvoy = false;
}


void CHudTextManager::CreateBuildingCom(char * text, CVector2f loc, Color4f color, float time)
{
   CHudText * hudtext = new CHudText();
   hudtext->m_Text = text;
   hudtext->m_Location = loc;
   hudtext->m_DeathAge = time;
   
   if(time == 0.0f)
      hudtext->m_AlphaStep = 0.0f;
   else
      hudtext->m_AlphaStep = color.a / hudtext->m_DeathAge;

   hudtext->m_Color = color;
   g_Hud.DrawBuilding = true;

   BuildingCommunicationList.push_back(hudtext);
}

void CHudTextManager::RenderBuildingCom()
{
   g_pFont->BeginRendering();

   iterator = BuildingCommunicationList.begin();
   while(iterator != BuildingCommunicationList.end())
   {
      g_pFont->Print((*iterator)->m_Text, &(*iterator)->m_Location, &(*iterator)->m_Color);
      ++iterator;
   }
  
   g_pFont->EndRendering();
}

void CHudTextManager::UpdateBuildingCom()
{
   iterator = BuildingCommunicationList.begin();
   while(iterator != BuildingCommunicationList.end())
   {
      (*iterator)->m_Color.a -= ((*iterator)->m_AlphaStep * g_FrameTime);
      if((*iterator)->m_Color.a <= 0)
      {
         next = iterator;
         ++next;
         delete *iterator;
         BuildingCommunicationList.erase(iterator);
         iterator = next;
      }
      else
         ++iterator;
   }

   if(BuildingCommunicationList.empty())
      g_Hud.DrawBuilding = false;
}

CHudText::CHudText()
{

}

CHudText::~CHudText()
{

}