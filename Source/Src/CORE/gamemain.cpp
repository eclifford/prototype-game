/// \file gamemain.cpp
/// Main Game 
//////////////////////////////////////////////////////////////////////////

#include "core\gamemain.h"
#include "core\oglstatemanager.h"
#include "graphics\particlemanager.h"
#include "graphics\texturemanager.h"
#include "graphics\geometrymanager.h"
#include "Camera\Camera.h"
#include "graphics\ptgfile.h"
#include "graphics\pttfile.h"
#include "gamelogic\objectfactory.h"
#include "input\inputmanager.h"
#include "core\oglwindow.h"   
#include "collision\collisionsystem.h"
#include "gamelogic\medtank.h"
#include "graphics\drawshape.h"
#include "graphics\hud.h"
#include "math3d\intersection.h"
#include "graphics\texturefonts.h"
#include "sound\audiomanager.h"
#include "sound\audio.h"   
#include "level editor\level.h"
#include "level editor\editormain.h"
#include "gamelogic\mothership.h"
#include "graphics\bitmapfont.h"
#include "graphics\light.h"
#include "graphics\hudtext.h"
#include "graphics\color4f.h"
#include "graphics\fader.h"
#include "gamelogic\player.h"
#include "graphics\skybox.h"
#include "graphics\minimap.h"
#include "camera\frustum.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ Global Instances          /*/ 
/////////////////////////////////

CParticleManager     g_ParticleManager;
CBaseCamera*         g_Camera = 0;
CFirstPersonCamera   g_FirstPersonCamera;
CFollowCamera        g_FollowCamera;
CQuadtree            g_Quadtree;
CObjectFactory       g_ObjectFactory;
CTerrain*            g_pCurrLevel;
CPlayer              g_Player;
CMothership          g_MotherShip;
CCollisionSystem     g_CollisionSystem;
CHud                 g_Hud;
CGameObject*         g_CollisionAvoidanceList;
CTextureFonts        g_TextureFont;
CPlayerCamera        g_PlayerCamera;
CBitmapFont          g_Font;
CHudTextManager      g_HudTextManager;
CDirectionalLight    g_Sunlight;
CPositionalLight     g_IonLight;
CLookAtCamera			g_LookAtCamera;
CFlyByCamera			g_FlyByCamera;
CAudioManager g_AudioManager;

bool LoadedPlayerAndMothership = false;

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern COpenGLWindow		   *pGLWIN; 
extern COpenGLStateManager *pGLSTATE;
extern CTextureManager     g_TextureManager;
extern CFrustum            g_Frustum;
extern CGeometryManager    g_GeometryManager;
extern CEditorMain         EditorMain;
extern CAudioManager       g_AudioManager;
extern CFader					g_Fader;

/////////////////////////////////
/*/ Global Variables          /*/ 
/////////////////////////////////

extern float g_FrameTime;
extern float g_last_time;
static bool tempbool; 
unsigned int g_NumModels = 50;
unsigned int g_NumTextures = 120;
extern float g_fps;
static float level3time = 0.0f;

//////////////////////////////////////////////////////////////////////////
/// Constructor
/// 
/// Input:     void
///																								 
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CGameMain::CGameMain(void)
{
   m_pEditorLevel = 0;
   m_CurrentLevel = Level1;
   m_GodMode = false;
}

//////////////////////////////////////////////////////////////////////////
/// Initialize
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameMain::Initialize()
{
   tempbool = false;
   m_Lvl1SequenceFadeOut = false;
   tempboolboss = false;
   lvl3spawnonce = true;
   m_NumEnemiesAlive = 0;
   m_BuildingsDead = 0;
   m_NumConvoysHome = 0;
   m_BossAlive = true;

   m_CurLevel1Event = NONE;
   m_CurLevel3Event = LVL3WAVE_1;
   m_CurrentWave = WAVE_1;
   m_ConvoysDead = 0;

   m_CurrentCommunication = COM_NONE;
   m_CurrentConvoyCom = COM_CONVOYNONE;
   m_CurrentBuildingCom = COM_BUILDINGNONE;
   
   m_ObjList = NULL;
   g_CollisionAvoidanceList = NULL;
   m_CurrentGameState = GAME_RUNNING;

   g_AudioManager.SetAudioRatios(g_GameOptions.GetFXVol() * 2, g_GameOptions.GetMusicVol() * 2);

   // Initialize Modules
   g_TextureManager.Initialize(g_NumTextures);
   g_GeometryManager.Initialize(g_NumModels);
   g_ParticleManager.Initialize();
   g_AudioManager.Initialize();
   g_AudioManager.LoadGameSounds();
   g_Hud.Initialize();
   
   g_pCurrLevel = new CTerrain;
   m_pRenderList = new CRenderList;
	
   pGLSTATE->SetState(LIGHTING, true);
   pGLSTATE->SetState(CULL_FACE, true);	
	pGLSTATE->SetState(DEPTH_TEST, true);
		
   glAlphaFunc(GL_GREATER, 0);	
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	pGLSTATE->SetState(TEXTURE_2D, TRUE);
		
	glMatrixMode(GL_MODELVIEW);
				
	// enable vertex arrays and normal arrays
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_COLOR_ARRAY);			

   FloatRect OrthoRect;
   OrthoRect.Max.x = 1.0f;
   OrthoRect.Min.x = -1.0f;
   OrthoRect.Max.y = 1.0f;
   OrthoRect.Min.y = -1.0f;

   m_IncMessage = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_INCOMINGMSG], 10);

//   g_Font.Initialize("resources\\images\\verdana1024.tga", 16, 16, &OrthoRect);

   
   //OBJToPTG("resources\\models\\convoy.obj", "resources\\images\\supply.bmp",
   //         "resources\\models\\supply.ptg", true, 1.0f, 1.0f, 1.0f);
   /*
   OBJToPTG("resources\\models\\rock5.obj", "resources\\images\\rock1.bmp",
            "resources\\models\\rock5dark.ptg", true, 2.0f, 2.0f, 2.0f);

   OBJToPTG("resources\\models\\rock4.obj", "resources\\images\\rock1.bmp",
            "resources\\models\\rock4dark.ptg", true, 2.0f, 2.0f, 2.0f);
   
   OBJToPTG("resources\\models\\rock4.obj", "resources\\images\\rock2.bmp",
            "resources\\models\\rock4grey.ptg", true, 2.0f, 2.0f, 2.0f);

   OBJToPTG("resources\\models\\rock3.obj", "resources\\images\\rock1.bmp",
            "resources\\models\\rock3dark.ptg", true, 2.0f, 2.0f, 2.0f);
   
   OBJToPTG("resources\\models\\rock3.obj", "resources\\images\\rock2.bmp",
            "resources\\models\\rock3grey.ptg", true, 2.0f, 2.0f, 2.0f);
   */
  /* 
     OBJToPTG("resources\\models\\residential.obj", "resources\\images\\residential.bmp",
          "resources\\models\\residential.ptg", true, .5f, .5f, .5f);
       OBJToPTG("resources\\models\\business.obj", "resources\\images\\business.bmp",
          "resources\\models\\business.ptg", false, .5f, .5f, .5f);
   OBJToPTG("resources\\models\\industrial.obj", "resources\\images\\industrial.bmp",
            "resources\\models\\industrial.ptg", true, .5f, .5f, .5f);
   
   */


   /*
   CVector2f SunDir;
   SunDir.x = 1.0f;
   SunDir.y = 1.0f;
   HeightMapToPTT("resources\\levels\\height2.raw", "resources\\images\\grass11.bmp",
                  "resources\\levels\\testbig.ptt", 17, 32, 16, 4.0f, .4f, 4.0f, &SunDir, .5f, 1.0f,
                  .4f);
   */
   LoadObjects();
   //g_ObjectFactory.SetupObjects();

   CGameObject* pGameObject;
   switch(m_CurrentLevel)
   {
      case Level1:
      {
         g_ObjectFactory.SetupLevel1Factory();
         LoadLevel1();
         break;
      }
  
      case Level2:
      {
         g_ObjectFactory.SetupLevel2Factory();
         LoadLevel2();
         break;
      }

      case Level3:
      {
         g_ObjectFactory.SetupLevel3Factory();
         LoadLevel3();
         break;
      }

      case TestLevel:
      {
         // Create the mothership
         g_MotherShip.Setup();

         LoadLevelTest();
         break;
      }
      case EditorLevel:
      {
         m_pSkyBox = new CSkyBox("resources\\images\\mtnbk.tga", "resources\\images\\mtnft.tga",
                           "resources\\images\\mtnlf.tga", "resources\\images\\mtnrt.tga",
                           "resources\\images\\mtnup.tga", "resources\\images\\mtndn.tga",
                           20.0f);

         LoadLevel(EditorMain.m_pLevelFilePath, m_pRenderList, 0);
         
         m_Level1BGSound = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LEVEL1_BG], 255);
         m_Level1BGSound->PlayBackgroundLoop(100);

         g_Player.SetupPlayer();
         AddToCollisionAvoidanceList(&g_Player);

         SetupCamera();

         // set up and awaken all the game objects
         pGameObject = m_ObjList;

         while (pGameObject)
         {
            pGameObject->Setup();
            pGameObject = pGameObject->m_Next;
         }

         SetupCamera();
         g_Camera = &g_PlayerCamera;
         
         g_Player.m_IonLastFired = g_time;
         
         g_Player.m_TimeMissionStarted = g_time;
   
         g_MotherShip.Setup();

         break;
      }
      case Skirmish:
      {
         LoadSkirmish();
         break;
      } 
   }
   
   SetupLightingAndFog();
   SetupCamera();

   // set up the minimap 
   m_pMiniMap = new CMiniMap;
   float MiniMapRadius = 500.0f;
   m_pMiniMap->Initialize(MiniMapRadius);

   //EnableLockArrays();

   //     HeightMapToPTT("resources\\levels\\height2.raw", "resources\\images\\rock.tga",
   //                  "resources\\levels\\test.ptt", 17, 1.0f, .1f, 1.0f);

  
   //OBJToPTG("resources\\models\\mothership.obj", "resources\\images\\mothership.bmp",
   //         "resources\\models\\mothership.ptg", true, 1.0f, 1.0f, 1.0f);

   //OBJToPTG("resources\\models\\missilecrate.obj", "resources\\images\\RedCrate.bmp",
   //      "resources\\models\\missilecrate.ptg", false, 3.0f, 3.0f, 3.0f);

   //OBJToPTG("resources\\models\\missilecrate.obj", "resources\\images\\missilecrate.bmp",
   //      "resources\\models\\trkmissilecrate.ptg", false, 3.0f, 3.0f, 3.0f);
 /*
   OBJToPTG("resources\\models\\sphere.obj", "resources\\images\\plasmaupgrade.tga",
         "resources\\models\\plasmaupgrade.ptg", false, 4.0f, 4.0f, 4.0f);

   OBJToPTG("resources\\models\\sphere.obj", "resources\\images\\missileupgrade.tga",
         "resources\\models\\missileupgrade.ptg", false, 4.0f, 4.0f, 4.0f);

   OBJToPTG("resources\\models\\sphere.obj", "resources\\images\\trkmissileupgrade.tga",
         "resources\\models\\trkmissileupgrade.ptg", false, 4.0f, 4.0f, 4.0f);
         */

   //OBJToPTG("resources\\models\\residential.obj", "resources\\images\\mediumtank.bmp",
   //      "resources\\models\\residential.ptg", false, 1.0f, 1.0f, 1.0f);
   
   //CGameObject* NewObj = new CGameObject(m_T)
}

