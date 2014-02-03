/// \file gamemain.h
/// The ingame loop
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "core\gamestatemanager.h"
#include "math3d\math3d.h"

// forward declarations
class CPlasmaShot;
class CMissile;
class CIonCannon;
class CTrackingMissile;
class CIonFlare;
class CLaser;
class CBomb;

// Enemies ///////////////////////
class CMedTank;
class CHeavyTank;
class CMissileTank;
class CMeleeTank;
class CSphere;
class CFighter;
class CScout;
class CEnemyTurret;

// Powerups //////////////////////
class CShieldPowerUp;
class CMissilePack;
class CTrackingMissilePack;
class CTrkMissileUpgrade;
class CPlasmaUpgrade;
class CMissileUpgrade;

// Stationary ////////////////////
class CTree;
class CTree2;
class CTree3;
class CBuilding;
class CBuilding2;
class CBuilding3;
class CConvoy;
class CRock1;
class CRock2;
class CRock3;
class CRock4;
class CRock5;
class CRock6;
class CBoss;

// Friendly
class CFriendlyTurret; 

class CAudio;
class CGameObject;
class CRenderList;
class CMiniMap;
class CSkyBox;

class CGameMain : CGameStateManager 
{
	public:
      CGameMain(void);
		~CGameMain(void) {} 

		virtual void Initialize(void);
		virtual void GetInput(void);
		virtual unsigned int Main(void);
		virtual void Render(void);
		virtual unsigned int Update(void);
		virtual void Shutdown(void);

      // States that the Gamemain could be in
      enum GameState{GAME_RUNNING = 0, GAME_PAUSED, GAME_EXIT, GOTO_MENU, GOTO_VICTORYSCREEN, GOTO_LOSTSCREEN, QUIT_SCREEN, MISSION_WON, MISSION_LOST};

      // Current level of the game
      enum CurrentLevel{Level1 = 0, Level2, Level3, EditorLevel, TestLevel, Skirmish};
      enum Level2SpawnZones{ZONE_1 = 0, ZONE_2, ZONE_3, ZONE_4};
      enum Level1Events{NONE = 0, FINAL_ATTACK, OVER};
      enum Level2Events{WAVE_1 = 0, WAVE_2, WAVE_3, WAVE_4, WAVE_5, WAVE_6, WAVE_7, WAVE_8, WAVE_9, WAVE_NONE};
      enum Level3Events{LVL3NONE = 0, LVL3WAVE_1, LVL3WAVE_2, LVL3WAVE_3};
      enum MothershipCommunications{COM_NONE = 0, COM_IONCHARGED, COM_INBOUND, COM_IONFIRING, COM_LEAVE, COM_LVL1INTRO1, COM_LVL1INTRO2, COM_LVL1INTRO3, COM_LVL1INTRO4};
      enum ConvoyCommunications{COM_CONVOYNONE = 0, COM_CONVOYHURT, COM_CONVOYDEAD, COM_CONVOYHOME};
      enum BuildingCommunications{COM_BUILDINGNONE = 0, COM_BUILDINGDEAD, COM_LVL2INTRO1, COM_LVL2INTRO2, COM_LVL2INTRO3};
      
      int m_CurrentWave;
      int m_CurrentLevel;
      int m_CurLevel1Event;
      int m_CurLevel3Event;
      int m_CurrentGameState;
      int m_NumEnemiesAlive;
      int m_CurrentCommunication;
      int m_CurrentBuildingCom;
      int m_CurrentConvoyCom;
      int m_ConvoysDead;
      bool m_EditerTest;
      int m_BuildingsDead;
      bool m_BossAlive;
      bool tempboolboss;
      bool lvl3spawnonce;

      int m_NumConvoysHome;

      // Projectiles ///////////////////
	  CPlasmaShot       *m_pPlasmaObject;
      CMissile          *m_pMissileObject;
      CIonCannon        *m_pIonCannonObject;
      CTrackingMissile  *m_pTrackingMissileObject;
      CIonFlare         *m_pIonFlareObject;
      CLaser            *m_pLaserObject;
      CBomb             *m_pBombObject;

      // Enemies ///////////////////////
      CMedTank     *m_pMedTankObject;
      CHeavyTank   *m_pHeavyTankObject;
      CMissileTank *m_pMissileTankObject;
      CMeleeTank   *m_pMeleeTankObject;
      CSphere      *m_pSphereObject;
      CFighter     *m_pFighterObject;
      CScout       *m_pScoutObject;
      CEnemyTurret *m_pEnemyTurretObject;

      // Powerups //////////////////////
      CShieldPowerUp       * m_pShieldPowerUpObject;
      CMissilePack         * m_pMissilePackObject;
      CTrackingMissilePack * m_pTrackingMissilePackObject;
      CTrkMissileUpgrade   * m_pTrkMissileUpgradeObject;
      CPlasmaUpgrade       * m_pPlasmaUpgradeObject;
      CMissileUpgrade      * m_pMissileUpgradeObject;

      // Stationary ////////////////////
      CTree       *m_pTreeObject;
      CTree2      *m_pTree2Object;
      CTree3      *m_pTree3Object;
      CBuilding   *m_pBuildingObject;
      CBuilding2  *m_pBuildingObject2;
      CBuilding3  *m_pBuildingObject3;
      CConvoy     *m_pConvoyObject;
	   CRock1	   *m_pRock1Object;
	   CRock2      *m_pRock2Object;
	   CRock3	   *m_pRock3Object;
      CRock4      *m_pRock4Object;
      CRock5      *m_pRock5Object;
      CRock6      *m_pRock6Object;
      CBoss       *m_pBossObject;

      // Friendly
      CFriendlyTurret *m_pFriendlyTurretObject;

      // Cheat codes
      bool m_GodMode;

      // In 
		bool m_InSequence;

      // Background sound
      CAudio * m_Level1BGSound;  
      CAudio * m_IncMessage;
      CAudio * m_Lvl1SequenceBG;

      bool m_Lvl1SequenceFadeOut;
        
      void LoadObjects(void);
		void AddToObjList(CGameObject *obj);
		void RemoveFromObjList(CGameObject *obj);
      void AddToCollisionAvoidanceList(CGameObject* obj);
      void RemoveFromCollisionAvoidanceList(CGameObject* obj);
      void AssignModelToObject(CGameObject *obj, char * fn);
      void LoadLevel1(void);
      void LoadLevel2(void);
      void LoadLevel3(void);
      
      void LoadSkirmish();
        
      void LoadLevelTest(void);
      void SetupLightingAndFog(void);
      void SetupCamera(void);
      void Level1Triggers();
      void Level2Triggers();
      void Level3Triggers();
	  void SkirmishTriggers();
      void SpawnUnits(int UnitType, int numtospawn, int SPAWN_ZONE);
      void ProccessCommunications(void);

		void SetupLevel1Sequence(void);
		void UpdateLevel1Sequence(void);
      
		void SetupLevel2Sequence(void);
		void UpdateLevel2Sequence(void);

		void SetupLevel3Sequence(void);
		void UpdateLevel3Sequence(void);

      CGameObject *m_ObjList;

      char* m_pEditorLevel;
      CRenderList* m_pRenderList;

	private:
      CSkyBox* m_pSkyBox;
		CGameObject* m_pTestObject;
		

      CMiniMap* m_pMiniMap;

		// Intro sequence stuff
		enum {FLY = 0, PAN, DROP, LEAVE};
		int			m_DropOffStage;
		CVector3f	m_LookAtOffset;
		bool			m_FirstTimeSetup;
};















