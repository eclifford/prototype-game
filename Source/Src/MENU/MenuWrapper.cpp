
#include "gamelogic\player.h"




#include "input\inputmanager.h"
#include "menu\irmanager.h"
#include "menu\MenuWrapper.h"
#include "menu\interfacemanager.h"
#include "core\menumain.h"
#include "core\gamemain.h"
#include "graphics\bitmapfont.h"
#include "gamelogic\gameoptions.h"
#include "sound\audiomanager.h"
#include "sound\audio.h"
#include "math3d\math3d.h"

#include "utils\mmgr.h"

extern CGameMain GameMain;
CIRManager g_MenuGraphics;
CGameOptions               g_GameOptions;
extern COpenGLStateManager* pGLSTATE;
extern COpenGLWindow		*pGLWIN;
extern CMenuMain MenuMain;
extern CPlayer g_Player;
extern CAudioManager g_AudioManager;

CMenuWrapper* CMenuWrapper::m_pInstance = 0;


CMenuWrapper::CMenuWrapper()
{
   g_GameOptions.InitSoundValues();
   g_GameOptions.SetDifficulty(1);
   m_WrapperState = MAIN_MENU;
   glAlphaFunc(GL_GREATER, 0);
   m_MousePos.x = 0.0f;
   m_MousePos.y = 0.0f;
   m_MousePos.z = 0.0f;
}


CMenuWrapper::~CMenuWrapper()
{
}



CMenuWrapper* CMenuWrapper::GetInstance()
{
   if (!m_pInstance)
      m_pInstance = new CMenuWrapper;

   return m_pInstance;
}


void CMenuWrapper::Init()
{
   m_ButtonClick = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_BUTTONCLK], 255);
   m_RadioClick = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_RADIOCLK], 255);
   m_ButtonSet = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_BUTTONSET], 255);

   FloatRect OrthoRect;
   OrthoRect.Max.x = 1.0f;
   OrthoRect.Min.x = -1.0f;
   OrthoRect.Max.y = 1.0f;
   OrthoRect.Min.y = -1.0f;

   g_AudioManager.SetAudioRatios(0.5f, 0.5f);

   g_pFont = new CBitmapFont;
   g_pFont->Initialize("resources//images//sanserif256.tga", "resources//images/sanserif256charwidth.dat", 1024, 768, &OrthoRect);
  
   g_pLargeFont = new CBitmapFont;
   g_pLargeFont->Initialize("resources//images/tahoma512.tga", "resources//images/tahoma512charwidth.dat", 1024, 768, &OrthoRect);

   g_pSmallFont = new CBitmapFont;
   g_pSmallFont->Initialize("resources//images/sanserif128.tga", "resources//images/sanserif128charwidth.dat", 1024, 768, &OrthoRect);

   switch(m_WrapperState)
   {
      case MAIN_MENU:
      {
         MenuInit();
      }
         break;

      case BRIEFING:
      {
         switch(GameMain.m_CurrentLevel)
         {
            case GameMain.Level1:
            {
               Briefing1Init();
            }
               break;

            case GameMain.Level2:
            {
               Briefing2Init();
            }
               break;

            case GameMain.Level3:
            {
               Briefing3Init();
            }
               break;
         }
      }
         break;

      case LEVEL_VICTORY:
      {

         VictoryInit();
      }
         break;

      case LEVEL_LOST:
      {
         LostInit();
      }
         break;

      default:
         break;
   }

   CGameInterfaceManager::GetInstance()->SetMousePos(m_MousePos);
}

void CMenuWrapper::VictoryInit()
{
   string largepath = "resources\\images\\menuimages\\largebuttonmouseover.tga";
   string medpath = "resources\\images\\menuimages\\medbuttonmouseover.tga";
   string smallpath = "resources\\images\\menuimages\\radiomouseover.tga";
   string radiopath = "resources\\images\\menuimages\\radiobutton_notclicked.tga";
   string radioclickpath = "resources\\images\\menuimages\\radiobutton_clicked.tga";
   int         TextureIdx;
   Point3f     Pos;

   Pos.z = 0.0f;
   Pos.x = 0.0f;
   Pos.y = 0.0f;

   pGLSTATE->SetState(ALPHA_TEST, false);

   m_pInput = CInputManager::GetInstance();
   
   CGameInterfaceManager::GetInstance()->Init();

   m_TextToScreen.LoadTextureFont("arial.bmp");
   m_TextToScreen.SetScale(.025f);
   m_TextToScreen.SetRowSize(20);

   // set texture for mouse
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mousepointer.tga", &Pos);

   CGameInterfaceManager::GetInstance()->SetPointerTexture(TextureIdx);
   g_MenuGraphics.SetPos(TextureIdx, &m_MousePos);

   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);


   // start item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.55f;
   Pos.y = -0.6f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\continue_button.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // back item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.85f;
   Pos.y = -0.60f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\medback_button.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);
   
   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

   m_CurrentMenu = 0;
}

void CMenuWrapper::LostInit()
{
   string largepath = "resources\\images\\menuimages\\largebuttonmouseover.tga";
   string medpath = "resources\\images\\menuimages\\medbuttonmouseover.tga";
   string smallpath = "resources\\images\\menuimages\\radiomouseover.tga";
   string radiopath = "resources\\images\\menuimages\\radiobutton_notclicked.tga";
   string radioclickpath = "resources\\images\\menuimages\\radiobutton_clicked.tga";
   int         TextureIdx;
   Point3f     Pos;

   Pos.z = 0.0f;
   Pos.x = 0.0f;
   Pos.y = 0.0f;

   pGLSTATE->SetState(ALPHA_TEST, false);

   m_pInput = CInputManager::GetInstance();
   
   CGameInterfaceManager::GetInstance()->Init();

   m_TextToScreen.LoadTextureFont("arial.bmp");
   m_TextToScreen.SetScale(.025f);
   m_TextToScreen.SetRowSize(20);

   // set texture for mouse
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mousepointer.tga", &Pos);

   CGameInterfaceManager::GetInstance()->SetPointerTexture(TextureIdx);
   g_MenuGraphics.SetPos(TextureIdx, &m_MousePos);

   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);


   // forward
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.55f;
   Pos.y = -0.6f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\continue_button.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   //back
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.85f;
   Pos.y = -0.60f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\medback_button.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);
   
   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

   m_CurrentMenu = 0;
}

void CMenuWrapper::Briefing1Init()
{
   string largepath = "resources\\images\\menuimages\\largebuttonmouseover.tga";
   string medpath = "resources\\images\\menuimages\\medbuttonmouseover.tga";
   string smallpath = "resources\\images\\menuimages\\radiomouseover.tga";
   string radiopath = "resources\\images\\menuimages\\radiobutton_notclicked.tga";
   string radioclickpath = "resources\\images\\menuimages\\radiobutton_clicked.tga";
   int         TextureIdx;
   Point3f     Pos;

   Pos.z = 0.0f;
   Pos.x = 0.0f;
   Pos.y = 0.0f;

   pGLSTATE->SetState(ALPHA_TEST, false);

   m_pInput = CInputManager::GetInstance();
   
   CGameInterfaceManager::GetInstance()->Init();

   m_TextToScreen.LoadTextureFont("arial.bmp");
   m_TextToScreen.SetScale(.1f);
   m_TextToScreen.SetRowSize(4);

   // set texture for mouse
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mousepointer.tga", &Pos);

   CGameInterfaceManager::GetInstance()->SetPointerTexture(TextureIdx);
   g_MenuGraphics.SetPos(TextureIdx, &m_MousePos);

   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // start item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.55f;
   Pos.y = -0.6f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\playbutton_notcliced.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);


   // back item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.85f;
   Pos.y = -0.60f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);
   
   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

// difficulty menu
// BRIEF_EASY = 0, BRIEF_MEDIUM, BRIEF_HARD, BRIEF_START, BRIEF_BACK };

   // text for easy

   Pos.x = 0.85f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\easy_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // text for medium

   Pos.x = 0.85f;
   Pos.y = 0.15f;
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\medium.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // text for hard
   Pos.x = 0.85f;
   Pos.y = 0.0f;
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\hard_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);


   // make easy button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickDifficultyEasy);

   // medium button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.15f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickDifficultyMedium);

   // hard button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickDifficultyHard);

   CGameInterfaceManager::GetInstance()->ActivateItem(g_GameOptions.GetDifficulty() + 2);

   Pos.x = -0.6f;
   Pos.y = 0.0f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\level1briefingmap.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);