void CGameMain::LoadSkirmish()
{
   m_pSkyBox = new CSkyBox("resources\\images\\mtnbk.tga", "resources\\images\\mtnft.tga",
                           "resources\\images\\mtnlf.tga", "resources\\images\\mtnrt.tga",
                           "resources\\images\\mtnup.tga", "resources\\images\\mtndn.tga",
                           20.0f);

   m_NumEnemiesAlive = 0;

   LoadRandomLevel();
   
   m_Level1BGSound = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LEVEL1_BG], 255);
   m_Level1BGSound->PlayBackgroundLoop(100);

   g_Player.SetupPlayer();
      g_Player.m_IonLastFired = g_time;
   AddToCollisionAvoidanceList(&g_Player);

   Point3f Pos(0.0f, 0.0f, 0.0f);
   g_Player.SetPosition(&Pos, 0);

   SetupCamera();
   g_Camera = &g_PlayerCamera;

   g_Player.m_TimeMissionStarted = g_time;
   
   // Push the player into the object list
   AddToObjList(&g_Player);

   // Add the player to the quadtree
   g_Quadtree.AddObject(g_Player.GetRenderObj());
   g_Player.FollowTerrain();

   g_Player.MoveUp(5.0f, 0);

   g_MotherShip.Setup();
}

void CGameMain::LoadLevel1(void)
{
   m_pSkyBox = new CSkyBox("resources\\images\\mtnbk.tga", "resources\\images\\mtnft.tga",
                           "resources\\images\\mtnlf.tga", "resources\\images\\mtnrt.tga",
                           "resources\\images\\mtnup.tga", "resources\\images\\mtndn.tga", 20.0f);

   m_NumEnemiesAlive = 0;

   // Setup the terrain  
   LoadLevel("resources\\levels\\level1test.ptl", m_pRenderList, 0);

   // Create the level one bg sound
   m_Level1BGSound = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LEVEL1_BG], 255);
   m_Lvl1SequenceBG = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LVL1SEQUENCELOOP], 255);
   m_Lvl1SequenceBG->PlayBackgroundLoop(100);

   // Create the player
   g_Player.SetupPlayer();
   g_Player.m_TimeMissionStarted = g_time;
      g_Player.m_IonLastFired = g_time;
   AddToCollisionAvoidanceList(&g_Player);

   // Create the mothership
   g_MotherShip.Setup();
   //AddToObjList(&g_MotherShip);

   

   CGameObject* pGameObject;
   pGameObject = m_ObjList;

   while (pGameObject)
   {
      pGameObject->Setup();
      pGameObject = pGameObject->m_Next;
   }
   
   CConvoy * convoy = g_ObjectFactory.CreateConvoy();
   convoy->Setup();
   CVector3f ppos(591.0f, -50.0f, 972.0f);
   CObjFrame tempframe;
   tempframe.SetPosition(&ppos);
   convoy->SetFrame(&tempframe, 0);
   convoy->FollowTerrain();
   convoy->RotateLocalY(360, 0);
   
   CConvoy * convoy2 = g_ObjectFactory.CreateConvoy();
   convoy2->Setup();
   CVector3f ppos2(695.0f, -50.0f, 975.0f);
   tempframe.SetPosition(&ppos2);
   convoy2->SetFrame(&tempframe, 0);
   convoy2->FollowTerrain();
   convoy2->RotateLocalY(360, 0);
   
   CConvoy * convoy3 = g_ObjectFactory.CreateConvoy();
   convoy3->Setup();
   CVector3f ppos3(792.0f, -50.0f, 976.0f);
   tempframe.SetPosition(&ppos3);
   convoy3->SetFrame(&tempframe, 0);
   convoy3->FollowTerrain();
   convoy3->RotateLocalY(360, 0);

	SetupLevel1Sequence();
}

void CGameMain::LoadLevel2(void)
{
   m_pSkyBox = new CSkyBox("resources\\images\\mtnbk.tga", "resources\\images\\mtnft.tga",
                           "resources\\images\\mtnlf.tga", "resources\\images\\mtnrt.tga",
                           "resources\\images\\mtnup.tga", "resources\\images\\mtndn.tga",
                           20.0f);

   m_NumEnemiesAlive = 0;
   // Setup the terrain
   
   LoadLevel("resources\\levels\\level2.ptl", m_pRenderList, 0);
   
   // Create the level one bg sound
   m_Level1BGSound = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LEVEL1_BG], 255);
   m_Lvl1SequenceBG = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LVL1SEQUENCELOOP], 255);
   m_Lvl1SequenceBG->PlayBackgroundLoop(100);

   // Create the player
   g_Player.SetupPlayer();
   g_Player.m_TimeMissionStarted = g_time;
      g_Player.m_IonLastFired = g_time;
   AddToCollisionAvoidanceList(&g_Player);

   // Create the mothership
   g_MotherShip.Setup();

   CGameObject* pGameObject;
   pGameObject = m_ObjList;

   while (pGameObject)
   {
      pGameObject->Setup();
      pGameObject = pGameObject->m_Next;
   }

   SetupLevel2Sequence();
}

void CGameMain::LoadLevel3(void)
{
   m_pSkyBox = new CSkyBox("resources\\images\\mtnbk.tga", "resources\\images\\mtnft.tga",
                           "resources\\images\\mtnlf.tga", "resources\\images\\mtnrt.tga",
                           "resources\\images\\mtnup.tga", "resources\\images\\mtndn.tga",
                           20.0f);

   m_NumEnemiesAlive = 0;
   // Setup the terrain
   
   LoadLevel("resources\\levels\\level3.ptl", m_pRenderList, 0);
   
   // Create the level one bg sound
   m_Level1BGSound = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LEVEL1_BG], 255);
   m_Lvl1SequenceBG = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LVL1SEQUENCELOOP], 255);
   m_Lvl1SequenceBG->PlayBackgroundLoop(100);

   // Create the player
   g_Player.SetupPlayer();
   g_Player.m_TimeMissionStarted = g_time;
   g_Player.m_IonLastFired = g_time;
   AddToCollisionAvoidanceList(&g_Player);

   // Create the mothership
   g_MotherShip.Setup();

   CBoss * boss = g_ObjectFactory.CreateBoss();
   boss->Setup();
   CVector3f Pos(490.0f, 0.0f, -716.0f);
   boss->SetPosition(&Pos, 0);
   boss->FollowTerrain();

   CGameObject* pGameObject;
   pGameObject = m_ObjList;

   while (pGameObject)
   {
      pGameObject->Setup();
      pGameObject = pGameObject->m_Next;
   }

   SetupLevel3Sequence();
   level3time = 0.0f;
}

void CGameMain::LoadLevelTest(void)
{
   m_pSkyBox = new CSkyBox("resources\\images\\mtnbk.tga", "resources\\images\\mtnft.tga",
                           "resources\\images\\mtnlf.tga", "resources\\images\\mtnrt.tga",
                           "resources\\images\\mtnup.tga", "resources\\images\\mtndn.tga",
                           20.0f);

   m_NumEnemiesAlive = 0;
   LoadLevel("resources\\levels\\test.ptl", m_pRenderList, 0);

  // LoadRandomLevel();
   
   m_Level1BGSound = g_AudioManager.CreateSound(g_AudioManager.samples[SFX_LEVEL1_BG], 255);
   m_Level1BGSound->PlayBackgroundLoop(100);

   g_Player.SetupPlayer();
      g_Player.m_IonLastFired = g_time;
   AddToCollisionAvoidanceList(&g_Player);

   Point3f Pos(0.0f, 0.0f, 0.0f);
   g_Player.SetPosition(&Pos, 0);

   SetupCamera();
   g_Camera = &g_PlayerCamera;

   g_Player.m_TimeMissionStarted = g_time;
   
   // Push the player into the object list
   AddToObjList(&g_Player);

   // Add the player to the quadtree
   g_Quadtree.AddObject(g_Player.GetRenderObj());
   g_Player.FollowTerrain();

   g_Player.MoveUp(5.0f, 0);

   g_MotherShip.Setup();
}

void CGameMain::SetupLightingAndFog()
{   		
   float SceneAmbient[] = { 1.0, 1.0f, 1.0f, 1.0f };
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, SceneAmbient);

   Color4f Amb = {0.8f, 0.8f, 0.8f, 1.0f};
	Color4f Dif = {1.0f, 1.0f, 1.0f, 1.0f};
	Color4f Spec = {.1f, .1f, .1f, 1.0f};
 
   CVector3f Dir(-1.0f, 1.0f, -1.0f);

   g_Sunlight.Initialize(GL_LIGHT0, &Amb, &Dif, &Spec, &Dir);
   g_Sunlight.Enable();

   Amb.r = 0.0f;
   Amb.g = 0.0f;
   Amb.b = 1.0f;
   Dif.r = 0.0f;
   Dif.g = 0.0f;
   Dif.b = 1.0f;
   Spec.b = .5f;

   Point3f Pos(0.0f, 0.0f, 0.0f);
   g_IonLight.Initialize(GL_LIGHT1, &Amb, &Dif, &Spec, &Pos, 1.0f, 0.0f, 0.0f);
   
   pGLSTATE->SetState(FOG, true);
   
   glFogf(GL_FOG_MODE, GL_LINEAR);

   float pFogColor[] = { .7f, .6f, .3f, 1.0f };

   glFogf(GL_FOG_START, 300.0f);
   glFogf(GL_FOG_END, 500.0f);
   glFogfv(GL_FOG_COLOR, pFogColor);
}



