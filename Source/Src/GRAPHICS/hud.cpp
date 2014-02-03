/// \file hud.h
/// The heads up display HUD
//////////////////////////////////////////////////////////////////////////

#include "graphics\hud.h"
#include "graphics\texturemanager.h"
#include "core\oglstatemanager.h"
#include "core\gamemain.h"
#include "core\oglwindow.h"
#include "graphics\bitmapfont.h"
#include "graphics\hudtext.h"
#include "gamelogic\player.h"
#include "gamelogic\gameoptions.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CTextureManager g_TextureManager;
extern CPlayer         g_Player;
extern COpenGLStateManager *pGLSTATE;
extern CGameMain       GameMain;
extern COpenGLWindow* pGLWIN;
extern CGameOptions g_GameOptions;
extern CBitmapFont g_Font;
extern CHudTextManager g_HudTextManager;

/////////////////////////////////
/*/ Global Variables          /*/ 
/////////////////////////////////

extern float g_fps;

//////////////////////////////////////////////////////////////////////////
/// Constructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CHud::CHud()
{
    g_pSmallFont = 0;
}

//////////////////////////////////////////////////////////////////////////
/// Destructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CHud::~CHud()
{
    Shutdown();
}

//////////////////////////////////////////////////////////////////////////
/// Initialize the Hud Class
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CHud::Initialize(void)
{
   FloatRect OrthoRect;
   OrthoRect.Max.x = 1.0f;
   OrthoRect.Max.y = 1.0f;
   OrthoRect.Min.x = -1.0f;
   OrthoRect.Min.y = -1.0f;
   DrawMothership = false;
   DrawConvoy = false;
   DrawBuilding = false;

   g_HudTextManager.Initialize();

   g_pSmallFont = new CBitmapFont;
   g_pSmallFont->Initialize("resources//images//sanserif256.tga", "resources//images/sanserif256charwidth.dat", 1024, 768, &OrthoRect);
   
    int VideoMode = g_GameOptions.GetVideoMode();
    unsigned int width = g_GameOptions.GetVideoWidth();
    unsigned int height = g_GameOptions.GetVideoHeight();
    
   switch(VideoMode)
   {
      case RES1600X1200:
      {
         Point3f Position17(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[14].Initialize("resources\\images\\hudimages\\currenttarget.tga", &OrthoRect, &Position17, width, height);

         Point3f Position15(0.70f, 0.95f, 0.0f);
         m_HudInterfaces[12].Initialize("resources\\images\\hudimages\\energybg.tga", &OrthoRect, &Position15, width,
                                    height);

         Point3f Position16(0.70f, 0.95f, 0.0f);
         m_HudInterfaces[13].Initialize("resources\\images\\hudimages\\energybar.tga", &OrthoRect, &Position16, width,
                                    height);

         Point3f Position(0.70f, 0.875f, 0.0f);
         m_HudInterfaces[0].Initialize("resources\\images\\hudimages\\shieldbg.tga", &OrthoRect, &Position, width,
                                    height);

         Point3f Position2(0.76f, 0.875f, 0.0f);
         m_HudInterfaces[1].Initialize("resources\\images\\hudimages\\shieldbar.tga", &OrthoRect, &Position2, width,
                                    height);

         Point3f Position3(0.74f, 0.80f, 0.0f);
         m_HudInterfaces[2].Initialize("resources\\images\\hudimages\\hullbg.tga", &OrthoRect, &Position3, width,
                                   height);

         Point3f Position4(0.80f, 0.80f, 0.0f);
         m_HudInterfaces[3].Initialize("resources\\images\\hudimages\\hullbar.tga", &OrthoRect, &Position4, width,
                                    height);

         Point3f Position5(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[4].Initialize("resources\\images\\hudimages\\missionfailure.tga", &OrthoRect, &Position5, width,
                                    height);

         Point3f Position6(0.0f, 0.94f, 0.0f);
         m_HudInterfaces[5].Initialize("resources\\images\\hudimages\\missileicon.tga", &OrthoRect, &Position6, width,
                                    height);

         Point3f Position7(0.11f, 0.94f, 0.0f);
         m_HudInterfaces[6].Initialize("resources\\images\\hudimages\\trkmissileicon.tga", &OrthoRect, &Position7, width,
                                    height);

         Point3f Position8(0.22f, 0.94f, 0.0f);
         m_HudInterfaces[7].Initialize("resources\\images\\hudimages\\ioncannonicon.tga", &OrthoRect, &Position8, width,
                                    height);

         Point3f Position9(0.0f, 0.94f, 0.0f);
         m_HudInterfaces[8].Initialize("resources\\images\\hudimages\\weaponselected.tga", &OrthoRect, &Position9, width,
                                    height);

         Point3f Position10(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[9].Initialize("resources\\images\\hudimages\\missionsuccess.tga", &OrthoRect, &Position10, width,
                                 height);

         Point3f Position11(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[10].Initialize("resources\\images\\hudimages\\ingamequitgame.tga", &OrthoRect, &Position11, width,
                                 height);

         Point3f Position12(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[11].Initialize("resources\\images\\hudimages\\testcrosshairsmall.tga", &OrthoRect, &Position11, width,
                                 height);

         Point3f Position13(-0.9f, 0.6f, 0.0f);
         m_HudInterfaces[15].Initialize("resources\\images\\hudimages\\mothershipicon.tga", &OrthoRect, &Position13, width, height);
      
         Point3f Position19(-0.9f, 0.40f, 0.0f);
         m_HudInterfaces[16].Initialize("resources\\images\\hudimages\\convoyicon.tga", &OrthoRect, &Position19, width, height);
     
         Point3f Position20(-0.9f, 0.40f, 0.0f);
         m_HudInterfaces[17].Initialize("resources\\images\\hudimages\\buildingicon.tga", &OrthoRect, &Position20, width, height);
      }
         break;

      case RES1024X768:
      {
         Point3f Position17(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[14].Initialize("resources\\images\\hudimages\\currenttarget1024.tga", &OrthoRect, &Position17, width, height);

         Point3f Position15(0.70f, 0.95f, 0.0f);
         m_HudInterfaces[12].Initialize("resources\\images\\hudimages\\energybg1024.tga", &OrthoRect, &Position15, width,
                                    height);

         Point3f Position16(0.70f, 0.95f, 0.0f);
         m_HudInterfaces[13].Initialize("resources\\images\\hudimages\\energybar1024.tga", &OrthoRect, &Position16, width,
                                    height);

         Point3f Position(0.70f, 0.875f, 0.0f);
         m_HudInterfaces[0].Initialize("resources\\images\\hudimages\\shieldbg1024.tga", &OrthoRect, &Position, width,
                                    height);

         Point3f Position2(0.76f, 0.875f, 0.0f);
         m_HudInterfaces[1].Initialize("resources\\images\\hudimages\\shieldbar1024.tga", &OrthoRect, &Position2, width,
                                    height);

         Point3f Position3(0.74f, 0.80f, 0.0f);
         m_HudInterfaces[2].Initialize("resources\\images\\hudimages\\hullbg1024.tga", &OrthoRect, &Position3, width,
                                   height);

         Point3f Position4(0.80f, 0.80f, 0.0f);
         m_HudInterfaces[3].Initialize("resources\\images\\hudimages\\hullbar1024.tga", &OrthoRect, &Position4, width,
                                    height);

         Point3f Position5(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[4].Initialize("resources\\images\\hudimages\\missionfailure1024.tga", &OrthoRect, &Position5, width,
                                    height);

         Point3f Position6(0.0f, 0.94f, 0.0f);
         m_HudInterfaces[5].Initialize("resources\\images\\hudimages\\missileicon1024.tga", &OrthoRect, &Position6, width,
                                    height);

         Point3f Position7(0.11f, 0.94f, 0.0f);
         m_HudInterfaces[6].Initialize("resources\\images\\hudimages\\trkmissileicon1024.tga", &OrthoRect, &Position7, width,
                                    height);

         Point3f Position8(0.22f, 0.94f, 0.0f);
         m_HudInterfaces[7].Initialize("resources\\images\\hudimages\\ioncannonicon1024.tga", &OrthoRect, &Position8, width,
                                    height);

         Point3f Position9(0.0f, 0.94f, 0.0f);
         m_HudInterfaces[8].Initialize("resources\\images\\hudimages\\weaponselected1024.tga", &OrthoRect, &Position9, width,
                                    height);

         Point3f Position10(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[9].Initialize("resources\\images\\hudimages\\missionsuccess1024.tga", &OrthoRect, &Position10, width,
                                 height);

         Point3f Position11(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[10].Initialize("resources\\images\\hudimages\\ingamequitgame1024.tga", &OrthoRect, &Position11, width,
                                 height);

         Point3f Position12(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[11].Initialize("resources\\images\\hudimages\\testcrosshairsmall1024.tga", &OrthoRect, &Position11, width,
                                 height);

         Point3f Position13(-0.9f, 0.6f, 0.0f);
         m_HudInterfaces[15].Initialize("resources\\images\\hudimages\\mothershipicon1024.tga", &OrthoRect, &Position13, width, height);
      
         Point3f Position19(-0.9f, 0.40f, 0.0f);
         m_HudInterfaces[16].Initialize("resources\\images\\hudimages\\convoyicon1024.tga", &OrthoRect, &Position19, width, height);
     
         Point3f Position20(-0.9f, 0.40f, 0.0f);
         m_HudInterfaces[17].Initialize("resources\\images\\hudimages\\buildingicon1024.tga", &OrthoRect, &Position20, width, height);
      }
         break;

      case RES800X600:
      {
         Point3f Position17(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[14].Initialize("resources\\images\\hudimages\\currenttarget800.tga", &OrthoRect, &Position17, width, height);

         Point3f Position15(0.70f, 0.95f, 0.0f);
         m_HudInterfaces[12].Initialize("resources\\images\\hudimages\\energybg800.tga", &OrthoRect, &Position15, width,
                                    height);

         Point3f Position16(0.70f, 0.95f, 0.0f);
         m_HudInterfaces[13].Initialize("resources\\images\\hudimages\\energybar800.tga", &OrthoRect, &Position16, width,
                                    height);

         Point3f Position(0.70f, 0.875f, 0.0f);
         m_HudInterfaces[0].Initialize("resources\\images\\hudimages\\shieldbg800.tga", &OrthoRect, &Position, width,
                                    height);

         Point3f Position2(0.76f, 0.875f, 0.0f);
         m_HudInterfaces[1].Initialize("resources\\images\\hudimages\\shieldbar800.tga", &OrthoRect, &Position2, width,
                                    height);

         Point3f Position3(0.74f, 0.80f, 0.0f);
         m_HudInterfaces[2].Initialize("resources\\images\\hudimages\\hullbg800.tga", &OrthoRect, &Position3, width,
                                   height);

         Point3f Position4(0.80f, 0.80f, 0.0f);
         m_HudInterfaces[3].Initialize("resources\\images\\hudimages\\hullbar800.tga", &OrthoRect, &Position4, width,
                                    height);

         Point3f Position5(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[4].Initialize("resources\\images\\hudimages\\missionfailure800.tga", &OrthoRect, &Position5, width,
                                    height);

         Point3f Position6(0.0f, 0.94f, 0.0f);
         m_HudInterfaces[5].Initialize("resources\\images\\hudimages\\missileicon800.tga", &OrthoRect, &Position6, width,
                                    height);

         Point3f Position7(0.11f, 0.94f, 0.0f);
         m_HudInterfaces[6].Initialize("resources\\images\\hudimages\\trkmissileicon800.tga", &OrthoRect, &Position7, width,
                                    height);

         Point3f Position8(0.22f, 0.94f, 0.0f);
         m_HudInterfaces[7].Initialize("resources\\images\\hudimages\\ioncannonicon800.tga", &OrthoRect, &Position8, width,
                                    height);

         Point3f Position9(0.0f, 0.94f, 0.0f);
         m_HudInterfaces[8].Initialize("resources\\images\\hudimages\\weaponselected800.tga", &OrthoRect, &Position9, width,
                                    height);

         Point3f Position10(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[9].Initialize("resources\\images\\hudimages\\missionsuccess800.tga", &OrthoRect, &Position10, width,
                                 height);

         Point3f Position11(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[10].Initialize("resources\\images\\hudimages\\ingamequitgame800.tga", &OrthoRect, &Position11, width,
                                 height);

         Point3f Position12(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[11].Initialize("resources\\images\\hudimages\\testcrosshairsmall800.tga", &OrthoRect, &Position11, width,
                                 height);

         Point3f Position13(-0.9f, 0.6f, 0.0f);
         m_HudInterfaces[15].Initialize("resources\\images\\hudimages\\mothershipicon800.tga", &OrthoRect, &Position13, width, height);
      
         Point3f Position19(-0.9f, 0.40f, 0.0f);
         m_HudInterfaces[16].Initialize("resources\\images\\hudimages\\convoyicon800.tga", &OrthoRect, &Position19, width, height);
     
         Point3f Position20(-0.9f, 0.40f, 0.0f);
         m_HudInterfaces[17].Initialize("resources\\images\\hudimages\\buildingicon800.tga", &OrthoRect, &Position20, width, height);
      }
      break;

      case RES640X480:
      {
         Point3f Position17(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[14].Initialize("resources\\images\\hudimages\\currenttarget640.tga", &OrthoRect, &Position17, width, height);

         Point3f Position15(0.70f, 0.95f, 0.0f);
         m_HudInterfaces[12].Initialize("resources\\images\\hudimages\\energybg640.tga", &OrthoRect, &Position15, width,
                                    height);

         Point3f Position16(0.70f, 0.95f, 0.0f);
         m_HudInterfaces[13].Initialize("resources\\images\\hudimages\\energybar640.tga", &OrthoRect, &Position16, width,
                                    height);

         Point3f Position(0.70f, 0.875f, 0.0f);
         m_HudInterfaces[0].Initialize("resources\\images\\hudimages\\shieldbg640.tga", &OrthoRect, &Position, width,
                                    height);

         Point3f Position2(0.76f, 0.875f, 0.0f);
         m_HudInterfaces[1].Initialize("resources\\images\\hudimages\\shieldbar640.tga", &OrthoRect, &Position2, width,
                                    height);

         Point3f Position3(0.74f, 0.80f, 0.0f);
         m_HudInterfaces[2].Initialize("resources\\images\\hudimages\\hullbg640.tga", &OrthoRect, &Position3, width,
                                   height);

         Point3f Position4(0.80f, 0.80f, 0.0f);
         m_HudInterfaces[3].Initialize("resources\\images\\hudimages\\hullbar640.tga", &OrthoRect, &Position4, width,
                                    height);

         Point3f Position5(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[4].Initialize("resources\\images\\hudimages\\missionfailure640.tga", &OrthoRect, &Position5, width,
                                    height);

         Point3f Position6(0.0f, 0.94f, 0.0f);
         m_HudInterfaces[5].Initialize("resources\\images\\hudimages\\missileicon640.tga", &OrthoRect, &Position6, width,
                                    height);

         Point3f Position7(0.11f, 0.94f, 0.0f);
         m_HudInterfaces[6].Initialize("resources\\images\\hudimages\\trkmissileicon640.tga", &OrthoRect, &Position7, width,
                                    height);

         Point3f Position8(0.22f, 0.94f, 0.0f);
         m_HudInterfaces[7].Initialize("resources\\images\\hudimages\\ioncannonicon640.tga", &OrthoRect, &Position8, width,
                                    height);

         Point3f Position9(0.0f, 0.94f, 0.0f);
         m_HudInterfaces[8].Initialize("resources\\images\\hudimages\\weaponselected640.tga", &OrthoRect, &Position9, width,
                                    height);

         Point3f Position10(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[9].Initialize("resources\\images\\hudimages\\missionsuccess640.tga", &OrthoRect, &Position10, width,
                                 height);

         Point3f Position11(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[10].Initialize("resources\\images\\hudimages\\ingamequitgame640.tga", &OrthoRect, &Position11, width,
                                 height);

         Point3f Position12(0.0f, 0.0f, 0.0f);
         m_HudInterfaces[11].Initialize("resources\\images\\hudimages\\testcrosshairsmall640.tga", &OrthoRect, &Position11, width,
                                 height);

         Point3f Position13(-0.9f, 0.6f, 0.0f);
         m_HudInterfaces[15].Initialize("resources\\images\\hudimages\\mothershipicon640.tga", &OrthoRect, &Position13, width, height);
      
         Point3f Position19(-0.9f, 0.40f, 0.0f);
         m_HudInterfaces[16].Initialize("resources\\images\\hudimages\\convoyicon640.tga", &OrthoRect, &Position19, width, height);
     
         Point3f Position20(-0.9f, 0.40f, 0.0f);
         m_HudInterfaces[17].Initialize("resources\\images\\hudimages\\buildingicon640.tga", &OrthoRect, &Position20, width, height);
      }
         break;
   }
}

//////////////////////////////////////////////////////////////////////////
/// Render all the objects associated with the HUD
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CHud::RenderHud(void)
{
   // get the opengl matrices for screen projection
   double ModelMatrix[16];
   glGetDoublev(GL_MODELVIEW_MATRIX, ModelMatrix);

   double ProjectionMatrix[16];
   glGetDoublev(GL_PROJECTION_MATRIX, ProjectionMatrix);

   int Viewport[4];
   glGetIntegerv(GL_VIEWPORT, Viewport);
   
   
   //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   
   pGLSTATE->SetState(DEPTH_TEST, false);
   pGLSTATE->SetState(LIGHTING, false);  
   pGLSTATE->SetState(TEXTURE_2D, true);
   
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
      glLoadIdentity();
      gluOrtho2D(-1, 1, -1, 1);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity(); 
      
      //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
      pGLSTATE->SetState(BLEND, true);

      float percent = 100.0f;

      if(!GameMain.m_InSequence)
      {
         m_HudInterfaces[0].Render();
         percent = g_Player.GetShield() / g_Player.GetMaxShield();
         m_HudInterfaces[1].CropLeft(percent);
         m_HudInterfaces[1].Render();
         m_HudInterfaces[2].Render();
         percent = g_Player.GetHull() / g_Player.GetMaxHull();
         m_HudInterfaces[3].CropLeft(percent);
         m_HudInterfaces[3].Render();
         m_HudInterfaces[5].Render();
         m_HudInterfaces[6].Render();
         m_HudInterfaces[7].Render();
         m_HudInterfaces[12].Render();
         percent = g_Player.GetEnergy() / g_Player.GetMaxEnergy();
         m_HudInterfaces[13].CropLeft(percent);
         m_HudInterfaces[13].Render();
      }
      if(DrawMothership)
         m_HudInterfaces[15].Render();
      if(DrawConvoy)
         m_HudInterfaces[16].Render();
      if(DrawBuilding)
         m_HudInterfaces[17].Render();
      
      // draw the crosshair
      
      /*
      // find the player's shot ray
      CObjFrame ShotFrame;
      g_Player.FindShotPosition(&ShotFrame);
      Ray ShotRay;
      ShotFrame.GetForward(&ShotRay.Direction);
      ShotFrame.GetPosition(&ShotRay.Origin);
      float Magnitude = 100.0f;

      double AimX = ShotRay.Origin.x + ShotRay.Direction.x * Magnitude;
      double AimY = ShotRay.Origin.y + ShotRay.Direction.y * Magnitude;
      double AimZ = ShotRay.Origin.z + ShotRay.Direction.z * Magnitude;      

      // find the screen location of the crosshair
      double ScreenX;
      double ScreenY;
      double ScreenZ;
      gluProject(AimX, AimY, AimZ, ModelMatrix, ProjectionMatrix,
                 Viewport, &ScreenX, &ScreenY, &ScreenZ);

      // scale the screen coords based on the ortho view
      ScreenX = ScreenX * (2.0f / (float)pGLWIN->GetScreenWidth()) - 1.0f;
      ScreenY = (ScreenY - pGLWIN->GetScreenHeight()) * (2.0f / (float)pGLWIN->GetScreenHeight()) + 1.0f;
      
      // render the crosshair

      Point3f Position(ScreenX, ScreenY, 0.0f);
      m_HudInterfaces[11].m_Frame.SetPosition(&Position);
      */

      // draw the player's target
      // find the screen location of target
      if(!GameMain.m_InSequence)
      {
         if (g_Player.m_pTarget)
         {
         
            Point3f TargetPos;
            g_Player.m_pTarget->GetPosition(&TargetPos, 0);
            double ScreenX;
            double ScreenY;
            double ScreenZ;
            gluProject(TargetPos.x, TargetPos.y, TargetPos.z,
                       ModelMatrix, ProjectionMatrix, Viewport, &ScreenX, &ScreenY,
                       &ScreenZ);

            // scale the screen coords based on the ortho view
            ScreenX = ScreenX * (2.0f / (float)pGLWIN->GetScreenWidth()) - 1.0f;
            ScreenY = (ScreenY - pGLWIN->GetScreenHeight()) * (2.0f / (float)pGLWIN->GetScreenHeight()) + 1.0f;

            // render the crosshair
            Point3f Position((float)ScreenX, (float)ScreenY, 0.0f);
            m_HudInterfaces[14].m_Frame.SetPosition(&Position);
            m_HudInterfaces[14].Render();    
         }
      
         m_HudInterfaces[11].Render();
         switch(g_Player.m_CurSecWeapon)
         {
            case g_Player.SEC_MISSILE:
               {
                  CVector3f position(0.0f, 0.94f, 0.0f);
                  m_HudInterfaces[8].m_Frame.SetPosition(&position);
                  break;
               }
            case g_Player.SEC_TRK_MISSILE:
               {
                  CVector3f position(0.11f, 0.94f, 0.0f);
                  m_HudInterfaces[8].m_Frame.SetPosition(&position);
                  break;
               }
            case g_Player.SEC_IONCANNON:
               {
                  CVector3f position(0.22f, 0.94f, 0.0f);
                  m_HudInterfaces[8].m_Frame.SetPosition(&position);
                  break;
               }      
         }
         m_HudInterfaces[8].Render();

         if(GameMain.m_CurrentGameState == GameMain.QUIT_SCREEN)
         {
            m_HudInterfaces[10].Render();
         }
         else if(GameMain.m_CurrentGameState == GameMain.MISSION_WON)
         {
            m_HudInterfaces[9].Render();
         }
         else if(GameMain.m_CurrentGameState == GameMain.MISSION_LOST)
         {
            m_HudInterfaces[4].Render();
         }
          
         char buffer[20];
         CVector2f Pos;      
         Color4f Color = { 1.0f, 1.0f, 1.0f, 1.0f };
         g_pSmallFont->BeginRendering();
      
         Pos.x = 0.01f;
         Pos.y = .9375f;      
         itoa(g_Player.GetNumMissiles(), buffer, 10);
         g_pSmallFont->Print(buffer, &Pos, &Color);           

         Pos.x = 0.12f;
         Pos.y = .9375f;
         itoa(g_Player.GetNumTrkMissiles(), buffer, 10); 
         g_pSmallFont->Print(buffer, &Pos, &Color);
      
         int minutes = int((g_Player.m_IonCannonCountDown) / 60);
         int seconds = int(g_Player.m_IonCannonCountDown) % 60;

         /*
         Pos.x = -.9f;
         Pos.y = -.9f; 
         _gcvt(g_fps, 3, buffer);
         g_pSmallFont->Print(buffer, &Pos, &Color);
         */
      
         if (seconds == 0 && minutes == 0)
         {
            Color.r = 0.0f;
            Color.g = 0.0f;
            Color.b = 1.0f;
         }
         if (seconds > 9)
            sprintf(buffer, "%d:%d", minutes, seconds);
         else
            sprintf(buffer, "%d:0%d", minutes, seconds);
         Pos.x = .225f;
         Pos.y = .9375f;
         g_pSmallFont->Print(buffer, &Pos, &Color);

		 /*
		 itoa(GameMain.m_NumEnemiesAlive, buffer, 10);
		 Pos.x = .5f;
		 Pos.y = .3f;
		 g_pSmallFont->Print(buffer, &Pos, &Color);
		 */

         g_pSmallFont->EndRendering(); 
      }
      
      //_gcvt(FSOUND_GetCPUUsage(), 5, buffer); 
      //itoa(g_ParticleManager.SystemList.size(), buffer, 10);
      //DrawTextOnHud(1.0f, .65f, .05f, 20, buffer);
     
      //itoa(FSOUND_GetChannelsPlaying(), buffer, 10);
      //itoa(g_ParticleManager.RenderList.size(), buffer, 10);
      //DrawTextOnHud(1.0f, .55f, .05f, 20, buffer);
      
      //itoa(g_ParticleManager.m_TotalParticlesOnScreen, buffer, 10);
      //DrawTextOnHud(1.0f, .45f, .05f, 20, buffer);

      //itoa(g_ParticleManager.ParticlePool.size(), buffer, 10);
      //DrawTextOnHud(1.0f, .35f, .05f, 20, buffer);
     
      // itoa(GameMain.m_NumEnemiesAlive, buffer, 10);
      // DrawTextOnHud(-.45f, .55f, .05f, 20, buffer);
     // DrawTextOnHud(-0.30f, 0.0f, .05f, 20, _gcvt(g_Player.m_CurMainWeaponDamage, 3, buffer));

      g_HudTextManager.UpdateMothershipCom();
      g_HudTextManager.RenderMothershipCom();

      g_HudTextManager.UpdateConvoyCom();
      g_HudTextManager.RenderConvoyCom();

      g_HudTextManager.UpdateBuildingCom();
      g_HudTextManager.RenderBuildingCom();

      g_HudTextManager.UpdateHudMessage();
      g_HudTextManager.RenderHudMessage();


      pGLSTATE->SetState(BLEND, false);

      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
   //pGLSTATE->SetState(LIGHTING, true);
   pGLSTATE->SetState(DEPTH_TEST, true);
}

//////////////////////////////////////////////////////////////////////////
/// Draw the shield bar
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CHud::DrawShieldBar()
{
   /*
   float startxpos = 0.15f;
   float startypos = 0.90f;
   float hudwidth = 0.8f;
   float hudheight = 0.075f;
   */
   float startxpos = 0.34f;
   float startypos = 0.94f;
   float hudwidth = 0.64f;
   float hudheight = 0.05f;

   g_TextureManager.ActivateTexture(Textures[2]);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
   pGLSTATE->SetState(BLEND, true);					
   
   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos + hudwidth, startypos);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos + hudwidth, startypos + hudheight);

      glTexCoord2f(.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);        
   glEnd();

   g_TextureManager.ActivateTexture(Textures[3]);
   startxpos = 0.99f;	

   float barpercent = (float)g_Player.GetShield() / (float)g_Player.GetMaxShield();
   hudwidth = 0.5f * barpercent;

   glBegin(GL_QUADS);
      // Bottom Right
      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      // Top Right
      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);

      // Top Left
      glTexCoord2f(1.0f - barpercent, 1.0f);
      glVertex2f(startxpos - hudwidth, startypos + hudheight);    

      // Bottom Left
      glTexCoord2f(1.0f - barpercent, 0.0f);
      glVertex2f(startxpos - hudwidth, startypos);  
   glEnd();
}

//////////////////////////////////////////////////////////////////////////
/// Draw the hull bar
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CHud::DrawHullBar()
{
   /*
   float startxpos = 0.25f;
   float startypos = 0.80f;
   float hudwidth = 0.7f;
   float hudheight = 0.075f;
   */
   float startxpos = 0.44f;
   float startypos = 0.875f;
   float hudwidth = 0.54f;
   float hudheight = 0.05f;

   g_TextureManager.ActivateTexture(Textures[4]);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
   pGLSTATE->SetState(BLEND, true);						
   
   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos + hudwidth, startypos);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos + hudwidth, startypos + hudheight);

      glTexCoord2f(.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);        
   glEnd();

   g_TextureManager.ActivateTexture(Textures[5]);
   startxpos = 0.99f;	

   float barpercent = (float)g_Player.GetHull() / (float)g_Player.GetMaxHull();
   hudwidth = 0.4f * barpercent;

   glBegin(GL_QUADS);
      // Bottom Right
      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      // Top Right
      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);

      // Top Left
      glTexCoord2f(1.0f - barpercent, 1.0f);
      glVertex2f(startxpos - hudwidth, startypos + hudheight);    

      // Bottom Left
      glTexCoord2f(1.0f - barpercent, 0.0f);
      glVertex2f(startxpos - hudwidth, startypos);  
   glEnd();
}

void CHud::DrawMissileIcon(void)
{
   float startxpos = 0.80f;
   float startypos = 0.750f;
   float hudwidth = 0.180f;
   float hudheight = 0.080f;

   g_TextureManager.ActivateTexture(Textures[7]);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
   pGLSTATE->SetState(BLEND, true);						
   
   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos + hudwidth, startypos);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos + hudwidth, startypos + hudheight);

      glTexCoord2f(.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);        
   glEnd();   
}

void CHud::DrawTrkMissileIcon(void)
{
   float startxpos = 0.80f;
   float startypos = 0.650f;
   float hudwidth = 0.180f;
   float hudheight = 0.080f;

   g_TextureManager.ActivateTexture(Textures[8]);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
   pGLSTATE->SetState(BLEND, true);						
   
   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos + hudwidth, startypos);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos + hudwidth, startypos + hudheight);

      glTexCoord2f(.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);        
   glEnd();   
}

void CHud::DrawIonCannonIcon(void)
{
   float startxpos = 0.80f;
   float startypos = 0.550f;
   float hudwidth = 0.180f;
   float hudheight = 0.080f;

   g_TextureManager.ActivateTexture(Textures[6]);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
   pGLSTATE->SetState(BLEND, true);						
   
   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos + hudwidth, startypos);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos + hudwidth, startypos + hudheight);

      glTexCoord2f(.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);        
   glEnd();   
}

void CHud::DrawSelectionIcon(void)
{
   float startxpos = 0;
   float startypos = 0;
   float hudwidth = 0;
   float hudheight = 0;

   switch(g_Player.m_CurSecWeapon)
   {
      case g_Player.SEC_MISSILE:
         {
            startxpos = 0.80f;
            startypos = 0.750f;
            hudwidth = 0.180f;
            hudheight = 0.080f;
            break;
         }
      case g_Player.SEC_TRK_MISSILE:
         {
            startxpos = 0.80f;
            startypos = 0.650f;
            hudwidth = 0.180f;
            hudheight = 0.080f;
            break;
         }
      case g_Player.SEC_IONCANNON:
         {
            startxpos = 0.80f;
            startypos = 0.550f;
            hudwidth = 0.180f;
            hudheight = 0.080f;
            break;
         }      
   }

   g_TextureManager.ActivateTexture(Textures[9]);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
   pGLSTATE->SetState(BLEND, true);					

   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos + hudwidth, startypos);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos + hudwidth, startypos + hudheight);

      glTexCoord2f(.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);        
   glEnd();
}

void CHud::DrawQuitMenu(void)
{
   float startxpos = -.320f;
   float startypos = -.120f;
   float hudwidth = .640f;
   float hudheight = .240f;

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
   pGLSTATE->SetState(BLEND, true);	

   g_TextureManager.ActivateTexture(Textures[10]);			

   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos + hudwidth, startypos);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos + hudwidth, startypos + hudheight);

      glTexCoord2f(.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);        
   glEnd();
}

void CHud::DrawMissionSuccess(void)
{
   float startxpos = -.320f;
   float startypos = -.120f;
   float hudwidth = .640f;
   float hudheight = .240f;

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
   pGLSTATE->SetState(BLEND, true);	

   g_TextureManager.ActivateTexture(Textures[11]);			

   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos + hudwidth, startypos);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos + hudwidth, startypos + hudheight);

      glTexCoord2f(.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);        
   glEnd();
}

void CHud::DrawMissionFailure(void)
{
   float startxpos = -.320f;
   float startypos = -.120f;
   float hudwidth = .640f;
   float hudheight = .240f;

   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
   pGLSTATE->SetState(BLEND, true);	

   g_TextureManager.ActivateTexture(Textures[12]);			

   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2f(startxpos, startypos);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2f(startxpos + hudwidth, startypos);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2f(startxpos + hudwidth, startypos + hudheight);

      glTexCoord2f(.0f, 1.0f);
      glVertex2f(startxpos, startypos + hudheight);        
   glEnd();
}


//////////////////////////////////////////////////////////////////////////
/// Draw text upon the Hud
/// 
/// Input:     float x - x position to draw text
///            float y - y position to draw text
///            float scale - scale of the text
///            float rowsize - size of the row of text
///            char * text - the text to print
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

/*
void CHud::DrawTextOnHud(float x, float y, float scale, float rowsize, char * text)
{
   CVector2f vec;
   vec.x = x;
   vec.y = y;

   m_Text.SetScale(scale);
   m_Text.SetRowSize(rowsize);
   m_Text.SetLocation(vec);
   
   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(LIGHT1, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_ONE, GL_ONE);
   m_Text.PrintText(text); 
   pGLSTATE->SetState(BLEND, false);
   pGLSTATE->SetState(LIGHTING, true);
   pGLSTATE->SetState(LIGHT1, true);
}*/

//////////////////////////////////////////////////////////////////////////
/// Shutdown all memory allocated in CHud
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CHud::Shutdown(void)
{
    unsigned int NumHudInterfaces = 18;
    for (unsigned int i = 0; i < NumHudInterfaces; ++i)
    {
        m_HudInterfaces[i].Shutdown();
    }

    g_HudTextManager.Shutdown();
   delete g_pSmallFont;
   g_pSmallFont = 0;   
}






