// Load menu
   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\Battle.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -3.85f;
   Pos.y = 3.2f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

   m_CurrentMenu = 0;
}

void CMenuWrapper::Briefing2Init()
{
    string largepath = "resources\\images\\menuimages\\largebuttonmouseover.tga";
   string medpath = "resources\\images\\menuimages\\medbuttonmouseover.tga";
   string smallpath = "resources\\images\\menuimages\\radiomouseover.tga";
   string radiopath = "resources\\images\\menuimages\\radiobutton_notclicked.tga";
   string radioclickpath = "resources\\images\\menuimages\\radiobutton_clicked.tga";
   int         TextureIdx;
   Point3f     Pos;

   Pos.z = 0.0f;
   Pos.x = 0.0f;
   Pos.y = 0.0f;

   pGLSTATE->SetState(ALPHA_TEST, false);

   m_pInput = CInputManager::GetInstance();
   
   CGameInterfaceManager::GetInstance()->Init();

   m_TextToScreen.LoadTextureFont("arial.bmp");
   m_TextToScreen.SetScale(.1f);
   m_TextToScreen.SetRowSize(4);

   // set texture for mouse
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mousepointer.tga", &Pos);

   CGameInterfaceManager::GetInstance()->SetPointerTexture(TextureIdx);
   g_MenuGraphics.SetPos(TextureIdx, &m_MousePos);

   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // start item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.55f;
   Pos.y = -0.6f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\playbutton_notcliced.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);


   // back item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.85f;
   Pos.y = -0.60f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);
   
   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

// difficulty menu
// BRIEF_EASY = 0, BRIEF_MEDIUM, BRIEF_HARD, BRIEF_START, BRIEF_BACK };

   // text for easy

   Pos.x = 0.85f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\easy_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // text for medium

   Pos.x = 0.85f;
   Pos.y = 0.15f;
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\medium.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // text for hard
   Pos.x = 0.85f;
   Pos.y = 0.0f;
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\hard_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);


   // make easy button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickDifficultyEasy);

   // medium button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.15f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickDifficultyMedium);

   // hard button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickDifficultyHard);

   CGameInterfaceManager::GetInstance()->ActivateItem(g_GameOptions.GetDifficulty() + 2);

   Pos.x = -0.6f;
   Pos.y = 0.0f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\level2briefingmap.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);


// Load menu
   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\Battle.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -3.85f;
   Pos.y = 3.2f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

   m_CurrentMenu = 0;

}


void CMenuWrapper::Briefing3Init()
{
   string largepath = "resources\\images\\menuimages\\largebuttonmouseover.tga";
   string medpath = "resources\\images\\menuimages\\medbuttonmouseover.tga";
   string smallpath = "resources\\images\\menuimages\\radiomouseover.tga";
   string radiopath = "resources\\images\\menuimages\\radiobutton_notclicked.tga";
   string radioclickpath = "resources\\images\\menuimages\\radiobutton_clicked.tga";
   int         TextureIdx;
   Point3f     Pos;

   Pos.z = 0.0f;
   Pos.x = 0.0f;
   Pos.y = 0.0f;

   pGLSTATE->SetState(ALPHA_TEST, false);

   m_pInput = CInputManager::GetInstance();
   
   CGameInterfaceManager::GetInstance()->Init();

   m_TextToScreen.LoadTextureFont("arial.bmp");
   m_TextToScreen.SetScale(.1f);
   m_TextToScreen.SetRowSize(4);

   // set texture for mouse
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mousepointer.tga", &Pos);

   CGameInterfaceManager::GetInstance()->SetPointerTexture(TextureIdx);
   g_MenuGraphics.SetPos(TextureIdx, &m_MousePos);

   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // start item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.55f;
   Pos.y = -0.6f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\playbutton_notcliced.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);


   // back item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.85f;
   Pos.y = -0.60f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);
   
   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

// difficulty menu
// BRIEF_EASY = 0, BRIEF_MEDIUM, BRIEF_HARD, BRIEF_START, BRIEF_BACK };

   // text for easy

   Pos.x = 0.85f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\easy_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // text for medium

   Pos.x = 0.85f;
   Pos.y = 0.15f;
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\medium.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // text for hard
   Pos.x = 0.85f;
   Pos.y = 0.0f;
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\hard_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);


   // make easy button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickDifficultyEasy);

   // medium button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.15f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickDifficultyMedium);

   // hard button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickDifficultyHard);

   CGameInterfaceManager::GetInstance()->ActivateItem(g_GameOptions.GetDifficulty() + 2);

      Pos.x = -0.6f;
   Pos.y = 0.0f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\level3briefingmap.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

// Load menu
   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\Battle.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -3.85f;
   Pos.y = 3.2f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

   m_CurrentMenu = 0;

}


void CMenuWrapper::SetState(int State)
{
   m_WrapperState = State;
}


void CMenuWrapper::MenuInit()
{
   int         TextureIdx;

   CVector2f   TempVec;
   Point3f     Pos;

   string largepath = "resources\\images\\menuimages\\largebuttonmouseover.tga";
   string medpath = "resources\\images\\menuimages\\medbuttonmouseover.tga";
   string smallpath = "resources\\images\\menuimages\\radiomouseover.tga";
   //string backclickpath = "resources\\images\\menuimages\\backbutton_clicked.tga";
   string backpath = "resources\\images\\menuimages\\backbutton_notclicked.tga";
   string videopath = "resources\\images\\menuimages\\videooptionsbutton_notclicked.tga";
   //string videoclickpath = "resources\\images\\menuimages\\videooptionsbutton_clicked.tga";
   string soundpath = "resources\\images\\menuimages\\videooptions\\controlsoptionsbutton_notclicked.tga";
   //string soundclickpath = "resources\\images\\menuimages\\videooptions\\controlsoptionsbutton_clicked.tga";
   string radiopath = "resources\\images\\menuimages\\radiobutton_notclicked.tga";
   string radioclickpath = "resources\\images\\menuimages\\radiobutton_clicked.tga";
   string savepath = "resources\\images\\menuimages\\save_button.tga";
   //string saveclickpath = "resources\\images\\menuimages\\savebutton_click.tga";
   string defaultpath = "resources\\images\\menuimages\\default_button.tga";
   //string defaultclickpath = "resources\\images\\menuimages\\defaultbutton_clicked.tga";

   Pos.z = 0.0f;
   Pos.x = 0.0f;
   Pos.y = 0.0f;

   pGLSTATE->SetState(ALPHA_TEST, false);

   m_pInput = CInputManager::GetInstance();
   
   CGameInterfaceManager::GetInstance()->Init();

   TempVec.x = 0.0f;
   TempVec.y = 0.0f;

   m_TextToScreen.LoadTextureFont("arial.bmp");
   m_TextToScreen.SetLocation(TempVec);
   m_TextToScreen.SetScale(.1f);
   m_TextToScreen.SetRowSize(4);

   // set texture for mouse
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mousepointer.tga", &Pos);

   CGameInterfaceManager::GetInstance()->SetPointerTexture(TextureIdx);
   g_MenuGraphics.SetPos(TextureIdx, &m_MousePos);

// create all menus here using InterfaceManager

//  MENU_MAIN = 0, MENU_DIFF, MENU_EDITOR,  MENU_VIDEO, MENU_CREDITS, MENU_QUIT, MENU_SOUND,
//  MENU_CONTROLS, };


/// Main Menu //////////////////////////////////////////////////////////////////////////

// main menu

   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = -0.25f;
   Pos.y = -0.12f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\tankpic.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);


   // make StartGame item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = .30f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\startgamebutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\startgamebutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);


   // editor item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = .15f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\editorbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\editorbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);


   // options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = .0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\optionsbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\optionsbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // credits item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = -.15f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\creditsbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\creditsbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);


   // exit game item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = -.30f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\exitgame_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\splashmenu\\exitgame_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);