void CGameMain::SetupCamera()
{
   CVector3f offset(0.0f, 5.0f, -20.0f);
   g_PlayerCamera.Setup(&g_Player, &offset);
}

//////////////////////////////////////////////////////////////////////////
/// Load all the game object models
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameMain::LoadObjects()
{
   // Create one instance of every class to then copy
   m_pPlasmaObject = new CPlasmaShot;
   m_pMissileObject = new CMissile;
   m_pTrackingMissileObject = new CTrackingMissile;
   m_pMedTankObject = new CMedTank;
   m_pMeleeTankObject = new CMeleeTank;
   m_pHeavyTankObject = new CHeavyTank;
   m_pShieldPowerUpObject = new CShieldPowerUp;
   m_pIonCannonObject = new CIonCannon;
   m_pTreeObject = new CTree;
   m_pTree2Object = new CTree2;
   m_pTree3Object = new CTree3;
   m_pMissilePackObject = new CMissilePack;
   m_pTrackingMissilePackObject = new CTrackingMissilePack;
   m_pScoutObject = new CScout;
   m_pBuildingObject = new CBuilding;
   m_pMissileUpgradeObject = new CMissileUpgrade;
   m_pPlasmaUpgradeObject = new CPlasmaUpgrade;
   m_pTrkMissileUpgradeObject = new CTrkMissileUpgrade;
   m_pConvoyObject = new CConvoy;
   m_pMissileTankObject = new CMissileTank;
   m_pSphereObject = new CSphere;
   m_pFighterObject = new CFighter;
   m_pIonFlareObject = new CIonFlare;
   m_pLaserObject = new CLaser;
   m_pFriendlyTurretObject = new CFriendlyTurret;
   m_pBuildingObject2 = new CBuilding2;
   m_pBuildingObject3 = new CBuilding3;
   m_pEnemyTurretObject = new CEnemyTurret;
   m_pBombObject = new CBomb;
   m_pRock1Object = new CRock1;
   m_pRock2Object = new CRock2;
   m_pRock3Object = new CRock3;
   m_pRock4Object = new CRock4;
   m_pRock5Object = new CRock5;
   m_pRock6Object = new CRock6;
   m_pBossObject = new CBoss;

   // Assign models to those objects
   AssignModelToObject(m_pTreeObject, "resources\\models\\tree1.ptg");
   AssignModelToObject(m_pTree2Object, "resources\\models\\tree2.ptg");
   AssignModelToObject(m_pTree3Object, "resources\\models\\tree3.ptg");
   AssignModelToObject(&g_Player, "resources\\models\\tank.ptg");
   AssignModelToObject(&g_MotherShip, "resources\\models\\mothership.ptg");
   AssignModelToObject(m_pPlasmaObject, "resources\\models\\plasmashot.ptg");
   AssignModelToObject(m_pMissileObject, "resources\\models\\missile.ptg");
   AssignModelToObject(m_pMedTankObject, "resources\\models\\medtank.ptg");   
   AssignModelToObject(m_pMeleeTankObject, "resources\\models\\kamikaze.ptg"); 
   AssignModelToObject(m_pShieldPowerUpObject, "resources\\models\\shldpowerup.ptg");
   AssignModelToObject(m_pIonCannonObject, "resources\\models\\ioncannon.ptg");
   AssignModelToObject(m_pMissilePackObject, "resources\\models\\missilecrate.ptg");
   AssignModelToObject(m_pTrackingMissilePackObject, "resources\\models\\trkmissilecrate.ptg");
   AssignModelToObject(m_pTrackingMissileObject, "resources\\models\\trkmissile.ptg");
   AssignModelToObject(m_pScoutObject, "resources\\models\\scouttank.ptg");
   AssignModelToObject(m_pBuildingObject, "resources\\models\\residential.ptg");
   AssignModelToObject(m_pHeavyTankObject, "resources\\models\\heavytank.ptg");
   AssignModelToObject(m_pMissileUpgradeObject, "resources\\models\\clustermissilepowerup.ptg");
   AssignModelToObject(m_pPlasmaUpgradeObject, "resources\\models\\plasmapowerup.ptg");
   AssignModelToObject(m_pTrkMissileUpgradeObject, "resources\\models\\trkmissilepowerup.ptg");
   AssignModelToObject(m_pConvoyObject, "resources\\models\\supply.ptg");   
   AssignModelToObject(m_pMissileTankObject, "resources\\models\\missiletank.ptg");
   AssignModelToObject(m_pSphereObject, "resources\\models\\sphere.ptg");
   AssignModelToObject(m_pFighterObject, "resources\\models\\flyer.ptg");
   AssignModelToObject(m_pIonFlareObject, "resources\\models\\ionflare.ptg");
   AssignModelToObject(m_pLaserObject, "resources\\models\\laser.ptg");
   AssignModelToObject(m_pFriendlyTurretObject, "resources\\models\\humanturret.ptg");
   AssignModelToObject(m_pBuildingObject2, "resources\\models\\business.ptg");
   AssignModelToObject(m_pBuildingObject3, "resources\\models\\industrial.ptg");
   AssignModelToObject(m_pEnemyTurretObject, "resources\\models\\enemyturret.ptg");
   AssignModelToObject(m_pBombObject, "resources\\models\\bomb.ptg");
   AssignModelToObject(m_pRock1Object, "resources\\models\\rocksmalldark.ptg");
   AssignModelToObject(m_pRock2Object, "resources\\models\\rocktalldark.ptg");
   AssignModelToObject(m_pRock3Object, "resources\\models\\rockbigdark.ptg");
   AssignModelToObject(m_pRock4Object, "resources\\models\\rocksmallgrey.ptg");
   AssignModelToObject(m_pRock5Object, "resources\\models\\rocktallgrey.ptg");
   AssignModelToObject(m_pRock6Object, "resources\\models\\rockbiggrey.ptg");
   AssignModelToObject(m_pBossObject, "resources\\models\\boss.ptg");
}

//////////////////////////////////////////////////////////////////////////
/// Load a model and assign it to an object
/// 
/// Input:     CGameObject *obj - The object to attach a model to
///            char *fn - Filepath of model to load
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameMain::AssignModelToObject(CGameObject *obj, char * fn)
{
   unsigned int GeometryIndex;
   int TextureIndex;
   unsigned int NumParts;
   Sphere BoundSphere;
   CVector3f* pTranslations;
   AABB Box;

   g_GeometryManager.LoadPTGFile(fn, &BoundSphere, &Box, &GeometryIndex, &NumParts,
                               &pTranslations, &TextureIndex); 

   obj->Initialize(GeometryIndex, NumParts, pTranslations, TextureIndex, &BoundSphere,
                    &Box, &g_Quadtree);

    delete [] pTranslations;
}

//////////////////////////////////////////////////////////////////////////
/// Add an object to the gameobject list
/// 
/// Input:     CGameObject *obj - The object to load
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameMain::AddToObjList(CGameObject *obj)
{
	if (m_ObjList)
		m_ObjList->m_Prev = obj;
	obj->m_Next = m_ObjList;
	obj->m_Prev = 0;
	m_ObjList = obj;
}

//////////////////////////////////////////////////////////////////////////
/// Remove an object from the gameobject list
/// 
/// Input:     CGameObject *obj - The object to remove
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameMain::RemoveFromObjList(CGameObject *obj)
{
	if (obj->m_Prev)
		obj->m_Prev->m_Next = obj->m_Next;
	if (obj->m_Next)
		obj->m_Next->m_Prev = obj->m_Prev;
	if (m_ObjList == obj)
		m_ObjList = obj->m_Next;
}


//////////////////////////////////////////////////////////////////////////
/// Add an object to the enemy object list
/// 
/// Input:     CEnemy* obj - The object to load
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameMain::AddToCollisionAvoidanceList(CGameObject* obj)
{
   int a = 0;

   if (g_CollisionAvoidanceList)
   {
      CGameObject* temp = g_CollisionAvoidanceList;

      do {
         
         if (temp == obj)
         {
            a = 0;
         }
         temp = temp->m_CollisionAvoidanceListNext;
      } while(temp != NULL);
   }
	if (g_CollisionAvoidanceList)
		g_CollisionAvoidanceList->m_CollisionAvoidanceListPrev = obj;
	obj->m_CollisionAvoidanceListNext = g_CollisionAvoidanceList;
	obj->m_CollisionAvoidanceListPrev = 0;
	g_CollisionAvoidanceList = obj;

}

//////////////////////////////////////////////////////////////////////////
/// Remove an object from the enemy object list
/// 
/// Input:     CGEnemy* obj - The object to remove
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameMain::RemoveFromCollisionAvoidanceList(CGameObject* obj)
{
 	if (obj->m_CollisionAvoidanceListPrev)
		obj->m_CollisionAvoidanceListPrev->m_CollisionAvoidanceListNext = obj->m_CollisionAvoidanceListNext;
	if (obj->m_CollisionAvoidanceListNext)
		obj->m_CollisionAvoidanceListNext->m_CollisionAvoidanceListPrev = obj->m_CollisionAvoidanceListPrev;
	if (g_CollisionAvoidanceList == obj)
		g_CollisionAvoidanceList = obj->m_CollisionAvoidanceListNext;

   obj->m_CollisionAvoidanceListPrev = NULL;
   obj->m_CollisionAvoidanceListNext = NULL;

}

