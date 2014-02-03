/// \file objectfactory.h 
/// Used to create instances of gameobjects
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/plasmashot.h"
#include "gamelogic/gameobject.h"
#include "gamelogic/medtank.h" 
#include "gamelogic/meleetank.h"
#include "gamelogic/missiletank.h"  
#include "gamelogic/heavytank.h" 
#include "gamelogic/sphere.h"
#include "gamelogic/missile.h"  
#include "gamelogic/fighter.h"   
#include "gamelogic/shieldpowerup.h"
#include "gamelogic/missilepack.h"
#include "gamelogic/trackingmissilepack.h"
#include "gamelogic/tree.h"
#include "gamelogic/building.h"
#include "gamelogic/ioncannon.h"
#include "gamelogic/tree2.h"
#include "gamelogic/tree3.h"  
#include "gamelogic/trackingmissile.h" 
#include "gamelogic/scout.h"
#include "gamelogic/convoy.h"
#include "gamelogic/plasmaupgrade.h"
#include "gamelogic/missileupgrade.h"  
#include "gamelogic/trkmissileupgrade.h"
#include "gamelogic/building2.h" 
#include "gamelogic/building3.h"
#include "gamelogic/fturret.h"   
#include "gamelogic/eturret.h"
#include "gamelogic/ionflare.h"
#include "gamelogic/laser.h"
#include "gamelogic/rock.h"
#include "gamelogic/bomb.h"
#include "gamelogic/boss.h"

struct PoolNode
{
	union
	{
		CPlasmaShot          *plasma;
      CMissile             *missile;
		CMedTank             *medtank;
      CHeavyTank           *heavytank;
      CMissileTank         *missiletank;
      CSphere              *sphere;  
      CFighter             *fighter;
      CMeleeTank           *meleetank;
      CShieldPowerUp       *shieldpowerup;
      CMissilePack         *missilepack;
      CTrackingMissilePack *trackingmissilepack;
      CTree                *tree;
      CBuilding            *building;
      CIonCannon           *ioncannon;
      CTrackingMissile     *trackingmissile;
      CTree2               *tree2;
      CTree3               *tree3;
      CScout               *scout;
      CEnemyTurret         *eturret;
      CFriendlyTurret      *fturret;
      CBuilding2           *building2;
      CBuilding3           *building3;
      CTrkMissileUpgrade   *trkmissileupgrade;
      CMissileUpgrade      *missileupgrade;
      CPlasmaUpgrade       *plasmaupgrade;
      CConvoy              *convoy;
      CIonFlare            *ionflare;
      CLaser               *laser;
	   CRock1			   *rock1;
	   CRock2			   *rock2;
	   CRock3			   *rock3;
      CRock4            *rock4;
      CRock5            *rock5;
      CRock6            *rock6;
      CBomb             *bomb;
      CBoss             *boss;

	} obj;

	PoolNode *next;
};

class CObjectFactory
{
	private:
      
		PoolNode *FreeNodes;
		PoolNode *PlasmaPool;
      PoolNode *MissilePool;
		PoolNode *MedTankPool;
      PoolNode *HeavyTankPool;
      PoolNode *MissileTankPool;
      PoolNode *FighterPool;
      PoolNode *SpherePool;
      PoolNode *MeleeTankPool;
      PoolNode *ShieldPowerUpPool;
      PoolNode *MissilePackPool;
      PoolNode *TrackingMissilePackPool;
      PoolNode *TreePool;
      PoolNode *BuildingPool;
      PoolNode *IonCannonPool;
      PoolNode *TrackingMissilePool;
      PoolNode *Tree3Pool;
      PoolNode *Tree2Pool;
      PoolNode *ScoutPool;
      PoolNode *ConvoyPool;
      PoolNode *EnemyTurretPool;
      PoolNode *FriendlyTurretPool;
      PoolNode *TrkMissileUpgradePool;
      PoolNode *MissileUpgradePool;
      PoolNode *PlasmaUpgradePool;
      PoolNode *Building2Pool;
      PoolNode *Building3Pool;
      PoolNode *IonFlarePool;
      PoolNode *LaserPool;
	   PoolNode *Rock1Pool;
	   PoolNode *Rock2Pool;
	   PoolNode *Rock3Pool;
      PoolNode *Rock4Pool;
      PoolNode *Rock5Pool;
      PoolNode *Rock6Pool;
      PoolNode *BombPool;
      PoolNode *BossPool;