// level selection menu
   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // start item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.55f;
   Pos.y = -0.6f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\continue_button.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);


   // back item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.85f;
   Pos.y = -0.60f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\exitbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);
   
   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

// difficulty menu
// BRIEF_EASY = 0, BRIEF_MEDIUM, BRIEF_HARD, BRIEF_START, BRIEF_BACK };

   // text for easy

   Pos.x = 0.85f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\level1_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // text for medium

   Pos.x = 0.85f;
   Pos.y = 0.15f;
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\level2_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // text for hard
   Pos.x = 0.85f;
   Pos.y = 0.0f;
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\level3_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = 0.85f;
   Pos.y = -0.15f;
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\skirmish_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // make easy button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // medium button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.15f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickDifficultyMedium);

   // hard button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // hard button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = 0.70f;
   Pos.y = -0.15f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   Pos.x = -0.2f;
   Pos.y = -0.125f;

   CGameInterfaceManager::GetInstance()->ActivateItem(LEVEL_ONE);
   CGameInterfaceManager::GetInstance()->MakeItemSelected(LEVEL_ONE);
   GameMain.m_CurrentLevel = GameMain.Level1;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\prototypelogo.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

/*
// Load menu
   CGameInterfaceManager::GetInstance()->MakeMenu();

   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\Battle.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -3.85f;
   Pos.y = 3.2f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);
*/

// editor menu
   CGameInterfaceManager::GetInstance()->MakeMenu();


// video menu
   CGameInterfaceManager::GetInstance()->MakeMenu();

   // background
   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\optionsmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // video options label
   Pos.x = -0.70f;
   Pos.y = 0.3f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\videooptions\\videooptions_header.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // res options label
   Pos.x = -0.65f;
   Pos.y = 0.225f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\videooptions\\resolution_header.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // bit depth label
   Pos.x = -0.680f;
   Pos.y = -0.125f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\videooptions\\bitdepth_header.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);


   // 640x480 text
   Pos.x = -0.70f;
   Pos.y = 0.15f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\640X480_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // 800X600 text
   Pos.x = -0.70f;
   Pos.y = 0.05f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\800X600_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // 1024X768 text
   Pos.x = -0.24f;
   Pos.y = 0.15f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\1024X768_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // 1600X1200 text
   Pos.x = -0.24f;
   Pos.y = 0.05f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\1600X1200_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // 16 bit depth text
   Pos.x = -0.75f;
   Pos.y = -0.2f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\16bit.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // 32 bit depth text
   Pos.x = -0.275f;
   Pos.y = -0.2f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\32bit.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

//enum { MODE640X480 = 0, MODE800X600, MODE1024X768, MODE1600X1200, V16B, V32B, VSAVE, VDEFAULT, 
//       VVIDEO, VSOUND, VCONTROL, VBACK };

   // 600x480 button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -0.85f;
   Pos.y = 0.15f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickScreenRez640x480);

   // 800x600 button
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -0.85f;
   Pos.y = 0.05f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickScreenRez800x600);

   // 1024x768
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -0.4f;
   Pos.y = 0.15f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickScreenRez1024x768);

   // 1600x1200
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -0.4f;
   Pos.y = 0.05f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickScreenRez1600x1200);

   // 16 bit depth
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -0.85f;
   Pos.y = -0.2f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickBitDepth16);
   
   if (g_GameOptions.GetBitDepth() == 16)
      CGameInterfaceManager::GetInstance()->ActivateItem(V16B);

   // 32 bit depth
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -0.4f;
   Pos.y = -0.2f;

   TextureIdx = g_MenuGraphics.IRLoad(radiopath, &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(radioclickpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(smallpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   if (g_GameOptions.GetBitDepth() == 32)
      CGameInterfaceManager::GetInstance()->ActivateItem(V32B);

   CGameInterfaceManager::GetInstance()->SetItemFuncAble(OnClickBitDepth32);

// same code from here on in setup each options menu

   // save menu item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -.17f;
   Pos.y = -.53f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\set_button.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\savebutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // default menu item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .12f;
   Pos.y = -.53f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\default_button.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\defaultbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // video options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\videooptionbuttion_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\videooptionbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // sound options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = .15f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\soundoptionsbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\soundoptionsbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // control options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = -.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\videooptions\\controlsoptionsbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\videooptions\\controlsoptionsbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // back options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = -.15f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\backbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\backbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->ActivateItem(g_GameOptions.GetVideoMode());
   
   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);


// credits menu

   CGameInterfaceManager::GetInstance()->MakeMenu();
   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\mainmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);
   // back options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = -.60f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\backbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\backbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);
   
   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

   Pos.x = -0.325f;
   Pos.y = 0.075f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\creditsscreen.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);


// Quit Menu
   CGameInterfaceManager::GetInstance()->MakeMenu();

// Sound Menu

//enum { MVOL1, MVOL2, MVOL3, MVOL4, MVOL5, MVOL6, MVOL7, MVOL8, MVOL9, MVOL10,
//       FVOL1, FVOL2, FVOL3, FVOL4, FVOL5, FVOL6, FVOL7, FVOL8, FVOL9, FVOL10, 
//       SSAVE, SDEFAULT, SVIDEO, SSOUND, SCONTROL, SBACK };

   CGameInterfaceManager::GetInstance()->MakeMenu();

   // background
   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\optionsmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = -0.65f;
   Pos.y = 0.3f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\soundoptions\\soundoptions_header.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = -0.5f;
   Pos.y =  0.2f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\soundoptions\\fxvolume_header.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x =  -0.7f;
   Pos.y =  -0.25f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\soundoptions\\min_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = -0.1f;
   Pos.y =  -0.25f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\soundoptions\\max_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = -0.57f;
   Pos.y = -0.1f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\soundoptions\\musicvolume_header.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x = -.7f;
   Pos.y =  0.05f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\soundoptions\\min_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   Pos.x =  -0.1f;
   Pos.y =  0.05f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\soundoptions\\max_text.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);
 