//////////////////////////////////////////////////////////////////////////
/// Main loop for our game
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CGameMain::Main()
{
   switch(m_CurrentGameState)
   {
      case GAME_RUNNING:
      {
         GetInput();
         Update();
         Render();

         return NO_CHANGE;
      }
      break;

      case GAME_PAUSED:
      {
         pGLWIN->m_StartTime += g_FrameTime;
         GetInput();
         Render();
      }
      break;

      case GOTO_MENU:
      {
         return MENU;
      }
      break;

      case GAME_EXIT:
      {
         return EXIT_PROGRAM;
      }
      break;

      case MISSION_WON:
      {
         GetInput();
         Update();
         Render();
      }
      break;

      case MISSION_LOST:
      {
         GetInput();
         Update();
         Render();
      }
      break;

      case QUIT_SCREEN:
      {
         GetInput();
         Render();
      }
      break;

      case GOTO_VICTORYSCREEN:
      {
         if(m_CurrentLevel != EditorLevel)
         {
            m_CurrentLevel++;
            g_Player.m_TimeMissionEnded = g_time;
            return WON;
         }
         else
            return EDITOR;
      }

      case GOTO_LOSTSCREEN:
      {
         if(m_CurrentLevel != EditorLevel)
         {
            g_Player.m_TimeMissionEnded = g_time;
            return LOST;
         }
         else
            return EDITOR;
      }
   }

   return NO_CHANGE;
}


//////////////////////////////////////////////////////////////////////////
/// Get Input from our Input Manager
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameMain::GetInput()
{
	if (m_InSequence)
		return;

   // Poll the keyboard
   CInputManager::GetInstance()->PollKeyboard();
   CInputManager::GetInstance()->PollMouse();

   switch(m_CurrentGameState)
   {
      case GAME_RUNNING:
      {
         g_Player.GetPlayerInput();

         if (CInputManager::GetInstance()->GetKey(DIK_DOWNARROW))
         {
            g_FirstPersonCamera.MoveForward(-1.0f);
         }

         if (CInputManager::GetInstance()->GetKey(DIK_UPARROW))
         {
            g_FirstPersonCamera.MoveForward(1.0f);
         }

         if (CInputManager::GetInstance()->GetKey(DIK_LEFTARROW))
         {
            g_FirstPersonCamera.MoveRight(-1.0f);
         }

         if (CInputManager::GetInstance()->GetKey(DIK_RIGHTARROW))
         {
            g_FirstPersonCamera.MoveRight(1.0f);
         }
   
         if (CInputManager::GetInstance()->GetKey(DIK_PRIOR))
         {
            g_FirstPersonCamera.TurnUp(.05f);
         }
   
         if (CInputManager::GetInstance()->GetKey(DIK_NEXT))
         {
            g_FirstPersonCamera.TurnUp(-.05f);
         }

         if (CInputManager::GetInstance()->GetKey(DIK_INSERT))
         {
            g_FirstPersonCamera.TurnRight(-.05f);
         }

         if (CInputManager::GetInstance()->GetKey(DIK_DELETE))
         {
            g_FirstPersonCamera.TurnRight(.05f);
         }

         /*
         if (CInputManager::GetInstance()->GetKey(DIK_F5))
         {
            SetupCamera();
				g_Camera = &g_PlayerCamera;
         }
         if (CInputManager::GetInstance()->GetKey(DIK_F6))
         {
            g_Camera = &g_FirstPersonCamera;
         }*/

         if (CInputManager::GetInstance()->GetKey(DIK_P))
         {
            m_CurrentGameState = GAME_PAUSED;
         }

         if (CInputManager::GetInstance()->GetKey(DIK_G))
            m_GodMode = true;
         else if (CInputManager::GetInstance()->GetKey(DIK_H))
            m_GodMode = false;

         if(CInputManager::GetInstance()->GetKey(DIK_ESCAPE))
         {
            m_CurrentGameState = QUIT_SCREEN;
         }
      }
      break;
     
      case MISSION_WON:
      {
		 if(!m_InSequence)
			 g_Player.GetPlayerInput();

         if(CInputManager::GetInstance()->GetKey(DIK_RETURN))
         {
            m_CurrentGameState = GOTO_VICTORYSCREEN;
         }     
      }
      break;

      case MISSION_LOST:
      {
         if(CInputManager::GetInstance()->GetKey(DIK_RETURN))
         {
            m_CurrentGameState = GOTO_LOSTSCREEN;
         }     
      }
      break;

      case QUIT_SCREEN:
      {
         if(CInputManager::GetInstance()->GetKey(DIK_Y))
         {
            m_CurrentGameState = GOTO_LOSTSCREEN;
         }  
         if(CInputManager::GetInstance()->GetKey(DIK_N))
         {
            m_CurrentGameState = GAME_RUNNING;
         } 
      }
      break;

      case GAME_PAUSED:
      {
         if(CInputManager::GetInstance()->GetKey(DIK_Q))
         {
            m_CurrentGameState = GAME_RUNNING;
         }  
      }
      break;
  }
}

//////////////////////////////////////////////////////////////////////////
/// Render all objects in our world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameMain::Render()
{	
   //CGameObject *obj = m_ObjList;
   //OBB *box;
   //static bool BoundingBox = false;

   glPushMatrix();
	   g_Camera->ApplyTransform(); 
	   g_Sunlight.Update();
      g_IonLight.Update();     
      m_pSkyBox->Render();
      g_pCurrLevel->BeginRendering();      
	   g_Quadtree.CreateVisibleList();
	   g_pCurrLevel->ResetPatchVisibility();
      g_pCurrLevel->ResetStates();
	   m_pRenderList->Render();
      g_ParticleManager.RenderSystem();
      /*
      if (CInputManager::GetInstance()->GetKey(DIK_LCONTROL))
      {
         float timetocheck = 10;
         float timecounter = 1.0f;
         static float curtime = 0.0f;
   
         curtime += timecounter;
         if(curtime >= timetocheck)
         {
            curtime = 0.0f;
            if(BoundingBox)
            {
               BoundingBox = false;
            }
 
            else
            {
               BoundingBox = true;
            }
         }
      }

      if(BoundingBox)
      {
         glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT |
                         GL_POINT_BIT);
         glEnable(GL_BLEND);
         glDisable(GL_LIGHTING);
         glDisable(GL_TEXTURE_2D);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		   glColor4f(1.0f, 0.0f, 0.0f, .3f);
         
         while (obj)
	      {
		      box = obj->GetOBB();
            DrawOBB(box);
            obj = obj->m_Next;
	      }

         glPopAttrib();
      }

      if(GetAsyncKeyState(VK_TAB))
      {
         glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT |
                         GL_POINT_BIT);
         glEnable(GL_BLEND);
         glDisable(GL_LIGHTING);
         glDisable(GL_TEXTURE_2D);
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		   glColor4f(1.0f, 0.0f, 0.0f, .3f);

         obj = m_ObjList;
         while (obj)
	      {
            DrawSphere(obj->GetSphere(), 10, 10);
            obj = obj->m_Next;
	      }

         glPopAttrib();
      }
      */

      /*
      CObjFrame ShotFrame;
      g_Player.FindShotPosition(&ShotFrame);
      Ray ShotRay;
      ShotFrame.GetForward(&ShotRay.Direction);
      ShotFrame.GetPosition(&ShotRay.Origin);

      obj = m_ObjList;
      bool Hit = false;
      while (obj)
	   {

		   if (obj->GetType() != OBJ_PLAYER && RayToSphere(&ShotRay, obj->GetSphere()))
            Hit = true;   
		   obj = obj->m_Next;
	   }      
      

      //glPushAttrib(GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_POINT_BIT);
      pGLSTATE->SetState(BLEND, true);
      pGLSTATE->SetState(DEPTH_TEST, false);
      pGLSTATE->SetState(LIGHTING, false);
      pGLSTATE->SetState(TEXTURE_2D, false);
      //glLineWidth(2.0f);
      glPointSize(5.0f);

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      
      if (Hit)
         glColor4f(0.0f, 1.0f, 0.0f, .5f);
      else
         glColor4f(1.0f, 0.0f, 0.0f, .5f);

      float Magnitude = 100.0f;
      glBegin(GL_POINTS);

      glVertex3f(ShotRay.Origin.x + ShotRay.Direction.x * Magnitude,
                 ShotRay.Origin.y + ShotRay.Direction.y * Magnitude,
                 ShotRay.Origin.z + ShotRay.Direction.z * Magnitude);
      glEnd();

      Magnitude = 200.0f;
      glBegin(GL_POINTS);

      glVertex3f(ShotRay.Origin.x + ShotRay.Direction.x * Magnitude,
                 ShotRay.Origin.y + ShotRay.Direction.y * Magnitude,
                 ShotRay.Origin.z + ShotRay.Direction.z * Magnitude);
      glEnd();

      Magnitude = 300.0f;
      glBegin(GL_POINTS);

      glVertex3f(ShotRay.Origin.x + ShotRay.Direction.x * Magnitude,
                 ShotRay.Origin.y + ShotRay.Direction.y * Magnitude,
                 ShotRay.Origin.z + ShotRay.Direction.z * Magnitude);
      glEnd();
      DrawRay(&ShotRay, 500);
      pGLSTATE->SetState(DEPTH_TEST, true);
      pGLSTATE->SetState(LIGHTING, true);
      pGLSTATE->SetState(TEXTURE_2D, true);
 
      //glPopAttrib();
      */
		g_Hud.RenderHud();  

	if (!m_InSequence)
	{
		m_pMiniMap->Render();

	}
   glPopMatrix();
	g_Fader.Render();
}