		PoolNode *GetFreeNode(void);
		void ReleaseNode(PoolNode *node);

		// Get a free obj from the pool
		CPlasmaShot *GetFreePlasma(void);
      CMissile *GetFreeMissile(void);
		CMedTank *GetFreeMedTank(void);
      CHeavyTank *GetFreeHeavyTank(void);
      CMissileTank *GetFreeMissileTank(void);
      CFighter *GetFreeFighter(void);
      CSphere *GetFreeSphere(void);
      CMeleeTank *GetFreeMeleeTank(void);
      CShieldPowerUp *GetFreeShieldPowerUp(void);
      CMissilePack *GetFreeMissilePack(void);
      CTrackingMissilePack *GetFreeTrackingMissilePack(void);
      CTree *GetFreeTree(void);
      CBuilding *GetFreeBuilding(void);
      CIonCannon *GetFreeIonCannon(void);
      CTrackingMissile *GetFreeTrackingMissile(void);
      CTree2 *GetFreeTree2(void);
      CTree3 *GetFreeTree3(void);
      CScout *GetFreeScout(void);
      CConvoy *GetFreeConvoy(void);
      CEnemyTurret *GetFreeEnemyTurret(void);
      CFriendlyTurret *GetFreeFriendlyTurret(void);
      CMissileUpgrade *GetFreeMissileUpgrade(void);
      CTrkMissileUpgrade *GetFreeTrkMissileUpgrade(void);
      CPlasmaUpgrade *GetFreePlasmaUpgrade(void);
      CBuilding2 *GetFreeBuilding2(void);
      CBuilding3 *GetFreeBuilding3(void);
      CIonFlare *GetFreeIonFlare(void);
      CLaser *GetFreeLaser(void);
	  CRock1 *GetFreeRock1(void);
	  CRock2 *GetFreeRock2(void);
	  CRock3 *GetFreeRock3(void);
     CRock4 *GetFreeRock4(void);
     CRock5 *GetFreeRock5(void);
     CRock6 *GetFreeRock6(void);
     CBomb *GetFreeBomb(void);
     CBoss *GetFreeBoss(void);

		// Free allocated memory for pools
		void DestroyPlasmaPool(void);
		void DestroyMedTankPool(void);
      void DestroyMeleeTankPool(void);
      void DestroyMissilePool(void);
      void DestroyHeavyTankPool(void);
      void DestroyMissileTankPool(void);
      void DestroyFighterPool(void);
      void DestroySpherePool(void);
      void DestroyShieldPowerUpPool(void);
      void DestroyMissilePackPool(void);
      void DestroyTrackingMissilePackPool(void);
      void DestroyTreePool(void);
      void DestroyBuildingPool(void);
      void DestroyIonCannonPool(void);
      void DestroyTrackingMissilePool(void);
      void DestroyTree2Pool(void);
      void DestroyTree3Pool(void);
      void DestroyScoutPool(void);
      void DestroyConvoyPool(void);
      void DestroyEnemyTurretPool(void);
      void DestroyFriendlyTurretPool(void);
      void DestroyMissileUpgradePool(void);
      void DestroyPlasmaUpgradePool(void);
      void DestroyTrkMissileUpgradePool(void);
      void DestroyBuilding2Pool(void);
      void DestroyBuilding3Pool(void);
      void DestroyFreeNodes(void);
      void DestroyIonFlarePool(void);
      void DestroyLaserPool(void);
	  void DestroyRock1Pool(void);
	  void DestroyRock2Pool(void);
	  void DestroyRock3Pool(void);
     void DestroyRock4Pool(void);
     void DestroyRock5Pool(void);
     void DestroyRock6Pool(void);
     void DestroyBombPool(void);
     void DestroyBossPool(void);
      