// fx volume
   CGameInterfaceManager::GetInstance()->MakeItem(); // ??? how are we going to do this?

   Pos.x = -0.4f;
   Pos.y =  0.05f;

   string soundpointer = "resources\\images\\menuimages\\soundoptions\\Gradientbar.tga";

   m_FXVolBar = g_MenuGraphics.IRLoad(soundpointer, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(m_FXVolBar);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(m_FXVolBar);
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(m_FXVolBar);
   
// music volume
   CGameInterfaceManager::GetInstance()->MakeItem(); // ??? how are we going to do this?

   Pos.x = -0.4f;
   Pos.y =  -0.25f;

   m_MusicVolBar = g_MenuGraphics.IRLoad(soundpointer, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(m_MusicVolBar);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(m_MusicVolBar);
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(m_MusicVolBar);


// fx volume
   CGameInterfaceManager::GetInstance()->MakeItem(); // ??? how are we going to do this?

   float volval = g_GameOptions.GetFXVol();

   FloatRect r = g_MenuGraphics.GetRect(m_FXVolBar);
   float length = r.Max.x - r.Min.x;
   Pos.x = -.4f - (length/2.0f) + length * volval;
   Pos.y = 0.05f;
   Pos.z = 0.0f;

   m_FXBarStartPos.x = -0.4f - (length/2.0f);
   m_FXBarStartPos.y = .05f;
   m_FXBarStartPos.z = 0.0f;

   soundpointer = "resources\\images\\menuimages\\soundoptions\\gradientPointer.tga";

   m_FXVolPointer = g_MenuGraphics.IRLoad(soundpointer, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(m_FXVolPointer);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(m_FXVolPointer);
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(m_FXVolPointer);

// music volume
   CGameInterfaceManager::GetInstance()->MakeItem(); // ??? how are we going to do this?

   volval = g_GameOptions.GetMusicVol();

   r = g_MenuGraphics.GetRect(m_MusicVolBar);
   Pos.x =  -.4f - (length/2.0f) + length * volval;
   Pos.y = -0.25f;
   Pos.z = 0.0f;

   m_MusicBarStartPos.x = -0.4f - (length/2.0f);
   m_MusicBarStartPos.y = -.25f;
   m_MusicBarStartPos.z = 0.0f;

   m_MusicVolPointer = g_MenuGraphics.IRLoad(soundpointer, &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(m_MusicVolPointer);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(m_MusicVolPointer);
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(m_MusicVolPointer);


// same code from here on in setup each options menu

   // save menu item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = -.17f;
   Pos.y = -.53f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\set_button.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\savebutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // default menu item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .12f;
   Pos.y = -.53f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\default_button.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\defaultbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(medpath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // video options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\videooptionbuttion_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\videooptionbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // sound options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = .15f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\soundoptionsbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\soundoptionsbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // control options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = -.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\videooptions\\controlsoptionsbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\videooptions\\controlsoptionsbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // back options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = -.15f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\backbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\backbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->ActivateItem(g_GameOptions.GetVideoMode());
   
   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

// Controls menu
   CGameInterfaceManager::GetInstance()->MakeMenu();

   // background
   Pos.x = 0.0f;
   Pos.y = 0.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\optionsmenubg.jpg", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   // video options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = 0.30f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\videooptionbuttion_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\videooptionbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // sound options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = .15f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\soundoptionsbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\VideoOptions\\soundoptionsbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // control options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = -.0f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\videooptions\\controlsoptionsbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\videooptions\\controlsoptionsbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   // back options item
   CGameInterfaceManager::GetInstance()->MakeItem();

   Pos.x = .75f;
   Pos.y = -.15f;

   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\backbutton_notclicked.tga", &Pos);
   
   CGameInterfaceManager::GetInstance()->SetInactiveUnSelectedTexture(TextureIdx);

   //TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\backbutton_clicked.tga", &Pos);
   CGameInterfaceManager::GetInstance()->SetActiveUnSelectedTexture(TextureIdx);

   TextureIdx = g_MenuGraphics.IRLoad(largepath, &Pos);
   CGameInterfaceManager::GetInstance()->SetInactiveSelectedTexture(TextureIdx);
   CGameInterfaceManager::GetInstance()->SetActiveSelectedTexture(TextureIdx);

   CGameInterfaceManager::GetInstance()->MakeItemSelected(0);

   Pos.x = -0.325f;
   Pos.y = -0.1f;

   // background
   TextureIdx = g_MenuGraphics.IRLoad("resources\\images\\menuimages\\controlsscreen.tga", &Pos);
   CGameInterfaceManager::GetInstance()->AddBackground(TextureIdx);

   m_CurrentMenu = MENU_MAIN;
   CGameInterfaceManager::GetInstance()->SetCurrentMenu(m_CurrentMenu);
}

unsigned int CMenuWrapper::Update()
{

   int ReturnValue = 0;

   switch(m_WrapperState)
   {
      case MAIN_MENU:
      {
         ReturnValue = MenuUpdate();
      }
         break;

      case BRIEFING:
      {
         switch(GameMain.m_CurrentLevel)
         {
            case GameMain.Level1:
            {
               ReturnValue = Briefing1Update();
            }
               break;

            case GameMain.Level2:
            {
               ReturnValue = Briefing2Update();
            }
               break;

            case GameMain.Level3:
            {
               ReturnValue = Briefing3Update();
            }
               break;
         }
      }
         break;

      case LEVEL_VICTORY:
      {
         ReturnValue = VictoryUpdate();
      }
         break;

      case LEVEL_LOST:
      {
         ReturnValue = LostUpdate();
      }
         break;

      default:
         break;
   }

   CGameInterfaceManager::GetInstance()->SetCurrentMenu(m_CurrentMenu);

   return ReturnValue;
}

unsigned int CMenuWrapper::VictoryUpdate()
{
   int ReturnValue = 0;

   if (CGameInterfaceManager::GetInstance()->GetItemClicked(VICTORY_BACK))
   {
      m_ButtonSet->Play2DAudio(200);
      m_CurrentMenu = MENU_MAIN;
      SetState(0);
      GetInstance()->Shutdown();
      GetInstance()->Init();
   }
   else if (CGameInterfaceManager::GetInstance()->GetItemClicked(VICTORY_START))
   {
      if(GameMain.m_CurrentLevel == GameMain.Level1 || GameMain.m_CurrentLevel == GameMain.Level2)
      {
         m_ButtonSet->Play2DAudio(200);
         CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_START);
         m_CurrentMenu = BRIEFING_MENU;
         SetState(BRIEFING);
         GetInstance()->Shutdown();
         GetInstance()->Init();
      }
      else
      {
         m_ButtonSet->Play2DAudio(200);
         CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_START);        
         SetState(MAIN_MENU);
         GetInstance()->Shutdown();
         GetInstance()->Init(); 
         m_CurrentMenu = MENU_CREDITS;
      }
   }
   return ReturnValue;
}

unsigned int CMenuWrapper::LostUpdate()
{
   int ReturnValue = 0;

   if (CGameInterfaceManager::GetInstance()->GetItemClicked(LOST_BACK))
   {
      m_ButtonSet->Play2DAudio(200);
      m_CurrentMenu = MENU_MAIN;
      SetState(0);
      GetInstance()->Shutdown();
      GetInstance()->Init();
   }

   else if (CGameInterfaceManager::GetInstance()->GetItemClicked(LOST_START) && GameMain.m_CurrentLevel == GameMain.Skirmish)
   {
      m_ButtonSet->Play2DAudio(200);
      CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_START);
      ReturnValue = 1;   
   }
   
   else if(CGameInterfaceManager::GetInstance()->GetItemClicked(LOST_START))
   {
      m_ButtonSet->Play2DAudio(200);
      CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_START);
      m_CurrentMenu = BRIEFING_MENU;
      SetState(BRIEFING);
      GetInstance()->Shutdown();
      GetInstance()->Init();  
   }

   return ReturnValue;
}

unsigned int CMenuWrapper::Briefing1Update()
{
   int ReturnValue = 0;

   switch(m_CurrentMenu)
   {
      case BRIEFING_MENU:
      {
         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_EASY))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_MEDIUM);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_HARD);

         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_MEDIUM))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(BRIEF_MEDIUM);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_MEDIUM);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_HARD);

         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_HARD))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(BRIEF_HARD);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_HARD);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_MEDIUM);
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_BACK))
         {
            m_ButtonSet->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_BACK);
            m_CurrentMenu = MENU_MAIN;
            SetState(0);
            GetInstance()->Shutdown();
            GetInstance()->Init();
            break;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_START))
         {
            m_ButtonSet->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_START);
            m_CurrentMenu = LOAD_MENU;
            ReturnValue = 1;
         }
      }
         break;

      case LOAD_MENU:
      {

      }
         break;
   }



   return ReturnValue;
}

unsigned int CMenuWrapper::Briefing2Update()
{
   int ReturnValue = 0;

   switch(m_CurrentMenu)
   {
      case BRIEFING_MENU:
      {
         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_EASY))
         {  
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_MEDIUM);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_HARD);

         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_MEDIUM))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(BRIEF_MEDIUM);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_MEDIUM);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_HARD);

         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_HARD))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(BRIEF_HARD);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_HARD);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_MEDIUM);
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_BACK))
         {
            m_ButtonSet->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_BACK);
            m_CurrentMenu = MENU_MAIN;
            SetState(0);
            GetInstance()->Shutdown();
            GetInstance()->Init();
            break;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_START))
         {
            m_ButtonSet->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_START);
            m_CurrentMenu = LOAD_MENU;
            ReturnValue = 1;
         }
      }
         break;

      case LOAD_MENU:
      {

      }
         break;
   }

   return ReturnValue;
}

unsigned int CMenuWrapper::Briefing3Update()
{
  int ReturnValue = 0;

   switch(m_CurrentMenu)
   {
      case BRIEFING_MENU:
      {
         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_EASY))
         { 
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_MEDIUM);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_HARD);

         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_MEDIUM))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(BRIEF_MEDIUM);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_MEDIUM);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_HARD);

         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_HARD))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(BRIEF_HARD);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_HARD);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_EASY);
            CGameInterfaceManager::GetInstance()->DeactivateItem(BRIEF_MEDIUM);
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_BACK))
         {
            m_ButtonSet->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_BACK);
            m_CurrentMenu = MENU_MAIN;
            SetState(0);
            GetInstance()->Shutdown();
            GetInstance()->Init();
            break;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(BRIEF_START))
         {
            m_ButtonSet->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(BRIEF_START);
            m_CurrentMenu = LOAD_MENU;
            ReturnValue = 1;
         }
      }
         break;

      case LOAD_MENU:
      {

      }
         break;
   }

   return ReturnValue;
}