void CGameMain::SetupLevel1Sequence(void)
{
	CGameObject *obj = m_ObjList;

	while (obj)
	{
		obj->GetRenderObj()->Update();
		obj = obj->m_Next;
	}

	g_Player.RemoveFromQuadtree();
	g_Player.m_pMissilePods->RemoveFromQuadtree();
	
	CVector3f pos;

	// Setup mother ship control frames
	CObjFrame frames[4];

	pos.x = -135.0f;
	pos.y = 53.0f;
	pos.z = 935.0f;
	frames[0].SetPosition(&pos);
	frames[0].RotateLocalY(PI * 0.75f);

	pos.x = 296.0f;
	pos.y = 53.0f;
	pos.z = 677.0f;
	frames[1].SetPosition(&pos);
	frames[1].RotateLocalY(PI * 0.75f);
	frames[1].RotateLocalZ(PI / 6.0f);

	pos.x = 947.0f;
	pos.y = 100.0f;
	pos.z = 915.0f;
	frames[2].SetPosition(&pos);
	frames[2].RotateLocalY(PI * 0.25f);

	pos.x = 947.0f;
	pos.y = 57.5f;
	pos.z = 915.0f;
	frames[3].SetPosition(&pos);
	frames[3].RotateLocalY(PI / 12.0f * 13.0f);

	g_MotherShip.SetupDropOff(4, frames, 15.0f);
	g_LookAtCamera.SetTarget(&g_MotherShip);

	pos.x = 200.0f;
	pos.y = 85.0f;
	pos.z = 550.0f;
	g_LookAtCamera.MoveTo(&pos);
	g_Camera = &g_LookAtCamera;

	m_LookAtOffset.x = -999999.0f;

	g_Fader.SetFadeTime(6.0f);

	m_InSequence = true;
	m_FirstTimeSetup = true;
	m_DropOffStage = FLY;
}

void CGameMain::UpdateLevel1Sequence(void)
{
	CVector3f pos1, pos2;

	if (m_FirstTimeSetup)
	{
		g_Fader.Start();
		m_FirstTimeSetup = false;
	}

	g_MotherShip.Update();
   float t = g_time - g_MotherShip.GetDropStart();

   static bool temp1 = false;
   if(t >= 2.0f && temp1 == false)
   {
      m_CurrentCommunication = COM_LVL1INTRO1;
      temp1 = true;
   }

   static bool temp2 = false;
   if(t >= 6.0f && temp2 == false)
   {
      m_CurrentCommunication = COM_LVL1INTRO2;
      temp2 = true;
   }

   static bool temp3 = false;
   if(t >= 10.0f && temp3 == false)
   {
      m_CurrentCommunication = COM_LVL1INTRO3;
      temp3 = true;
   }

	if (t >= 5.5f && t <= 15.0f)
	{
		g_MotherShip.GetPosition(&pos1, 0);

		if (m_LookAtOffset.x == -999999.0f)
		{
			g_LookAtCamera.GetPosition(&pos2);
			Vec3fSubtract(&m_LookAtOffset, &pos2, &pos1);
		}
		else
		{
			Vec3fAdd(&pos2, &pos1, &m_LookAtOffset);
			g_LookAtCamera.MoveTo(&pos2);
		}
	}
	else if (t >= 16.0f && t <= 21.0f)
	{
		if (m_DropOffStage == FLY)
		{
			CObjFrame mframe = *g_MotherShip.GetFrame(0);
			mframe.MoveForward(-50.0f);
			mframe.MoveUp(-15.0f);
			g_Player.SetFrame(&mframe, 0);
			SetupCamera();
			g_Player.Update();

			// Setup pan
			CObjFrame frames[3];

			g_LookAtCamera.GetFrame(&frames[0]);

			pos1.x = 800.0f;
			pos1.y = 100.0f;
			pos1.z = 1030.0f;
			frames[1].SetPosition(&pos1);
			frames[1].RotateLocalY(PI * 0.5f);
			frames[1].RotateLocalX(PI / 12.0f);

			g_PlayerCamera.GetFrame(&frames[2]);

			g_FlyByCamera.SetUp(3, frames, 5.0f);
			g_Camera = &g_FlyByCamera;
			m_DropOffStage = PAN;
		}
	}
	else if (t > 22.0f)
	{
		g_Camera = &g_PlayerCamera;
		g_MotherShip.DropOffDone();
		m_InSequence = false;
      m_Lvl1SequenceFadeOut = true;
      temp1 = false;
      temp2 = false;
      temp3 = false;
	}

	if (g_Fader.IsDone())
		g_Fader.Stop();
}

void CGameMain::SetupLevel2Sequence(void)
{
	CGameObject *obj = m_ObjList;

	while (obj)
	{
		obj->GetRenderObj()->Update();
		obj = obj->m_Next;
	}

   SetupCamera();

	CObjFrame frames[4];
	CVector3f pos(-2.35f, -31.8f, -132.54f);

	frames[0].SetPosition(&pos);
	frames[0].RotateLocalY(PI * 3.0f / 4.0f);
	frames[1] = frames[0];

	frames[2].SetPosition(&pos);
	frames[2].RotateLocalY(PI / 4.0f);
	frames[3] = frames[2];

	g_FlyByCamera.SetUp(4, frames, 10.0f);

   g_Camera = &g_FlyByCamera;
	m_DropOffStage = 0;
	m_InSequence = true;
	m_FirstTimeSetup = true;
}

void CGameMain::UpdateLevel2Sequence(void)
{
	if (m_FirstTimeSetup)
	{
		g_Fader.Start();
		m_FirstTimeSetup = false;
	}
   
   float t = g_FlyByCamera.GetElapsedTime();
   static float texttime = 0.0f;
   texttime += g_FrameTime;

   /*
   static bool temp11 = false;
   if(texttime >= 10.0f && temp11 == false)
   {
      m_CurrentBuildingCom = COM_LVL2INTRO1;
      temp11 = true;
   }

   static bool temp22 = false;
   if(texttime >= 22.0f && temp22 == false)
   {
      m_CurrentBuildingCom = COM_LVL2INTRO2;
      temp22 = true;
   }

   static bool temp33 = false;
   if(texttime >= 33.0f && temp33 == false)
   {
      m_CurrentBuildingCom = COM_LVL2INTRO3;
      temp33 = true;
   }
   */
   
	if (m_DropOffStage == 0 && t >= 8.0f && g_Fader.IsDone())
	{
		g_Fader.SetFadeTime(3.0f);
		g_Fader.SetFadeDirection(false);
		g_Fader.Start();
	}

	else if (m_DropOffStage == 1 && t >= 8.0f && g_Fader.IsDone())
	{
		g_Fader.SetFadeTime(3.0f);
		g_Fader.SetFadeDirection(false);
		g_Fader.Start();
	}

	if (m_DropOffStage == 0 && t > 11.0f)
   {
		CObjFrame frames[4];
		CVector3f pos(-409.27f, -26.77f, -255.44f);

		frames[0].SetPosition(&pos);
		frames[0].RotateLocalY(PI * 5.0f / 4.0f);
		frames[1] = frames[0];

		frames[2].SetPosition(&pos);
		frames[2].RotateLocalY(PI * 7.0f / 4.0f);
		frames[3] = frames[2];

		g_FlyByCamera.SetUp(4, frames, 10.0f);
		g_Player.m_TimeMissionStarted = g_time - 11.0f;
		g_Fader.SetFadeDirection(true);
		g_Fader.Start();
		m_DropOffStage = 1;
	}
	else if (m_DropOffStage == 1 && t > 10.0f)
	{
	    SpawnUnits(OBJ_SCOUT, 15, ZONE_1);
        SpawnUnits(OBJ_SCOUT, 15, ZONE_1);
        SpawnUnits(OBJ_MEDTANK, 12, ZONE_1);
        SpawnUnits(OBJ_MEDTANK, 12, ZONE_1);

		CObjFrame frames[11];
		CVector3f pos(-186.0f, -20.0f, -90.0f);

		frames[0].SetPosition(&pos);
		frames[1] = frames[0];

		pos.z = 300.0f;
		frames[2].SetPosition(&pos);
		frames[3] = frames[2];

		frames[4].SetPosition(&pos);
		frames[4].RotateLocalY(PI / 6.0f);
		frames[5] = frames[4];

		frames[6].SetPosition(&pos);
		frames[6].RotateLocalY(-PI / 6.0f);
		frames[7] = frames[6];

		frames[8].SetPosition(&pos);
		frames[9] = frames[8];

		g_PlayerCamera.GetFrame(&frames[10]);

		g_FlyByCamera.SetUp(11, frames, 11.0f);

		g_Fader.SetFadeTime(2.0f);
		g_Fader.SetFadeDirection(true);
		g_Fader.Start();
		m_DropOffStage = 2;
	}
	else if (m_DropOffStage == 2)
	{
		if (t > 11.5f)
		{
			g_Camera = &g_PlayerCamera;
			m_InSequence = false;
		}
	}

	if (g_Fader.IsDone())
		g_Fader.Stop();
}

void CGameMain::SetupLevel3Sequence(void)
{
	CGameObject *obj = m_ObjList;

	while (obj)
	{
		obj->GetRenderObj()->Update();
		obj = obj->m_Next;
	}

	g_Player.RemoveFromQuadtree();
	g_Player.m_pMissilePods->RemoveFromQuadtree();
	
	CVector3f pos;
	CObjFrame frames[4];

	pos.x = 700.0f;
	pos.y = 69.0f;
	pos.z = 529.0f;
	frames[0].SetPosition(&pos);
	frames[0].RotateLocalY(PI);

	pos.x = 533.0f;
	pos.y = 4.0f;
	pos.z = 157.0f;
	frames[1].SetPosition(&pos);
	frames[1].RotateLocalY(PI);
	frames[1].RotateLocalZ(PI / 9.0f);

	pos.x = 707.0f;
	pos.y = 54.0f;
	pos.z = -169.0f;
	frames[2].SetPosition(&pos);
	frames[2].RotateLocalY(PI);
	frames[2].RotateLocalZ(-PI / 12.0f);

	pos.x = 490.0f;
	pos.y = -20.0f;
	pos.z = -650.0f;
	frames[3].SetPosition(&pos);
	frames[3].RotateLocalY(PI);

	g_FlyByCamera.SetUp(4, frames, 10.0f);

   g_Camera = &g_FlyByCamera;
	m_DropOffStage = 0;
	m_InSequence = true;
	m_FirstTimeSetup = true;
}