		// Release the object back to pool
      void ReleasePlasma(CPlasmaShot *shot);
      void ReleaseMedTank(CMedTank *medtank);
      void ReleaseMeleeTank(CMeleeTank *meleetank);
      void ReleaseMissileTank(CMissileTank *missiletank);
      void ReleaseHeavyTank(CHeavyTank *heavytank);
      void ReleaseFighter(CFighter *fighter);
      void ReleaseSphere(CSphere *sphere);
      void ReleaseMissile(CMissile *missile);
      void ReleaseShieldPowerUp(CShieldPowerUp *shieldpowerup);
      void ReleaseMissilePack(CMissilePack *missilepack);
      void ReleaseTrackingMissilePack(CTrackingMissilePack *trackingmissilepack);
      void ReleaseTree(CTree *tree);
      void ReleaseBuilding(CBuilding *building);
      void ReleaseIonCannon(CIonCannon *ioncannon);
      void ReleaseTrackingMissile(CTrackingMissile *trackingmissile);
      void ReleaseTree2(CTree2 *tree2);
      void ReleaseTree3(CTree3 *tree3);
      void ReleaseScout(CScout *scout);
      void ReleaseConvoy(CConvoy *convoy);
      void ReleaseEnemyTurret(CEnemyTurret *eturret);
      void ReleaseFriendlyTurret(CFriendlyTurret *fturret);
      void ReleaseMissileUpgrade(CMissileUpgrade *missileupgrade);
      void ReleaseTrkMissileUpgrade(CTrkMissileUpgrade *trkmissileupgrade);
      void ReleasePlasmaUpgrade(CPlasmaUpgrade * plasmaupgrade);
      void ReleaseBuilding2(CBuilding2 * building2);
      void ReleaseBuilding3(CBuilding3 * building3);
      void ReleaseIonFlare(CIonFlare *ionflare);
      void ReleaseLaser(CLaser *laser);
	   void ReleaseRock1(CRock1 *rock);
	   void ReleaseRock2(CRock2 *rock);
	   void ReleaseRock3(CRock3 *rock);
      void ReleaseRock4(CRock4 *rock);
      void ReleaseRock5(CRock5 *rock);
      void ReleaseRock6(CRock6 *rock);
      void ReleaseBomb(CBomb *bomb);
      void ReleaseBoss(CBoss *boss);
		void ReleaseObject(PoolNode *node, PoolNode **pool);
      
	public:
      
		CObjectFactory(void);
		~CObjectFactory(void);
      void Shutdown(void);

		// Create an object
      CPlasmaShot *CreatePlasmaShot(void);
      CMedTank *CreateMediumTank(void);
      CHeavyTank *CreateHeavyTank(void);
      CMissileTank *CreateMissileTank(void);
      CFighter *CreateFighter(void);
      CSphere *CreateSphere(void);
      CMissile *CreateMissile(void);
      CMeleeTank *CreateMeleeTank(void);
      CShieldPowerUp *CreateShieldPowerUp(void);
      CMissilePack *CreateMissilePack(void);
      CTrackingMissilePack *CreateTrackingMissilePack(void);
      CTree *CreateTree(void);
      CBuilding *CreateBuilding(void);
      CIonCannon *CreateIonCannon(void);
      CTrackingMissile *CreateTrackingMissile(void);
      CTree2 *CreateTree2(void);
      CTree3 *CreateTree3(void);
      CScout *CreateScout(void);
      CConvoy *CreateConvoy(void);
      CEnemyTurret *CreateEnemyTurret(void);
      CFriendlyTurret *CreateFriendlyTurret(void);
      CMissileUpgrade *CreateMissileUpgrade(void);
      CTrkMissileUpgrade *CreateTrkMissileUpgrade(void);
      CPlasmaUpgrade *CreatePlasmaUpgrade(void);
      CBuilding2 *CreateBuilding2(void);
      CBuilding3 *CreateBuilding3(void);
      CIonFlare *CreateIonFlare(void);
      CLaser *CreateLaser(void);
	   CRock1 *CreateRock1(void);
	   CRock2 *CreateRock2(void);	
	   CRock3 *CreateRock3(void);
      CRock4 *CreateRock4(void);
      CRock5 *CreateRock5(void);
      CRock6 *CreateRock6(void);
      CBomb * CreateBomb(void);
      CBoss * CreateBoss(void);

     void SetupLevel1Factory();
     void SetupLevel2Factory();
     void SetupLevel3Factory();

	void ReturnAndNoRefund(CGameObject *obj);
};