unsigned int CMenuWrapper::MenuUpdate()
{
   int iReturnVal = 0;

   CGameInterfaceManager::GetInstance()->Update();

   switch(m_CurrentMenu)
   {
      case MENU_MAIN:
      {
         if (CGameInterfaceManager::GetInstance()->GetItemClicked(START_GAME))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(START_GAME);
            m_CurrentMenu = MENU_LEVEL;
            break;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(CONFIGURE_OPTIONS))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(CONFIGURE_OPTIONS);
            m_CurrentMenu = MENU_VIDEO;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(EDITOR))
         {
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(EDITOR);
            m_ButtonClick->Play2DAudio(200);
            iReturnVal = 2;
         }



         if (CGameInterfaceManager::GetInstance()->GetItemClicked(CREDITS))
         {
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(CREDITS);
            m_CurrentMenu = MENU_CREDITS;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(QUIT_GAME))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(QUIT_GAME);
            m_CurrentMenu = MENU_QUIT;
         }

      }
         break;

      case MENU_LEVEL:
      {
         if (CGameInterfaceManager::GetInstance()->GetItemClicked(LEVEL_START))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(START_GAME);        
           
            if (GameMain.m_CurrentLevel != GameMain.Skirmish)
            {
               m_CurrentMenu = BRIEFING_MENU;
               SetState(BRIEFING);
               GetInstance()->Shutdown();
               GetInstance()->Init();
            }
            else
            {
               m_ButtonSet->Play2DAudio(200);
               m_CurrentMenu = LOAD_MENU;
               iReturnVal = 1;
            }
            break;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(LEVEL_BACK))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(LEVEL_BACK);
            m_CurrentMenu = MAIN_MENU;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(LEVEL_ONE))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(LEVEL_ONE);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(LEVEL_ONE);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_TWO);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_THREE);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_SKIRMISH);
            GameMain.m_CurrentLevel = GameMain.Level1;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(LEVEL_TWO))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(LEVEL_TWO);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(LEVEL_TWO);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_ONE);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_THREE);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_SKIRMISH);
            GameMain.m_CurrentLevel = GameMain.Level2;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(LEVEL_THREE))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(LEVEL_THREE);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(LEVEL_THREE);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_ONE);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_TWO);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_SKIRMISH);
            GameMain.m_CurrentLevel = GameMain.Level3;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(LEVEL_SKIRMISH))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->ActivateItem(LEVEL_SKIRMISH);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(LEVEL_SKIRMISH);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_ONE);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_TWO);
            CGameInterfaceManager::GetInstance()->DeactivateItem(LEVEL_THREE);
            GameMain.m_CurrentLevel = GameMain.Skirmish;
         }

         break;
      }

      case MENU_CREDITS:
      {

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(CREDBACK))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(CREDBACK);
            m_CurrentMenu = MENU_MAIN;
         }

         break;
      }

      case MENU_VIDEO:
      {

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(VSOUND))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(VSOUND);
            m_CurrentMenu = MENU_SOUND;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(VCONTROL))
         {
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(VCONTROL);
            m_CurrentMenu = MENU_CONTROLS;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(VBACK))
         {
            m_ButtonSet->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(VBACK);
            m_CurrentMenu = MENU_MAIN;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(MODE640X480))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(MODE640X480);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE800X600);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE1024X768);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE1600X1200);
            CGameInterfaceManager::GetInstance()->ActivateItem(MODE640X480);
         }
         
         if (CGameInterfaceManager::GetInstance()->GetItemClicked(MODE800X600))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(MODE800X600);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE640X480);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE1024X768);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE1600X1200);
            CGameInterfaceManager::GetInstance()->ActivateItem(MODE800X600);
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(MODE1024X768))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(MODE1024X768);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE640X480);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE800X600);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE1600X1200);
            CGameInterfaceManager::GetInstance()->ActivateItem(MODE1024X768);
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(MODE1600X1200))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(MODE1600X1200);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE640X480);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE800X600);
            CGameInterfaceManager::GetInstance()->DeactivateItem(MODE1024X768);
            CGameInterfaceManager::GetInstance()->ActivateItem(MODE1600X1200);
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(V16B))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(V16B);
            CGameInterfaceManager::GetInstance()->DeactivateItem(V32B);
            CGameInterfaceManager::GetInstance()->ActivateItem(V16B);
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(V32B))
         {
            m_RadioClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(V32B);
            CGameInterfaceManager::GetInstance()->DeactivateItem(V16B);
            CGameInterfaceManager::GetInstance()->ActivateItem(V32B);
         }

         if(CGameInterfaceManager::GetInstance()->GetItemClicked(VSAVE))
         {
            m_ButtonSet->Play2DAudio(200);
            MenuMain.Shutdown();
            pGLWIN->DestroyWindowGL (&pGLWIN->window);
            pGLWIN->CreateWindowGL (&pGLWIN->window, g_GameOptions.GetVideoWidth(), g_GameOptions.GetVideoHeight(), g_GameOptions.GetBitDepth(), true);
            
            pGLSTATE->Initialize();
            pGLWIN->OpenGLInit();
            MenuMain.Initialize();   
            CMenuWrapper::GetInstance()->m_CurrentMenu = MENU_VIDEO;  
         }

      }
         break;

      case MENU_SOUND:
      {
         if (CGameInterfaceManager::GetInstance()->GetItemClicked(SVIDEO))
         {
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(SVIDEO);
            m_CurrentMenu = MENU_VIDEO;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(SCONTROL))
         {
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(SCONTROL);
            m_CurrentMenu = MENU_CONTROLS;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(MVOL1))
         {
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(MVOL1);
            m_MousePos = CGameInterfaceManager::GetInstance()->GetMousePos();
            FloatRect r = g_MenuGraphics.GetRect(m_MusicVolBar);
            g_GameOptions.SetMusicVol((m_MousePos.x - r.Min.x)/(r.Max.x - r.Min.x));
            CVector3f pos;
            pos.x = m_MousePos.x;
            pos.y = m_MusicBarStartPos.y;
            pos.z = 0.0f;
            g_MenuGraphics.SetPos(m_MusicVolPointer, &pos);
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(FVOL1))
         {
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(FVOL1);
            m_MousePos = CGameInterfaceManager::GetInstance()->GetMousePos();
            FloatRect r = g_MenuGraphics.GetRect(m_FXVolBar);
            g_GameOptions.SetFXVol((m_MousePos.x - r.Min.x)/(r.Max.x - r.Min.x));
            CVector3f pos;
            pos.x = m_MousePos.x;
            pos.y = m_FXBarStartPos.y;
            pos.z = 0.0f;
            g_MenuGraphics.SetPos(m_FXVolPointer, &pos);
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(SBACK))
         {
            //g_AudioManager.SetAudioRatios(g_GameOptions.GetFXVol() * 2, g_GameOptions.GetMusicVol() * 2);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(SBACK);
            m_CurrentMenu = MENU_MAIN;
         }
      }
         break;

      case MENU_CONTROLS:
      {
         if (CGameInterfaceManager::GetInstance()->GetItemClicked(CVIDEO))
         {
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(CVIDEO);
            m_CurrentMenu = MENU_VIDEO;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(CSOUND))
         {
            m_ButtonClick->Play2DAudio(200);
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(CSOUND);
            m_CurrentMenu = MENU_SOUND;
         }

         if (CGameInterfaceManager::GetInstance()->GetItemClicked(CBACK))
         {
            CGameInterfaceManager::GetInstance()->MakeItemUnclicked(CBACK);
            m_CurrentMenu = MENU_MAIN;
         }
      }
         break;

      case MENU_QUIT:
      {
         iReturnVal = 255;
      }
         break;

      default:
         break;
   }

   // -1 quit game, 0 menus are still active, 1 quit menus and start game
   return iReturnVal;
}