void CGameMain::UpdateLevel3Sequence(void)
{
	if (m_FirstTimeSetup)
	{
		g_Fader.SetFadeTime(3.0f);
		g_Fader.Start();
		m_FirstTimeSetup = false;
	}

	if (m_DropOffStage == 0 && g_FlyByCamera.GetElapsedTime() >= 12.0f && g_Fader.IsDone())
	{
		g_Fader.SetFadeTime(2.0f);
		g_Fader.SetFadeDirection(false);
		g_Fader.Start();
	}

	g_MotherShip.Update();
	if (m_DropOffStage == 0 && g_FlyByCamera.GetElapsedTime() >= 14.0f)
	{
		CVector3f pos;
		CObjFrame frames[2];

		pos.x = -274.0f;
		pos.y = 99.0f;
		pos.z = 850.0f;
		frames[0].SetPosition(&pos);
		frames[0].RotateLocalY(PI * 7.0f / 12.0f);
		frames[0].RotateLocalX(-PI / 9.0f);

		pos.x = -274.0f;
		pos.y = 40.0f;
		pos.z = 850.0f;
		frames[1].SetPosition(&pos);
		frames[1].RotateLocalY(PI * 7.0f / 12.0f);

		g_MotherShip.SetupDropOff(2, frames, 3.0f);
		g_LookAtCamera.SetTarget(&g_MotherShip);

		pos.x = -16.0f;
		pos.y = 44.0f;
		pos.z = 771.0f;
		g_LookAtCamera.MoveTo(&pos);
		g_Camera = &g_LookAtCamera;

		g_Fader.SetFadeTime(3.0f);
		g_Fader.SetFadeDirection(true);
		g_Fader.Start();

		m_DropOffStage = 1;
	}
	else if (m_DropOffStage == 1 && g_time - g_MotherShip.GetDropStart() >= 4.0f)
	{
		CObjFrame mframe = *g_MotherShip.GetFrame(0);
		mframe.MoveForward(-50.0f);
		mframe.MoveUp(-15.0f);
		g_Player.SetFrame(&mframe, 0);

		g_Player.Update();
		g_MotherShip.DropOffDone();
		m_DropOffStage = 2;
	}
	else if (m_DropOffStage == 2)
	{
		if (g_time - g_MotherShip.GetDropStart() >= 7.0f)
		{
			m_DropOffStage = 3;

			CObjFrame frames[2];

			g_LookAtCamera.GetFrame(&frames[0]);
	
			SetupCamera();
			g_PlayerCamera.GetFrame(&frames[1]);

			g_FlyByCamera.SetUp(2, frames, 3.0f);
			g_Camera = &g_FlyByCamera;
		}
		else 
		{
			g_Player.Update();
		}
	}
	else if (m_DropOffStage == 3 && g_time - g_MotherShip.GetDropStart() >= 12.0f)
	{
		g_Camera = &g_PlayerCamera;
		m_InSequence = false;
	}

	if (g_Fader.IsDone())
		g_Fader.Stop();
}

//////////////////////////////////////////////////////////////////////////
/// Update all objects in our world
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

unsigned int CGameMain::Update()
{
   g_AudioManager.Update();

   g_Frustum.TransformFrustum(g_Camera);
   g_ParticleManager.UpdateSystem();
   ProccessCommunications();
    
	if (m_InSequence)
	{
		switch (m_CurrentLevel)
		{
		case Level1:
			UpdateLevel1Sequence();
			break;
		case Level2:
			UpdateLevel2Sequence();
			break;
		case Level3:
			UpdateLevel3Sequence();
			break;
		}
		//return 0;
	}

   CGameObject *obj = m_ObjList, *temp;
   if(!m_InSequence)
   {
		switch(m_CurrentLevel)
		{
		  case Level1:
			 Level1Triggers();
		  break;

		  case Level2:
			 Level2Triggers();
		  break;

		  case Level3:
			 Level3Triggers();
		  break;
		  
		  case Skirmish:
			  SkirmishTriggers();
		  break;
		}
   }

	// If object is active, update then go to next
	while ((obj && !m_InSequence) || (obj && m_CurrentLevel == Level2))
	{
		if (obj->IsActive())
		{
			obj->Update();
			obj = obj->m_Next;
		}
		else
		{
         if(obj->GetType() != OBJ_PLAYER)
         {
			   // Object is dead, remove from list and return to factory
			   temp = obj->m_Next;
			   RemoveFromObjList(obj);
            if (obj->m_CollisionAvoidanceListNext != NULL || obj->m_CollisionAvoidanceListPrev != NULL ||
               g_CollisionAvoidanceList == obj)
               RemoveFromCollisionAvoidanceList(obj);
			   g_ObjectFactory.ReturnAndNoRefund(obj);
			   obj = temp;
         }

         // SPECIAL CASE FOR WHEN PLAYER DIES
         else
         {
            temp = obj->m_Next;
			   RemoveFromObjList(obj);
            RemoveFromCollisionAvoidanceList(obj);
			   obj = temp;   
         }
		}
	}

	// TO DO:
	// Check collisions here
   g_CollisionSystem.CheckCollisions(m_ObjList);
   
   //m_pMiniMap->Update();

	if (g_MotherShip.InOperation() && !m_InSequence)
		g_MotherShip.Update();

	return 0;	
}

void CGameMain::SkirmishTriggers()
{
	static bool temp = true;
	if(m_NumEnemiesAlive <= 10)
	{
		if(temp)
		{
			m_CurrentGameState = MISSION_LOST;
			g_Player.m_TimeMissionEnded = g_time;
			temp = false;
		}
	}
}

void CGameMain::Level1Triggers()
{
   if(m_Lvl1SequenceFadeOut)
   {
      m_Lvl1SequenceBG->AdjustVolume(unsigned int(FSOUND_GetVolume(m_Lvl1SequenceBG->m_Channel) - 10 * g_FrameTime));
      if(FSOUND_GetVolume(m_Lvl1SequenceBG->m_Channel) <= 0)
      {
         m_Lvl1SequenceBG->StopAudio();
         m_Level1BGSound->PlayBackgroundLoop(120);
      }
   }

   if(m_ConvoysDead >= 2)
   {
      if(tempbool == false)
         {
            m_CurrentGameState = MISSION_LOST;
            g_Player.m_TimeMissionEnded = g_time;
            tempbool = true;
         }
   }

   switch(m_CurLevel1Event)
   {
      case NONE:
         {

         }
         break;

      case FINAL_ATTACK:
         {
            int numfighters = 6;

            CObjFrame frame;
            CVector3f ppos;
	         float angle; 

            for(int y = 0; y < numfighters; y++)
            {
               CFighter * fighter = g_ObjectFactory.CreateFighter();  
               fighter->Setup();
               fighter->Awaken();

               // 600 units from player, random direction
	            g_Player.GetPosition(&ppos, 0);
	            frame.SetPosition(&ppos);
	            angle = (rand() % 360) * PI / 180.0f;
	            frame.RotateLocalY(angle);
	            frame.MoveForward(-600.0f);
               frame.MoveUp(300.0f);

	            fighter->SetFrame(&frame, 0);
            }  

            m_CurLevel1Event = OVER;
         }
         break;

      case OVER:
         {
            if(m_NumConvoysHome > 1)
            {
               if(tempbool == false)
               {
                  m_CurrentGameState = MISSION_WON;
                  g_Player.m_TimeMissionEnded = g_time;
                  tempbool = true;
               }
            }
         }
         break;
   }   
}