void CMenuWrapper::RefreshItems(int FirstItem, int LastItem, int ExcludeItem)
{
   for (int i = FirstItem; i <= LastItem; i++)
   {
      if (i == ExcludeItem)
      {
         CGameInterfaceManager::GetInstance()->ActivateItem(i);
         CGameInterfaceManager::GetInstance()->MakeItemUnclicked(i);
      }
      else
         CGameInterfaceManager::GetInstance()->DeactivateItem(i);
   }
}


void CMenuWrapper::Render()
{ 
   switch(m_WrapperState)
   {
      case MAIN_MENU:
      {
         MenuRender();
      }
         break;

      case BRIEFING:
      {
         switch(GameMain.m_CurrentLevel)
         {
            case GameMain.Level1:
            {
               Briefing1Render();
            }
               break;

            case GameMain.Level2:
            {
               Briefing2Render();
            }
               break;

            case GameMain.Level3:
            {
               Briefing3Render();
            }
               break;
         }
      }
         break;

      case LEVEL_VICTORY:
      {
         VictoryRender();
      }
         break;

      case LEVEL_LOST:
      {
         LostRender();
      }
         break;

      default:
         break;
   }
}

void CMenuWrapper::Briefing1Render()
{
   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   CVector2f Pos;
   Color4f Color = {1.0f, 1.0f, 1.0f, 1.0f};

   if (m_CurrentMenu == LOAD_MENU)
      CGameInterfaceManager::GetInstance()->ShowMouse(false);

   CGameInterfaceManager::GetInstance()->Render();

   if (m_CurrentMenu == LOAD_MENU)
      CGameInterfaceManager::GetInstance()->ShowMouse(true);

   if (m_CurrentMenu != LOAD_MENU)
   {
      Color4f Color = {1.0f, 1.0f, 1.0f, 1.0f};
      CVector2f Pos;
      
      g_pLargeFont->BeginRendering();
      Pos.x = -0.85f;
      Pos.y = 0.425f;
      g_pLargeFont->Print("Level 1 : Protect the convoy", &Pos, &Color);
      
      g_pLargeFont->EndRendering();
      pGLSTATE->SetState(DEPTH_TEST, false);

      g_pFont->BeginRendering();
      Pos.x = -0.3f;
      Pos.y = 0.325f;
  
      g_pFont->Print("Commander there is a strong Choikin presence", &Pos, &Color);

      pGLSTATE->SetState(DEPTH_TEST, false);
      g_pFont->EndRendering();

      g_pFont->BeginRendering();
      Pos.x = -0.3f;
      Pos.y = 0.225f;
      g_pFont->Print("in the area and they threaten to cut off the", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = 0.125f;
      g_pFont->Print("small outpost of Vesta.  We must get them a supply", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = 0.025f;
      g_pFont->Print("of weapons to Vesta so that they can defend themselves. ", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = -0.075f;
      g_pFont->Print("Your mission is to escort 3 supply trucks to the", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = -0.175f;
      g_pFont->Print("besieged city.  It is imperative that at least 2 of ", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = -0.275f;
      g_pFont->Print("the supply trucks make it through if Vesta is to hold.", &Pos, &Color);

      Pos.x = -0.84f;
      Pos.y = -0.35f;
      g_pFont->Print("Objectives:", &Pos, &Color);

      Color.r = 0.0f;
      Color.b= 0.0f;

      Pos.x = -0.80f;
      Pos.y = -0.40f;
      g_pFont->Print("- Escort at least two supply", &Pos, &Color);
      Pos.x = -0.80f;
      Pos.y = -0.45f;
      g_pFont->Print("  trucks to Vesta.", &Pos, &Color);

      Pos.x = -0.80f;
      Pos.y = -0.525f;
      g_pFont->Print("- Remain alive.", &Pos, &Color);

      g_pFont->EndRendering();
      pGLSTATE->SetState(DEPTH_TEST, false);
   }

   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   if (m_CurrentMenu != LOAD_MENU)
      CGameInterfaceManager::GetInstance()->RenderMouse();

   pGLSTATE->SetState(BLEND, false);
   pGLSTATE->SetState(LIGHTING, true);
}

void CMenuWrapper::Briefing2Render()
{
   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   if (m_CurrentMenu == LOAD_MENU)
      CGameInterfaceManager::GetInstance()->ShowMouse(false);

   CGameInterfaceManager::GetInstance()->Render();

   CVector2f vec;

   if (m_CurrentMenu != LOAD_MENU)
   {
    Color4f Color = {1.0f, 1.0f, 1.0f, 1.0f};
      CVector2f Pos;
      
      g_pLargeFont->BeginRendering();
      Pos.x = -0.85f;
      Pos.y = 0.425f;
      g_pLargeFont->Print("Level 2 : Defend Corinth", &Pos, &Color);
      
      g_pLargeFont->EndRendering();
      pGLSTATE->SetState(DEPTH_TEST, false);

      g_pFont->BeginRendering();
      Pos.x = -0.3f;
      Pos.y = 0.325f;
  
      g_pFont->Print("Choikin forces have amassed around the captial", &Pos, &Color);

      pGLSTATE->SetState(DEPTH_TEST, false);
      g_pFont->EndRendering();

      g_pFont->BeginRendering();
      Pos.x = -0.3f;
      Pos.y = 0.225f;
      g_pFont->Print("city of Corinth.  You must bolster their defenses", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = 0.125f;
      g_pFont->Print("and drive off the insidious Choikin menace. ", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = 0.025f;
      g_pFont->Print("The Choikin are ruthless and intend to kill every", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = -0.075f;
      g_pFont->Print("last civilian in Corinth.  You must protect the", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = -0.175f;
      g_pFont->Print("civilian buildings at all costs.  Loss of 6 or more", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = -0.275f;
      g_pFont->Print("buildings will deem Corinth uninhabitable and will", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = -0.375f;
      g_pFont->Print("inevitably result in our defeat on Tanix.", &Pos, &Color);

      Pos.x = -0.84f;
      Pos.y = -0.35f;
      g_pFont->Print("Objectives:", &Pos, &Color);

      Color.r = 0.0f;
      Color.b= 0.0f;

      Pos.x = -0.80f;
      Pos.y = -0.40f;
      g_pFont->Print("- Keep at least 6 buildings", &Pos, &Color);
      Pos.x = -0.80f;
      Pos.y = -0.45f;
      g_pFont->Print("  intact.", &Pos, &Color);

      Pos.x = -0.80f;
      Pos.y = -0.525f;
      g_pFont->Print("- Remain alive.", &Pos, &Color);

      g_pFont->EndRendering();
      pGLSTATE->SetState(DEPTH_TEST, false);
   }

   if (m_CurrentMenu == LOAD_MENU)
      CGameInterfaceManager::GetInstance()->ShowMouse(true);

   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   if (m_CurrentMenu != LOAD_MENU)
      CGameInterfaceManager::GetInstance()->RenderMouse();

   pGLSTATE->SetState(BLEND, false);
   pGLSTATE->SetState(LIGHTING, true);
}

void CMenuWrapper::Briefing3Render()
{
   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   if (m_CurrentMenu == LOAD_MENU)
      CGameInterfaceManager::GetInstance()->ShowMouse(false);

   CGameInterfaceManager::GetInstance()->Render();

   CVector2f vec;

   if (m_CurrentMenu != LOAD_MENU)
   {
    Color4f Color = {1.0f, 1.0f, 1.0f, 1.0f};
      CVector2f Pos;
      
      g_pLargeFont->BeginRendering();
      Pos.x = -0.85f;
      Pos.y = 0.425f;
      g_pLargeFont->Print("Level 3 : Destroy the Choikin Mothership", &Pos, &Color);
      
      g_pLargeFont->EndRendering();
      pGLSTATE->SetState(DEPTH_TEST, false);

      g_pFont->BeginRendering();
      Pos.x = -0.3f;
      Pos.y = 0.325f;
  
      g_pFont->Print("We have located the Choikin encampment.  Rid", &Pos, &Color);

      pGLSTATE->SetState(DEPTH_TEST, false);
      g_pFont->EndRendering();

      g_pFont->BeginRendering();
      Pos.x = -0.3f;
      Pos.y = 0.225f;
      g_pFont->Print("the planet of the last of the Choikin presence.", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = 0.125f;
      g_pFont->Print("Beware, enemy activity is very high in this area.", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = 0.025f;
      g_pFont->Print("We have detected an unusually strong energy signature", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = -0.075f;
      g_pFont->Print("of unknown origin in the enemy base camp.  They may", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = -0.175f;
      g_pFont->Print("have some powerful new kind of weapon.", &Pos, &Color);

      Pos.x = -0.3f;
      Pos.y = -0.275f;
      g_pFont->Print("Be careful, Commander.", &Pos, &Color);

      Pos.x = -0.84f;
      Pos.y = -0.35f;
      g_pFont->Print("Objectives:", &Pos, &Color);

      Color.r = 0.0f;
      Color.b= 0.0f;

      Pos.x = -0.80f;
      Pos.y = -0.40f;
      g_pFont->Print("- Destroy Choikin Mothership", &Pos, &Color);

      Pos.x = -0.80f;
      Pos.y = -0.475f;
      g_pFont->Print("- Remain alive", &Pos, &Color);

      g_pFont->EndRendering();
      pGLSTATE->SetState(DEPTH_TEST, false);
   }

   if (m_CurrentMenu == LOAD_MENU)
      CGameInterfaceManager::GetInstance()->ShowMouse(true);

   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   if (m_CurrentMenu != LOAD_MENU)
      CGameInterfaceManager::GetInstance()->RenderMouse();

   pGLSTATE->SetState(BLEND, false);
   pGLSTATE->SetState(LIGHTING, true);
}

void CMenuWrapper::VictoryRender()
{
   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   CGameInterfaceManager::GetInstance()->Render();

   char buffer[256];

   g_pFont->BeginRendering();
   CVector2f Pos;
   Color4f Color = {1.0f, 1.0f, 1.0f, 1.0f};
   Pos.x = -0.75f;
   Pos.y = 0.4f;

   g_pFont->Print("Mission Successful!", &Pos, &Color);

     Pos.x = -0.7f;
   Pos.y = 0.3f;

   g_pFont->Print("Mission Time: ", &Pos, &Color);

   int minutes = int(g_Player.m_TimeMissionEnded - g_Player.m_TimeMissionStarted) / 60;
   int seconds = int(g_Player.m_TimeMissionEnded - g_Player.m_TimeMissionStarted) % 60;

   if (seconds > 9)
      sprintf(buffer, "%d:%d", minutes, seconds);
   else
      sprintf(buffer, "%d:0%d", minutes, seconds);

   Pos.x = -0.4f;
   Pos.y = 0.3f;
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = -0.7f;
   Pos.y = 0.2f;
   g_pFont->Print("Enemies Destroyed", &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = 0.125f;
   g_pFont->Print("- Medium Tanks:" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = 0.125f;
   itoa(g_Player.m_EnemyMedTanksDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = 0.05f;
   g_pFont->Print("- Heavy Tanks:" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = 0.05f;
   itoa(g_Player.m_EnemyHvyTanksDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = -0.025f;
   g_pFont->Print("- Missile Tanks:" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = -0.025f;
   itoa(g_Player.m_EnemyMslTanksDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = -0.1f;
   g_pFont->Print("- Scouts" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = -0.1f;
   itoa(g_Player.m_EnemyScoutsDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = -0.175f;
   g_pFont->Print("- Kamikaze Units" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = -0.175f;
   itoa(g_Player.m_EnemyMeleeTanksDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = -0.25f;
   g_pFont->Print("- Bombers" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = -0.25f;
   itoa(g_Player.m_EnemyFightersDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = 0.0f;
   Pos.y = 0.3f;
   g_pFont->Print("Plasma Shots" , &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = 0.225f;
   g_pFont->Print("- Shots fired:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = 0.225f;
   itoa(g_Player.m_ShotsFiredPlasma, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = 0.150f;
   g_pFont->Print("- Shots hit:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = 0.150f;
   itoa(g_Player.m_HitsWithPlasma, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = 0.075f;
   g_pFont->Print("- Hit percentage:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = 0.075f;
   int percentage;
   if(g_Player.m_ShotsFiredPlasma == 0)
      percentage = 100;
   else
      percentage = int((float)g_Player.m_HitsWithPlasma / (float)g_Player.m_ShotsFiredPlasma * 100.0f);
   itoa(percentage, buffer, 10);
   if(percentage < 10)
   {
      char temp = buffer[0];
      buffer[0] = '0';
      buffer[1] = temp;
   }
   g_pFont->Print(buffer, &Pos, &Color);


   Pos.x = 0.0f;
   Pos.y = 0.0f;
   g_pFont->Print("Cluster Missiles" , &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = -0.075f;
   g_pFont->Print("- Shots fired:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = -0.075f;
   itoa(g_Player.m_ShotsFiredMissiles, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = -0.15f;
   g_pFont->Print("- Shots hit:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = -0.150f;
   itoa(g_Player.m_HitsWithMissiles, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = -0.225f;
   g_pFont->Print("- Hit percentage:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = -0.225f;
   if(g_Player.m_ShotsFiredMissiles == 0)
      percentage = 100;
   else
      percentage = int((float)g_Player.m_HitsWithMissiles / (float)g_Player.m_ShotsFiredMissiles * 100.0f);
   itoa(percentage, buffer, 10);
   if(percentage < 10)
   {
      char temp = buffer[0];
      buffer[0] = '0';
      buffer[1] = temp;
   }
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.5f;
   Pos.y = 0.3f;
   g_pFont->Print("Tracking Missiles" , &Pos, &Color);

   Pos.x = 0.55f;
   Pos.y = 0.225f;
   g_pFont->Print("- Shots fired:" , &Pos, &Color);

   Pos.x = 0.8f;
   Pos.y = 0.225f;
   itoa(g_Player.m_ShotsFiredTrkMissiles, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.55f;
   Pos.y = 0.150f;
   g_pFont->Print("- Shots hit:" , &Pos, &Color);

   Pos.x = 0.8f;
   Pos.y = 0.150f;
   itoa(g_Player.m_HitsWithTrkMissiles, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.55f;
   Pos.y = 0.075f;
   g_pFont->Print("- Hit percentage:" , &Pos, &Color);

   Pos.x = 0.8f;
   Pos.y = 0.075f;
   if(g_Player.m_ShotsFiredTrkMissiles == 0)
      percentage = 100;
   else
      percentage = int((float)g_Player.m_HitsWithTrkMissiles / (float)g_Player.m_ShotsFiredTrkMissiles * 100.0f);
   itoa(percentage, buffer, 10);
   if(percentage < 10)
   {
      char temp = buffer[0];
      buffer[0] = '0';
      buffer[1] = temp;
   }
   g_pFont->Print(buffer, &Pos, &Color);


   g_pFont->EndRendering();
   pGLSTATE->SetState(DEPTH_TEST, false);
   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      CGameInterfaceManager::GetInstance()->RenderMouse();

   pGLSTATE->SetState(BLEND, false);
   pGLSTATE->SetState(LIGHTING, true);
}


void CMenuWrapper::LostRender()
{
   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   CGameInterfaceManager::GetInstance()->Render();

   char buffer[256];

   g_pFont->BeginRendering();
   CVector2f Pos;
   Color4f Color = {1.0f, 1.0f, 1.0f, 1.0f};
   Pos.x = -0.75f;
   Pos.y = 0.4f;

   g_pFont->Print("Mission Failure!", &Pos, &Color);

     Pos.x = -0.7f;
   Pos.y = 0.3f;

   g_pFont->Print("Mission Time: ", &Pos, &Color);

   int minutes = int(g_Player.m_TimeMissionEnded - g_Player.m_TimeMissionStarted) / 60;
   int seconds = int(g_Player.m_TimeMissionEnded - g_Player.m_TimeMissionStarted) % 60;

   if (seconds > 9)
      sprintf(buffer, "%d:%d", minutes, seconds);
   else
      sprintf(buffer, "%d:0%d", minutes, seconds);

   Pos.x = -0.4f;
   Pos.y = 0.3f;
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = -0.7f;
   Pos.y = 0.2f;
   g_pFont->Print("Enemies Destroyed", &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = 0.125f;
   g_pFont->Print("- Medium Tanks:" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = 0.125f;
   itoa(g_Player.m_EnemyMedTanksDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = 0.05f;
   g_pFont->Print("- Heavy Tanks:" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = 0.05f;
   itoa(g_Player.m_EnemyHvyTanksDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = -0.025f;
   g_pFont->Print("- Missile Tanks:" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = -0.025f;
   itoa(g_Player.m_EnemyMslTanksDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = -0.1f;
   g_pFont->Print("- Scouts" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = -0.1f;
   itoa(g_Player.m_EnemyScoutsDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = -0.175f;
   g_pFont->Print("- Kamikaze Units" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = -0.175f;
   itoa(g_Player.m_EnemyMeleeTanksDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = -0.65f;
   Pos.y = -0.25f;
   g_pFont->Print("- Bombers" , &Pos, &Color);

   Pos.x = -0.4f;
   Pos.y = -0.25f;
   itoa(g_Player.m_EnemyFightersDestroyed, buffer, 10);
   g_pFont->Print(buffer , &Pos, &Color);

   Pos.x = 0.0f;
   Pos.y = 0.3f;
   g_pFont->Print("Plasma Shots" , &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = 0.225f;
   g_pFont->Print("- Shots fired:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = 0.225f;
   itoa(g_Player.m_ShotsFiredPlasma, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = 0.150f;
   g_pFont->Print("- Shots hit:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = 0.150f;
   itoa(g_Player.m_HitsWithPlasma, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = 0.075f;
   g_pFont->Print("- Hit percentage:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = 0.075f;
   int percentage;
   if(g_Player.m_ShotsFiredPlasma == 0)
      percentage = 100;
   else
      percentage = int((float)g_Player.m_HitsWithPlasma / (float)g_Player.m_ShotsFiredPlasma * 100.0f);
   itoa(percentage, buffer, 10);
   if(percentage < 10)
   {
      char temp = buffer[0];
      buffer[0] = '0';
      buffer[1] = temp;
   }
   g_pFont->Print(buffer, &Pos, &Color);


   Pos.x = 0.0f;
   Pos.y = 0.0f;
   g_pFont->Print("Cluster Missiles" , &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = -0.075f;
   g_pFont->Print("- Shots fired:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = -0.075f;
   itoa(g_Player.m_ShotsFiredMissiles, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = -0.15f;
   g_pFont->Print("- Shots hit:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = -0.150f;
   itoa(g_Player.m_HitsWithMissiles, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.05f;
   Pos.y = -0.225f;
   g_pFont->Print("- Hit percentage:" , &Pos, &Color);

   Pos.x = 0.3f;
   Pos.y = -0.225f;
   if(g_Player.m_ShotsFiredMissiles == 0)
      percentage = 100;
   else
      percentage = int((float)g_Player.m_HitsWithMissiles / (float)g_Player.m_ShotsFiredMissiles * 100.0f);
   itoa(percentage, buffer, 10);
   if(percentage < 10)
   {
      char temp = buffer[0];
      buffer[0] = '0';
      buffer[1] = temp;
   }
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.5f;
   Pos.y = 0.3f;
   g_pFont->Print("Tracking Missiles" , &Pos, &Color);

   Pos.x = 0.55f;
   Pos.y = 0.225f;
   g_pFont->Print("- Shots fired:" , &Pos, &Color);

   Pos.x = 0.8f;
   Pos.y = 0.225f;
   itoa(g_Player.m_ShotsFiredTrkMissiles, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.55f;
   Pos.y = 0.150f;
   g_pFont->Print("- Shots hit:" , &Pos, &Color);

   Pos.x = 0.8f;
   Pos.y = 0.150f;
   itoa(g_Player.m_HitsWithTrkMissiles, buffer, 10);
   g_pFont->Print(buffer, &Pos, &Color);

   Pos.x = 0.55f;
   Pos.y = 0.075f;
   g_pFont->Print("- Hit percentage:" , &Pos, &Color);

   Pos.x = 0.8f;
   Pos.y = 0.075f;
   if(g_Player.m_ShotsFiredTrkMissiles == 0)
      percentage = 100;
   else
      percentage = int((float)g_Player.m_HitsWithTrkMissiles / (float)g_Player.m_ShotsFiredTrkMissiles * 100.0f);
   itoa(percentage, buffer, 10);
   if(percentage < 10)
   {
      char temp = buffer[0];
      buffer[0] = '0';
      buffer[1] = temp;
   }

	g_pFont->Print(buffer, &Pos, &Color);

	g_pFont->EndRendering();

	pGLSTATE->SetState(DEPTH_TEST, false);
   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	CGameInterfaceManager::GetInstance()->RenderMouse();

   pGLSTATE->SetState(BLEND, false);
   pGLSTATE->SetState(LIGHTING, true);
}

void CMenuWrapper::MenuRender()
{
    
   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   CVector2f vec;

   switch(m_CurrentMenu)
   {
      case MENU_MAIN:
      {
         CGameInterfaceManager::GetInstance()->Render();
      }
         break;

      case MENU_LEVEL:
      {
         CGameInterfaceManager::GetInstance()->Render();
      }
         break;

      case MENU_CONTROLS:
      {
         CGameInterfaceManager::GetInstance()->Render();
      }
         break;

      case MENU_CREDITS:
      {
         CGameInterfaceManager::GetInstance()->Render();
      }
         break;

      case MENU_VIDEO:
      {
         CGameInterfaceManager::GetInstance()->Render();
      }
         break;

      case MENU_SOUND:
      {
         CGameInterfaceManager::GetInstance()->Render();
      }
         break;

      case MENU_QUIT:
      {
         // JUST QUIT   
      }
         break;

      {
         CGameInterfaceManager::GetInstance()->Render();
      }
         break;
   }

   pGLSTATE->SetState(LIGHTING, false);
   pGLSTATE->SetState(BLEND, true);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      CGameInterfaceManager::GetInstance()->RenderMouse();

   pGLSTATE->SetState(BLEND, false);
   pGLSTATE->SetState(LIGHTING, true);
}

void CMenuWrapper::GetInput()
{
   switch(m_WrapperState)
   {
      case MAIN_MENU:
      {
         MenuGetInput();
      }
         break;

      case BRIEFING:
      {
         switch(GameMain.m_CurrentLevel)
         {
            case GameMain.Level1:
            {
               Briefing1GetInput();
            }
               break;

            case GameMain.Level2:
            {
               Briefing2GetInput();
            }
               break;

            case GameMain.Level3:
            {
               Briefing3GetInput();
            }
               break;
         }
      }
         break;

      case LEVEL_VICTORY:
      {
         VictoryGetInput();
      }
         break;

      case LEVEL_LOST:
      {
         LostGetInput();
      }
         break;

      default:
         break;
   }
}

void CMenuWrapper::Briefing1GetInput()
{
   CGameInterfaceManager::GetInstance()->GetInput();
}


void CMenuWrapper::Briefing2GetInput()
{
   CGameInterfaceManager::GetInstance()->GetInput();
}

void CMenuWrapper::Briefing3GetInput()
{
   CGameInterfaceManager::GetInstance()->GetInput();
}

void CMenuWrapper::VictoryGetInput()
{
   CGameInterfaceManager::GetInstance()->GetInput();
}

void CMenuWrapper::LostGetInput()
{
   CGameInterfaceManager::GetInstance()->GetInput();
}


void CMenuWrapper::MenuGetInput()
{
   CGameInterfaceManager::GetInstance()->GetInput();
}

void CMenuWrapper::Shutdown()
{
   m_MousePos = CGameInterfaceManager::GetInstance()->GetMousePos();
   CGameInterfaceManager::GetInstance()->Shutdown();
   g_MenuGraphics.Clear();
   delete g_pFont;
   delete g_pLargeFont;
   delete g_pSmallFont;
}


void CMenuWrapper::DeleteInstance()
{
   delete m_pInstance;
   m_pInstance = 0;
}