void CGameMain::Level2Triggers()
{
   if(!m_InSequence)
   {
      m_Lvl1SequenceBG->AdjustVolume(unsigned int(FSOUND_GetVolume(m_Lvl1SequenceBG->m_Channel) - 10 * g_FrameTime));
      if(FSOUND_GetVolume(m_Lvl1SequenceBG->m_Channel) <= 0)
      {
         m_Lvl1SequenceBG->StopAudio();
         m_Level1BGSound->PlayBackgroundLoop(120);
      }
   }

   if(g_time - g_Player.m_TimeMissionStarted > 360.0f && m_BuildingsDead < 6 && g_Player.m_CurPlayerState != g_Player.DEAD)
   {
      if(tempbool == false)
      {
		 m_CurrentBuildingCom = COM_LVL2INTRO1;
         m_CurrentGameState = MISSION_WON;
         g_Player.m_TimeMissionEnded = g_time;
         tempbool = true;

		 return;
      }
   }

   if(m_BuildingsDead >= 6)
   {
      if(tempbool == false)
      {
         m_CurrentGameState = MISSION_LOST;
         g_Player.m_TimeMissionEnded = g_time;
         tempbool = true;
      }
   }
   
   switch(m_CurrentWave)
   {
      case WAVE_1:
      {
		if((g_time - g_Player.m_TimeMissionStarted) > 11.0f)
		{
			m_CurrentBuildingCom = COM_LVL2INTRO2;
            //SpawnUnits(OBJ_SCOUT, 10, ZONE_1);
           // SpawnUnits(OBJ_SCOUT, 10, ZONE_1);
            //SpawnUnits(OBJ_MEDTANK, 10, ZONE_1);
           // SpawnUnits(OBJ_MEDTANK, 10, ZONE_1);

            //SpawnUnits(OBJ_MEDTANK, 5, ZONE_1);
            //SpawnUnits(OBJ_MEDTANK, 5, ZONE_2);
            //SpawnUnits(OBJ_MEDTANK, 5, ZONE_3);
            //SpawnUnits(OBJ_MEDTANK, 5, ZONE_4);
            
            m_CurrentWave = WAVE_2;
		}
      }
      break;
      
      case WAVE_2:
      {
         if((g_time - g_Player.m_TimeMissionStarted) > 60.0f)
         {
            CPlasmaUpgrade * plasmaupgrade = g_ObjectFactory.CreatePlasmaUpgrade();
            plasmaupgrade->Setup();
            CVector3f Pos(-208.0f, 0.0f, -96.32f);
            plasmaupgrade->SetPosition(&Pos, 0);
            plasmaupgrade->FollowTerrain();

            SpawnUnits(OBJ_KAMITANK, 10, ZONE_1);
            SpawnUnits(OBJ_KAMITANK, 10, ZONE_2);
            SpawnUnits(OBJ_KAMITANK, 10, ZONE_3);
            SpawnUnits(OBJ_KAMITANK, 10, ZONE_4);
            
            m_CurrentWave = WAVE_3;
         }      
      }
      break;

      case WAVE_3:
      {    
         if((g_time - g_Player.m_TimeMissionStarted) > 90.0f)
         {
            CMissileUpgrade * plasmaupgrade = g_ObjectFactory.CreateMissileUpgrade();
            plasmaupgrade->Setup();
            CVector3f Pos(-208.0f, 0.0f, -96.32f);
            plasmaupgrade->SetPosition(&Pos, 0);
            plasmaupgrade->FollowTerrain();
            SpawnUnits(OBJ_SCOUT, 10, ZONE_1);
            SpawnUnits(OBJ_SCOUT, 10, ZONE_2);
            SpawnUnits(OBJ_SCOUT, 10, ZONE_3);
            SpawnUnits(OBJ_SCOUT, 10, ZONE_4);

            SpawnUnits(OBJ_MEDTANK, 10, ZONE_1);
            SpawnUnits(OBJ_MEDTANK, 10, ZONE_2);
            SpawnUnits(OBJ_MEDTANK, 10, ZONE_3);
            SpawnUnits(OBJ_MEDTANK, 10, ZONE_4);
            
            m_CurrentWave = WAVE_4;
         }      
      }
      break;

      case WAVE_4:
      {
         if((g_time - g_Player.m_TimeMissionStarted) > 130.0f)
         {
            CTrkMissileUpgrade * plasmaupgrade = g_ObjectFactory.CreateTrkMissileUpgrade();
            plasmaupgrade->Setup();
            CVector3f Pos(-208.0f, 0.0f, -96.32f);
            plasmaupgrade->SetPosition(&Pos, 0);
            plasmaupgrade->FollowTerrain();

            SpawnUnits(OBJ_SCOUT, 6, ZONE_1);
            SpawnUnits(OBJ_SCOUT, 6, ZONE_2);
            SpawnUnits(OBJ_SCOUT, 6, ZONE_3);
            SpawnUnits(OBJ_SCOUT, 6, ZONE_4);

            SpawnUnits(OBJ_MSLETANK, 3, ZONE_1);
            SpawnUnits(OBJ_MSLETANK, 3, ZONE_2);
            SpawnUnits(OBJ_MSLETANK, 3, ZONE_3);
            SpawnUnits(OBJ_MSLETANK, 3, ZONE_4);

            SpawnUnits(OBJ_MEDTANK, 6, ZONE_1);
            SpawnUnits(OBJ_MEDTANK, 6, ZONE_2);
            SpawnUnits(OBJ_MEDTANK, 6, ZONE_3);
            SpawnUnits(OBJ_MEDTANK, 6, ZONE_4);
            
            m_CurrentWave = WAVE_5;
         }      
      }
      break;

      case WAVE_5:
      {
         if((g_time - g_Player.m_TimeMissionStarted) > 190.0f)
         {
            SpawnUnits(OBJ_SCOUT, 3, ZONE_1);
            SpawnUnits(OBJ_SCOUT, 3, ZONE_2);
            SpawnUnits(OBJ_SCOUT, 3, ZONE_3);
            SpawnUnits(OBJ_SCOUT, 3, ZONE_4);

            SpawnUnits(OBJ_HVYTANK, 3, ZONE_1);
            SpawnUnits(OBJ_HVYTANK, 3, ZONE_2);
            SpawnUnits(OBJ_HVYTANK, 3, ZONE_3);
            SpawnUnits(OBJ_HVYTANK, 3, ZONE_4);

            SpawnUnits(OBJ_KAMITANK, 3, ZONE_1);
            SpawnUnits(OBJ_KAMITANK, 3, ZONE_2);
            SpawnUnits(OBJ_KAMITANK, 3, ZONE_3);
            SpawnUnits(OBJ_KAMITANK, 3, ZONE_4);

            SpawnUnits(OBJ_MSLETANK, 2, ZONE_1);
            SpawnUnits(OBJ_MSLETANK, 2, ZONE_2);
            SpawnUnits(OBJ_MSLETANK, 2, ZONE_3);
            SpawnUnits(OBJ_MSLETANK, 2, ZONE_4);
            
            m_CurrentWave = WAVE_6;
         }      
      }
      break;

      case WAVE_6:
      {
         if((g_time - g_Player.m_TimeMissionStarted) > 240.0f)
         {
            SpawnUnits(OBJ_SCOUT, 4, ZONE_1);
            SpawnUnits(OBJ_SCOUT, 4, ZONE_2);
            SpawnUnits(OBJ_SCOUT, 4, ZONE_3);
            SpawnUnits(OBJ_SCOUT, 4, ZONE_4);

            SpawnUnits(OBJ_KAMITANK, 4, ZONE_1);
            SpawnUnits(OBJ_KAMITANK, 4, ZONE_2);
            SpawnUnits(OBJ_KAMITANK, 4, ZONE_3);
            SpawnUnits(OBJ_KAMITANK, 4, ZONE_4);

            SpawnUnits(OBJ_HVYTANK, 2, ZONE_1);
            SpawnUnits(OBJ_HVYTANK, 2, ZONE_2);
            SpawnUnits(OBJ_HVYTANK, 2, ZONE_3);
            SpawnUnits(OBJ_HVYTANK, 2, ZONE_4);
            
            m_CurrentWave = WAVE_7;
         }      
      }
      break;

      case WAVE_7:
      {
         if((g_time - g_Player.m_TimeMissionStarted) > 280.0f)
         {
            SpawnUnits(OBJ_MSLETANK, 2, ZONE_1);
            SpawnUnits(OBJ_MSLETANK, 1, ZONE_2);
            SpawnUnits(OBJ_MSLETANK, 1, ZONE_3);
            SpawnUnits(OBJ_MSLETANK, 2, ZONE_4);

            SpawnUnits(OBJ_HVYTANK, 2, ZONE_1);
            SpawnUnits(OBJ_HVYTANK, 2, ZONE_2);
            SpawnUnits(OBJ_HVYTANK, 2, ZONE_3);
            SpawnUnits(OBJ_HVYTANK, 2, ZONE_4);

            SpawnUnits(OBJ_KAMITANK, 3, ZONE_1);
            SpawnUnits(OBJ_KAMITANK, 3, ZONE_2);
            SpawnUnits(OBJ_KAMITANK, 3, ZONE_3);
            SpawnUnits(OBJ_KAMITANK, 3, ZONE_4);

            SpawnUnits(OBJ_MEDTANK, 4, ZONE_1);
            SpawnUnits(OBJ_MEDTANK, 4, ZONE_2);
            SpawnUnits(OBJ_MEDTANK, 4, ZONE_3);
            SpawnUnits(OBJ_MEDTANK, 4, ZONE_4);
            
            m_CurrentWave = WAVE_8;
         }      
      }
      break;
   }
}

void CGameMain::Level3Triggers()
{
   level3time += g_FrameTime;

   if(!m_InSequence)
   {
      m_Lvl1SequenceBG->AdjustVolume(unsigned int(FSOUND_GetVolume(m_Lvl1SequenceBG->m_Channel) - 10 * g_FrameTime));
      if(FSOUND_GetVolume(m_Lvl1SequenceBG->m_Channel) <= 0)
      {
         m_Lvl1SequenceBG->StopAudio();
         m_Level1BGSound->PlayBackgroundLoop(120);
      }
   }

   // Winning Condition
   if(m_BossAlive == false)
   {
      if(tempboolboss == false)
      {
         m_CurrentGameState = MISSION_WON;
         g_Player.m_TimeMissionEnded = g_time;
         tempboolboss = true;
      }
   }

   switch(m_CurLevel3Event)
   {
      case LVL3WAVE_1:
         {
           if(level3time > 20.0f)
           {
               SpawnUnits(OBJ_FIGHTER, 5, ZONE_1);
               SpawnUnits(OBJ_FIGHTER, 5, ZONE_2);  
               m_CurLevel3Event = LVL3WAVE_2;
           }
         }
         break;

      case LVL3WAVE_2:
         {
           if(level3time > 120.0f)
           {
               SpawnUnits(OBJ_FIGHTER, 5, ZONE_3);
               SpawnUnits(OBJ_FIGHTER, 5, ZONE_4);  
               m_CurLevel3Event = LVL3WAVE_3;
           }
         }
         break;

      case LVL3WAVE_3:
         {

         }
         break;
   }

}

void CGameMain::SpawnUnits(int UnitType, int numtospawn, int SPAWN_ZONE)
{
   for(int g = 0; g < numtospawn; g++)
   {
      CEnemy * nmy;
      switch(UnitType)
      {
         case OBJ_MEDTANK:
            nmy = g_ObjectFactory.CreateMediumTank();  
            nmy = (CMedTank*)nmy;
            break;
         case OBJ_MSLETANK:
            nmy = g_ObjectFactory.CreateMissileTank();  
            nmy = (CMissileTank*)nmy;
            break;
         case OBJ_SCOUT:
            nmy = g_ObjectFactory.CreateScout();  
            nmy = (CScout*)nmy;
            break;
         case OBJ_HVYTANK:
            nmy = g_ObjectFactory.CreateHeavyTank();  
            nmy = (CHeavyTank*)nmy;
            break;
         case OBJ_SPHERE:
            nmy = g_ObjectFactory.CreateSphere();  
            nmy = (CSphere*)nmy;
            break;
         case OBJ_FIGHTER:
            nmy = g_ObjectFactory.CreateFighter();
            nmy->MoveUp(50.0f, 0);  
            nmy = (CFighter*)nmy;
            break;
         case OBJ_KAMITANK:
            nmy = g_ObjectFactory.CreateMeleeTank();  
            nmy = (CMeleeTank*)nmy;
            break;
      }
     
      //CObjFrame frame;
      CVector3f ppos;
	  // float angle;

      nmy->Setup();
      ppos.y = 0.0f;


      switch(SPAWN_ZONE)
      {
        case ZONE_1:
            ppos.x = RandomFloatRange(-800.0f, 800.0f);
            ppos.z = RandomFloatRange(880.0f, 980.0f);
            break;
        case ZONE_2:
            ppos.x = RandomFloatRange(-900.0f, -980.0f);
            ppos.z = RandomFloatRange(-800.0f, 800.0f);
            break;
        case ZONE_3:
            ppos.x = RandomFloatRange(-800.0f, 800.0f);
            ppos.z = RandomFloatRange(-880.0f, -980.0f);
            break;
        case ZONE_4:
            ppos.x = RandomFloatRange(900.0f, 980.0f);
            ppos.z = RandomFloatRange(-800.0f, 800.0f);
            break;
      }

      nmy->SetPosition(&ppos, 0);
      nmy->Awaken();
      nmy->DebugAIOff();

      if (UnitType != OBJ_FIGHTER)
         nmy->FollowTerrain();
   }
}

void CGameMain::ProccessCommunications(void)
{
   switch(m_CurrentCommunication)
   {
      case COM_NONE:
         break;
      case COM_INBOUND:
         {
           m_IncMessage->Play2DAudio(150);
           CVector2f loc;
           loc.x = -0.8f;
           loc.y = 0.625f;
           Color4f color = {0.0f, 0.0f, 1.0f, 1.0f};
           g_HudTextManager.CreateMothershipCom("\"Attack run initiated.\"", loc, color, 3);
           m_CurrentCommunication = COM_NONE;
         }
         break;
      case COM_IONCHARGED:
         {
           m_IncMessage->Play2DAudio(150);
           CVector2f loc;
           loc.x = -0.8f;
           loc.y = 0.625f;
           Color4f color = {0.0f, 0.0f, 1.0f, 1.0f};
           g_HudTextManager.CreateMothershipCom("\"Ion Cannon charged Commander.\"", loc, color, 3);
           m_CurrentCommunication = COM_NONE;   
         }
         break;
      case COM_IONFIRING:
         {
            m_IncMessage->Play2DAudio(150);
           CVector2f loc;
           loc.x = -0.8f;
           loc.y = 0.625f;
           Color4f color = {1.0f, 0.0f, 0.0f, 1.0f};
           g_HudTextManager.CreateMothershipCom("\"Clear the blast radius, Commander.\"", loc, color, 3);
           m_CurrentCommunication = COM_NONE;
         }
         break;
      case COM_LEAVE:
         {
            m_IncMessage->Play2DAudio(150);
           CVector2f loc;
            loc.x = -0.8f;
           loc.y = 0.625f;
           Color4f color = {0.0f, 0.0f, 1.0f, 1.0f};
           g_HudTextManager.CreateMothershipCom("\"Re-entering orbit.\"", loc, color, 3);
           m_CurrentCommunication = COM_NONE;
         }
         break;
     case COM_LVL1INTRO1:
         {
            m_IncMessage->Play2DAudio(150);
           CVector2f loc;
            loc.x = -0.8f;
           loc.y = 0.625f;
           Color4f color = {0.0f, 0.0f, 1.0f, 1.0f};
           g_HudTextManager.CreateMothershipCom("\"ETA 15 seconds until drop-off.\"", loc, color, 3);
           m_CurrentCommunication = COM_NONE;
         }
         break;

     case COM_LVL1INTRO2:
      {
        m_IncMessage->Play2DAudio(150);
        CVector2f loc;
        loc.x = -0.8f;
        loc.y = 0.65f;
        Color4f color = {0.0f, 0.0f, 1.0f, 1.0f};
        g_HudTextManager.CreateMothershipCom("\"The convoy should be making its", loc, color, 3);
        
        loc.x = -0.8f;
        loc.y = 0.6f;
        g_HudTextManager.CreateMothershipCom("way through the valley as we land.\"", loc, color, 3);
        m_CurrentCommunication = COM_NONE;
      }
      break;

    case COM_LVL1INTRO3:
      {
         m_IncMessage->Play2DAudio(150);
        CVector2f loc;
         loc.x = -0.8f;
        loc.y = 0.65f;
        Color4f color = {0.0f, 0.0f, 1.0f, 1.0f};
        g_HudTextManager.CreateMothershipCom("\"You must get at least two of these", loc, color, 3);
        
        loc.x = -0.8f;
        loc.y = 0.6f;
        g_HudTextManager.CreateMothershipCom("trucks to Vesta.  You are their only hope.\"", loc, color, 3);  
        m_CurrentCommunication = COM_NONE;
      }
      break;
   }

   switch(m_CurrentConvoyCom)
   {
      case COM_CONVOYNONE:
         {

         }
         break;
      case COM_CONVOYHURT:
         {
           m_IncMessage->Play2DAudio(150);
           CVector2f loc;
            loc.x = -0.8f;
           loc.y = 0.425f;
           Color4f color = {1.0f, 0.0f, 0.0f, 2.0f};
           g_HudTextManager.CreateConvoyCom("\"Sir, We are taking heavy damage!\"", loc, color, 3);
           m_CurrentConvoyCom = COM_CONVOYNONE;
         }
         break;
      case COM_CONVOYDEAD:
         {
           m_IncMessage->Play2DAudio(150);
           CVector2f loc;
            loc.x = -0.8f;
           loc.y = 0.425f;
           Color4f color = {1.0f, 0.0f, 0.0f, 2.0f};
           g_HudTextManager.CreateConvoyCom("\"Sir, We have lost a supply truck!\"", loc, color, 3);
           m_CurrentConvoyCom = COM_CONVOYNONE;
         }
         break;
      case COM_CONVOYHOME:
         {
            m_IncMessage->Play2DAudio(150);
            CVector2f loc;
            loc.x = -0.8f;
            loc.y = 0.425f;
            Color4f color = {0.0f, 0.0f, 1.0f, 2.0f};
            g_HudTextManager.CreateConvoyCom("\"We are safely in the base Sir.\"", loc, color, 3);
            m_CurrentConvoyCom = COM_CONVOYNONE;
         }
         break;
   }

   switch(m_CurrentBuildingCom)
   {
      case COM_BUILDINGNONE:
         break;
      case COM_BUILDINGDEAD:
         {
            m_IncMessage->Play2DAudio(150);
            CVector2f loc;
            loc.x = -0.8f;
            loc.y = 0.425f;
            Color4f color = {1.0f, 0.0f, 0.0f, 2.0f};
            g_HudTextManager.CreateBuildingCom("\"The Choikin have destroyed a building Sir!\"", loc, color, 3);
            m_CurrentBuildingCom = COM_BUILDINGNONE;
         }
         break;
      case COM_LVL2INTRO1:
         {
            m_IncMessage->Play2DAudio(150);
            CVector2f loc;
            loc.x = -0.8f;
            loc.y = 0.425f;
            Color4f color = {0.0f, 0.0f, 1.0f, 2.0f};
            g_HudTextManager.CreateBuildingCom("\"The Choikin forces have retreated!.\"", loc, color, 2);
            m_CurrentBuildingCom = COM_BUILDINGNONE;
         }
         break;

      case COM_LVL2INTRO2:
         {
           m_IncMessage->Play2DAudio(150);
           CVector2f loc;
           loc.x = -0.8f;
           loc.y = 0.45f;
           Color4f color = {0.0f, 0.0f, 1.0f, 1.0f};
           g_HudTextManager.CreateBuildingCom("\"Choikin approaching northern perimiter", loc, color, 2);
        
           loc.x = -0.8f;
           loc.y = 0.4f;
           g_HudTextManager.CreateBuildingCom("Sound the alarm!.\"", loc, color, 2);  
           m_CurrentBuildingCom = COM_BUILDINGNONE;
         }
         break;

      case COM_LVL2INTRO3:
         {
            m_IncMessage->Play2DAudio(150);
           CVector2f loc;
            loc.x = -0.8f;
           loc.y = 0.45f;
           Color4f color = {0.0f, 0.0f, 1.0f, 1.0f};
           g_HudTextManager.CreateBuildingCom("\"Choikin units spotted!", loc, color, 2);
        
           loc.x = -0.8f;
           loc.y = 0.4f;
           g_HudTextManager.CreateBuildingCom("Sound the alarm!\"", loc, color, 2);  
           m_CurrentBuildingCom = COM_BUILDINGNONE;
         }
         break;
   }
}

//////////////////////////////////////////////////////////////////////////
/// Lets clean up
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CGameMain::Shutdown()
{
   g_Player.Reset();
   g_Player.Unload();
   g_MotherShip.Die();
   g_MotherShip.Unload();
   g_Hud.Shutdown();
   
   // Reset the game state to running for next time
   m_CurrentGameState = GAME_RUNNING;

   //m_Level1BGSound->StopAudio();

   // Shutdown all managers
   g_TextureFont.Shutdown();
	g_GeometryManager.UnloadAll();
	g_TextureManager.UnloadAll();
	g_ParticleManager.Shutdown();
	g_ObjectFactory.Shutdown();
   g_AudioManager.Shutdown();

   // Clean up objects in the game list
	CGameObject *obj;
   while ((obj = m_ObjList))
	{
		m_ObjList = m_ObjList->m_Next;

      if (obj != &g_Player && obj != &g_MotherShip)
		   delete obj;
	}

   g_IonLight.Disable();

   // Delete other game memory
	delete m_pSkyBox;
   delete g_pCurrLevel;
	delete m_pRenderList;
   delete m_pMiniMap;
          
	delete m_pPlasmaObject;
   delete m_pMedTankObject;
   delete m_pMeleeTankObject;
   delete m_pShieldPowerUpObject;
   delete m_pMissileObject;
   delete m_pIonCannonObject;
   delete m_pTreeObject;
   delete m_pTree2Object;
   delete m_pTree3Object;
   delete m_pMissilePackObject;
   delete m_pTrackingMissilePackObject;
   delete m_pTrackingMissileObject;
   delete m_pScoutObject;
   delete m_pBuildingObject;
   delete m_pHeavyTankObject;
   delete m_pMissileUpgradeObject;
   delete m_pTrkMissileUpgradeObject;
   delete m_pPlasmaUpgradeObject;
   delete m_pConvoyObject;
   delete m_pMissileTankObject;
   delete m_pSphereObject;
   delete m_pFighterObject;
   delete m_pIonFlareObject;
   delete m_pLaserObject;
   delete m_pFriendlyTurretObject;
   delete m_pBuildingObject2;
   delete m_pBuildingObject3;
   delete m_pEnemyTurretObject;
   delete m_pRock1Object;
   delete m_pRock2Object;
   delete m_pRock3Object;
   delete m_pRock4Object;
   delete m_pRock5Object;
   delete m_pRock6Object;
   delete m_pBossObject;
   delete m_pBombObject;

    g_Quadtree.Unload();

   g_Camera = 0;
}