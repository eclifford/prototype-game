/// \file objectfactory.cpp
/// Used to create game objects
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/objectfactory.h"
#include "core/gamemain.h"
#include "graphics/particlemanager.h"

#include "utils\mmgr.h"

/////////////////////////////////
/*/ External Global Instances /*/ 
/////////////////////////////////

extern CGameMain GameMain;
extern CQuadtree g_Quadtree;
extern CParticleManager g_ParticleManager;

//////////////////////////////////////////////////////////////////////////
/// Constructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CObjectFactory::CObjectFactory()
{
	FreeNodes = 0;
	PlasmaPool = 0;
	MedTankPool = 0;
   MeleeTankPool = 0;
   MissileTankPool = 0;
   HeavyTankPool = 0;
   SpherePool = 0;
   FighterPool = 0;
   MissilePool = 0;
   ShieldPowerUpPool = 0;
   MissilePackPool = 0;
   TrackingMissilePackPool = 0;
   TreePool = 0;
   BuildingPool = 0;
   IonCannonPool = 0;
   TrackingMissilePool = 0;
   Tree2Pool = 0;
   Tree3Pool = 0;
   ScoutPool = 0;
   EnemyTurretPool = 0;
   FriendlyTurretPool =0;
   ConvoyPool = 0;
   MissileUpgradePool = 0;
   TrkMissileUpgradePool = 0;
   PlasmaUpgradePool = 0;
   Building2Pool = 0;
   Building3Pool = 0;
   IonFlarePool = 0;
   LaserPool = 0;
   Rock1Pool = 0;
   Rock2Pool = 0;
   Rock3Pool = 0;
   Rock4Pool = 0;
   Rock5Pool = 0;
   Rock6Pool = 0;
   BombPool = 0;
   BossPool = 0;
}

//////////////////////////////////////////////////////////////////////////
/// Destructor
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

CObjectFactory::~CObjectFactory()
{

}

void CObjectFactory::SetupLevel1Factory()
{
 // Projectiles
   int numplasmashots = 300;
   int numclustermissiles = 100;
   int numtrackingmissiles = 300;
   int numlasers = 200;
   int numioncannon = 1;
   int numionflare = 1;

   // Enemies
   int nummedtanks = 0;
   int numhvytanks = 0;
   int nummeleetanks = 0;
   int nummissiletanks = 0;
   int numscouts = 0;
   int numfighters = 6;

   // Powerups
   int numshieldpowerups = 15;
   int nummissileammo = 15;
   int numtrkmissileammo = 15;
   int nummissilepowerup = 1;
   int numtrkmissilepowerup = 1;
   int numplasmapowerup = 1;

   // Preallocate some plasma shot objects
   for(int i = 0; i < 300; i++)
   {
      CPlasmaShot *shot = new CPlasmaShot(*GameMain.m_pPlasmaObject); 
		ReleasePlasma(shot);
   }

   // Preallocate some missiles
   for(i = 0; i < 100; i++)
   {
      CMissile *missile = new CMissile(*GameMain.m_pMissileObject); 
		ReleaseMissile(missile);
   }

   // Preallocate some trk missiles
   for(i = 0; i < 300; i++)
   {
      CTrackingMissile *missile = new CTrackingMissile(*GameMain.m_pTrackingMissileObject); 
		ReleaseTrackingMissile(missile);
   }

   // Preallocate some lasers
   for(i = 0; i < 200; i++)
   {
      CLaser *laser= new CLaser(*GameMain.m_pLaserObject);  
		ReleaseLaser(laser);
   }

   // Preallocate a Ion Cannon
   for(i = 0; i < 1; i++)
   {
      CIonCannon *ioncannon = new CIonCannon(*GameMain.m_pIonCannonObject);  
		ReleaseIonCannon(ioncannon);
   }

   // Preallocate a Ion Cannon flare
   for(i = 0; i < 1; i++)
   {
      CIonFlare *ionflare = new CIonFlare(*GameMain.m_pIonFlareObject);  
		ReleaseIonFlare(ionflare);
   } 

   for(i = 0; i < nummedtanks; i++)
   {
      CMedTank *medtank = new CMedTank(*GameMain.m_pMedTankObject);
      ReleaseMedTank(medtank);
   }

   for(i = 0; i < numhvytanks; i++)
   {
      CHeavyTank *hvytank = new CHeavyTank(*GameMain.m_pHeavyTankObject);
      ReleaseHeavyTank(hvytank);
   }

   for(i = 0; i < nummeleetanks; i++)
   {
      CMeleeTank *meleetank = new CMeleeTank(*GameMain.m_pMeleeTankObject);
      ReleaseMeleeTank(meleetank);
   }

   for(i = 0; i < nummissiletanks; i++)
   {
      CMissileTank *msltank = new CMissileTank(*GameMain.m_pMissileTankObject);
      ReleaseMissileTank(msltank);
   }

   for(i = 0; i < nummissiletanks; i++)
   {
      CScout *scout = new CScout(*GameMain.m_pScoutObject);
      ReleaseScout(scout);
   }  
   
   for(i = 0; i < numfighters; i++)
   {
      CFighter *fighter = new CFighter(*GameMain.m_pFighterObject);
      ReleaseFighter(fighter);
   }

   for(i = 0; i < numshieldpowerups; i++)
   {
      CShieldPowerUp *shldpowerup = new CShieldPowerUp(*GameMain.m_pShieldPowerUpObject);
      ReleaseShieldPowerUp(shldpowerup);
   }

   for(i = 0; i < nummissileammo; i++)
   {
      CMissilePack *mslammo = new CMissilePack(*GameMain.m_pMissilePackObject);
      ReleaseMissilePack(mslammo);
   }

   for(i = 0; i < numtrkmissileammo; i++)
   {
      CTrackingMissilePack *trkmissileammo = new CTrackingMissilePack(*GameMain.m_pTrackingMissilePackObject);
      ReleaseTrackingMissilePack(trkmissileammo);
   }

   for(i = 0; i < numtrkmissilepowerup; i++)
   {
      CTrkMissileUpgrade *trkmissileup = new CTrkMissileUpgrade(*GameMain.m_pTrkMissileUpgradeObject);
      ReleaseTrkMissileUpgrade(trkmissileup);
   }

   for(i = 0; i < nummissilepowerup; i++)
   {
      CMissileUpgrade *mslup = new CMissileUpgrade(*GameMain.m_pMissileUpgradeObject);
      ReleaseMissileUpgrade(mslup);
   }

   for(i = 0; i < numplasmapowerup; i++)
   {
      CPlasmaUpgrade *plasmaup = new CPlasmaUpgrade(*GameMain.m_pPlasmaUpgradeObject);
      ReleasePlasmaUpgrade(plasmaup);
   }
}

void CObjectFactory::SetupLevel2Factory()
{
   // Projectiles
   int numplasmashots = 300;
   int numclustermissiles = 100;
   int numtrackingmissiles = 300;
   int numlasers = 200;
   int numioncannon = 1;
   int numionflare = 1;

   // Enemies
   int nummedtanks = 50;
   int numhvytanks = 30;
   int nummeleetanks = 50;
   int nummissiletanks = 30;
   int numscouts = 100;
   int numfighters = 0;

   // Powerups
   int numshieldpowerups = 30;
   int nummissileammo = 30;
   int numtrkmissileammo = 30;
   int nummissilepowerup = 3;
   int numtrkmissilepowerup = 3;
   int numplasmapowerup = 3;

   // Preallocate some plasma shot objects
   for(int i = 0; i < 300; i++)
   {
      CPlasmaShot *shot = new CPlasmaShot(*GameMain.m_pPlasmaObject); 
		ReleasePlasma(shot);
   }

   // Preallocate some missiles
   for(i = 0; i < 100; i++)
   {
      CMissile *missile = new CMissile(*GameMain.m_pMissileObject); 
		ReleaseMissile(missile);
   }

   // Preallocate some trk missiles
   for(i = 0; i < 300; i++)
   {
      CTrackingMissile *missile = new CTrackingMissile(*GameMain.m_pTrackingMissileObject); 
		ReleaseTrackingMissile(missile);
   }

   // Preallocate some lasers
   for(i = 0; i < 200; i++)
   {
      CLaser *laser= new CLaser(*GameMain.m_pLaserObject);  
		ReleaseLaser(laser);
   }

   // Preallocate a Ion Cannon
   for(i = 0; i < 1; i++)
   {
      CIonCannon *ioncannon = new CIonCannon(*GameMain.m_pIonCannonObject);  
		ReleaseIonCannon(ioncannon);
   }

   // Preallocate a Ion Cannon flare
   for(i = 0; i < 1; i++)
   {
      CIonFlare *ionflare = new CIonFlare(*GameMain.m_pIonFlareObject);  
		ReleaseIonFlare(ionflare);
   } 

   for(i = 0; i < nummedtanks; i++)
   {
      CMedTank *medtank = new CMedTank(*GameMain.m_pMedTankObject);
      ReleaseMedTank(medtank);
   }

   for(i = 0; i < numhvytanks; i++)
   {
      CHeavyTank *hvytank = new CHeavyTank(*GameMain.m_pHeavyTankObject);
      ReleaseHeavyTank(hvytank);
   }

   for(i = 0; i < nummeleetanks; i++)
   {
      CMeleeTank *meleetank = new CMeleeTank(*GameMain.m_pMeleeTankObject);
      ReleaseMeleeTank(meleetank);
   }

   for(i = 0; i < nummissiletanks; i++)
   {
      CMissileTank *msltank = new CMissileTank(*GameMain.m_pMissileTankObject);
      ReleaseMissileTank(msltank);
   }

   for(i = 0; i < nummissiletanks; i++)
   {
      CScout *scout = new CScout(*GameMain.m_pScoutObject);
      ReleaseScout(scout);
   }  
   
   for(i = 0; i < numfighters; i++)
   {
      CFighter *fighter = new CFighter(*GameMain.m_pFighterObject);
      ReleaseFighter(fighter);
   }

   for(i = 0; i < numshieldpowerups; i++)
   {
      CShieldPowerUp *shldpowerup = new CShieldPowerUp(*GameMain.m_pShieldPowerUpObject);
      ReleaseShieldPowerUp(shldpowerup);
   }

   for(i = 0; i < nummissileammo; i++)
   {
      CMissilePack *mslammo = new CMissilePack(*GameMain.m_pMissilePackObject);
      ReleaseMissilePack(mslammo);
   }

   for(i = 0; i < numtrkmissileammo; i++)
   {
      CTrackingMissilePack *trkmissileammo = new CTrackingMissilePack(*GameMain.m_pTrackingMissilePackObject);
      ReleaseTrackingMissilePack(trkmissileammo);
   }

   for(i = 0; i < numtrkmissilepowerup; i++)
   {
      CTrkMissileUpgrade *trkmissileup = new CTrkMissileUpgrade(*GameMain.m_pTrkMissileUpgradeObject);
      ReleaseTrkMissileUpgrade(trkmissileup);
   }

   for(i = 0; i < nummissilepowerup; i++)
   {
      CMissileUpgrade *mslup = new CMissileUpgrade(*GameMain.m_pMissileUpgradeObject);
      ReleaseMissileUpgrade(mslup);
   }

   for(i = 0; i < numplasmapowerup; i++)
   {
      CPlasmaUpgrade *plasmaup = new CPlasmaUpgrade(*GameMain.m_pPlasmaUpgradeObject);
      ReleasePlasmaUpgrade(plasmaup);
   }
}

void CObjectFactory::SetupLevel3Factory()
{
   // Projectiles
   int numplasmashots = 300;
   int numclustermissiles = 100;
   int numtrackingmissiles = 300;
   int numlasers = 200;
   int numioncannon = 1;
   int numionflare = 1;

   // Enemies
   int nummedtanks = 0;
   int numhvytanks = 0;
   int nummeleetanks = 0;
   int nummissiletanks = 0;
   int numscouts = 0;
   int numfighters = 60;

   // Powerups
   int numshieldpowerups = 15;
   int nummissileammo = 15;
   int numtrkmissileammo = 15;
   int nummissilepowerup = 1;
   int numtrkmissilepowerup = 1;
   int numplasmapowerup = 1;

   // Preallocate some plasma shot objects
   for(int i = 0; i < 300; i++)
   {
      CPlasmaShot *shot = new CPlasmaShot(*GameMain.m_pPlasmaObject); 
		ReleasePlasma(shot);
   }

   // Preallocate some missiles
   for(i = 0; i < 100; i++)
   {
      CMissile *missile = new CMissile(*GameMain.m_pMissileObject); 
		ReleaseMissile(missile);
   }

   // Preallocate some trk missiles
   for(i = 0; i < 300; i++)
   {
      CTrackingMissile *missile = new CTrackingMissile(*GameMain.m_pTrackingMissileObject); 
		ReleaseTrackingMissile(missile);
   }

   // Preallocate some lasers
   for(i = 0; i < 200; i++)
   {
      CLaser *laser= new CLaser(*GameMain.m_pLaserObject);  
		ReleaseLaser(laser);
   }

   // Preallocate a Ion Cannon
   for(i = 0; i < 1; i++)
   {
      CIonCannon *ioncannon = new CIonCannon(*GameMain.m_pIonCannonObject);  
		ReleaseIonCannon(ioncannon);
   }

   // Preallocate a Ion Cannon flare
   for(i = 0; i < 1; i++)
   {
      CIonFlare *ionflare = new CIonFlare(*GameMain.m_pIonFlareObject);  
		ReleaseIonFlare(ionflare);
   } 

   for(i = 0; i < nummedtanks; i++)
   {
      CMedTank *medtank = new CMedTank(*GameMain.m_pMedTankObject);
      ReleaseMedTank(medtank);
   }

   for(i = 0; i < numhvytanks; i++)
   {
      CHeavyTank *hvytank = new CHeavyTank(*GameMain.m_pHeavyTankObject);
      ReleaseHeavyTank(hvytank);
   }

   for(i = 0; i < nummeleetanks; i++)
   {
      CMeleeTank *meleetank = new CMeleeTank(*GameMain.m_pMeleeTankObject);
      ReleaseMeleeTank(meleetank);
   }

   for(i = 0; i < nummissiletanks; i++)
   {
      CMissileTank *msltank = new CMissileTank(*GameMain.m_pMissileTankObject);
      ReleaseMissileTank(msltank);
   }

   for(i = 0; i < nummissiletanks; i++)
   {
      CScout *scout = new CScout(*GameMain.m_pScoutObject);
      ReleaseScout(scout);
   }  
   
   for(i = 0; i < numfighters; i++)
   {
      CFighter *fighter = new CFighter(*GameMain.m_pFighterObject);
      ReleaseFighter(fighter);
   }

   for(i = 0; i < numshieldpowerups; i++)
   {
      CShieldPowerUp *shldpowerup = new CShieldPowerUp(*GameMain.m_pShieldPowerUpObject);
      ReleaseShieldPowerUp(shldpowerup);
   }

   for(i = 0; i < nummissileammo; i++)
   {
      CMissilePack *mslammo = new CMissilePack(*GameMain.m_pMissilePackObject);
      ReleaseMissilePack(mslammo);
   }

   for(i = 0; i < numtrkmissileammo; i++)
   {
      CTrackingMissilePack *trkmissileammo = new CTrackingMissilePack(*GameMain.m_pTrackingMissilePackObject);
      ReleaseTrackingMissilePack(trkmissileammo);
   }

   for(i = 0; i < numtrkmissilepowerup; i++)
   {
      CTrkMissileUpgrade *trkmissileup = new CTrkMissileUpgrade(*GameMain.m_pTrkMissileUpgradeObject);
      ReleaseTrkMissileUpgrade(trkmissileup);
   }

   for(i = 0; i < nummissilepowerup; i++)
   {
      CMissileUpgrade *mslup = new CMissileUpgrade(*GameMain.m_pMissileUpgradeObject);
      ReleaseMissileUpgrade(mslup);
   }

   for(i = 0; i < numplasmapowerup; i++)
   {
      CPlasmaUpgrade *plasmaup = new CPlasmaUpgrade(*GameMain.m_pPlasmaUpgradeObject);
      ReleasePlasmaUpgrade(plasmaup);
   }
}

//////////////////////////////////////////////////////////////////////////
/// Get a free node from the list, if none then make one
/// 
/// Input:     void
///
/// Returns:   PoolNode* - The node returned 
//////////////////////////////////////////////////////////////////////////

PoolNode *CObjectFactory::GetFreeNode(void)
{
	PoolNode *node = FreeNodes;
	if (!node)
		node = new PoolNode;
	else
		FreeNodes = node->next;

	return node;
}

//////////////////////////////////////////////////////////////////////////
/// Put a node back in the free node list
/// 
/// Input:     PoolNode *node - The node to add back to the free list
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseNode(PoolNode *node)
{
	node->next = FreeNodes;
	FreeNodes = node;
}

//////////////////////////////////////////////////////////////////////////
/// Kill free node list
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyFreeNodes(void)
{
	PoolNode *node;

	while ((node = FreeNodes))
	{
		FreeNodes = FreeNodes->next;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CPlasmaShot - Return object
//////////////////////////////////////////////////////////////////////////

CPlasmaShot *CObjectFactory::GetFreePlasma(void)
{
	CPlasmaShot *shot;
	PoolNode *node = PlasmaPool;

	if (!node)
		shot = 0;
	else
	{
		shot = node->obj.plasma;
		node->obj.plasma = 0;
		PlasmaPool = node->next;
		ReleaseNode(node);
	}

	return shot;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CMedTank - Return object
//////////////////////////////////////////////////////////////////////////

CMedTank *CObjectFactory::GetFreeMedTank(void)
{
	CMedTank *tank;
	PoolNode *node = MedTankPool;

	if (!node)
		tank = 0;
	else
	{
		tank = node->obj.medtank;
		node->obj.medtank = 0;
		MedTankPool = node->next;
		ReleaseNode(node);
	}

	return tank;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CMeleeTank - Return object
//////////////////////////////////////////////////////////////////////////

CMeleeTank *CObjectFactory::GetFreeMeleeTank(void)
{
	CMeleeTank *tank;
	PoolNode *node = MeleeTankPool;

	if (!node)
		tank = 0;
	else
	{
		tank = node->obj.meleetank;
		node->obj.meleetank = 0;
		MeleeTankPool = node->next;
		ReleaseNode(node);
	}

	return tank;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CMeleeTank - Return object
//////////////////////////////////////////////////////////////////////////

CMissileTank *CObjectFactory::GetFreeMissileTank(void)
{
	CMissileTank *tank;
	PoolNode *node = MissileTankPool;

	if (!node)
		tank = 0;
	else
	{
		tank = node->obj.missiletank;
		node->obj.missiletank = 0;
		MissileTankPool = node->next;
		ReleaseNode(node);
	}

	return tank;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CHeavyTank - Return object
//////////////////////////////////////////////////////////////////////////

CHeavyTank *CObjectFactory::GetFreeHeavyTank(void)
{
	CHeavyTank *tank;
	PoolNode *node = HeavyTankPool;

	if (!node)
		tank = 0;
	else
	{
		tank = node->obj.heavytank;
		node->obj.heavytank = 0;
		HeavyTankPool = node->next;
		ReleaseNode(node);
	}

	return tank;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CFighter - Return object
//////////////////////////////////////////////////////////////////////////

CFighter *CObjectFactory::GetFreeFighter(void)
{
	CFighter *fighter;
	PoolNode *node = FighterPool;

	if (!node)
		fighter = 0;
	else
	{
		fighter = node->obj.fighter;
		node->obj.fighter = 0;
		FighterPool = node->next;
		ReleaseNode(node);
	}

	return fighter;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CSphere - Return object
//////////////////////////////////////////////////////////////////////////

CSphere *CObjectFactory::GetFreeSphere(void)
{
	CSphere *sphere;
	PoolNode *node = SpherePool;

	if (!node)
		sphere = 0;
	else
	{
		sphere = node->obj.sphere;
		node->obj.sphere = 0;
		SpherePool = node->next;
		ReleaseNode(node);
	}

	return sphere;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CMissile - Return object
//////////////////////////////////////////////////////////////////////////

CMissile *CObjectFactory::GetFreeMissile(void)
{
	CMissile *missile;
	PoolNode *node = MissilePool;

	if (!node)
		missile = 0;
	else
	{
		missile = node->obj.missile;
		node->obj.missile = 0;
		MissilePool = node->next;
		ReleaseNode(node);
	}

	return missile;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CShieldPowerUp - Return object
//////////////////////////////////////////////////////////////////////////

CShieldPowerUp *CObjectFactory::GetFreeShieldPowerUp(void)
{
	CShieldPowerUp *shieldpowerup;
	PoolNode *node = ShieldPowerUpPool;

	if (!node)
		shieldpowerup = 0;
	else
	{
		shieldpowerup= node->obj.shieldpowerup;
		node->obj.shieldpowerup= 0;
		ShieldPowerUpPool = node->next;
		ReleaseNode(node);
	}

	return shieldpowerup;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CMissilePack - Return object
//////////////////////////////////////////////////////////////////////////

CMissilePack *CObjectFactory::GetFreeMissilePack(void)
{
	CMissilePack *missilepack;
	PoolNode *node = MissilePackPool;

	if (!node)
		missilepack = 0;
	else
	{
		missilepack = node->obj.missilepack;
		node->obj.missilepack= 0;
		MissilePackPool = node->next;
		ReleaseNode(node);
	}

	return missilepack;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CTrackingMissilePack - Return object
//////////////////////////////////////////////////////////////////////////

CTrackingMissilePack *CObjectFactory::GetFreeTrackingMissilePack(void)
{
	CTrackingMissilePack *trackingmissilepack;
	PoolNode *node = TrackingMissilePackPool;

	if (!node)
		trackingmissilepack = 0;
	else
	{
		trackingmissilepack = node->obj.trackingmissilepack;
		node->obj.trackingmissilepack = 0;
		TrackingMissilePackPool = node->next;
		ReleaseNode(node);
	}

	return trackingmissilepack;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CTree - Return object
//////////////////////////////////////////////////////////////////////////

CTree *CObjectFactory::GetFreeTree(void)
{
	CTree *tree;
	PoolNode *node = TreePool;

	if (!node)
		tree = 0;
	else
	{
		tree = node->obj.tree;
		node->obj.tree= 0;
		TreePool = node->next;
		ReleaseNode(node);
	}

	return tree;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CBuilding - Return object
//////////////////////////////////////////////////////////////////////////

CBuilding *CObjectFactory::GetFreeBuilding(void)
{
	CBuilding *building;
	PoolNode *node = BuildingPool;

	if (!node)
		building = 0;
	else
	{
		building = node->obj.building;
		node->obj.building= 0;
		BuildingPool = node->next;
		ReleaseNode(node);
	}

	return building;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CTrackingMissile - Return object
//////////////////////////////////////////////////////////////////////////

CTrackingMissile *CObjectFactory::GetFreeTrackingMissile(void)
{
	CTrackingMissile *trackingmissile;
	PoolNode *node = TrackingMissilePool;

	if (!node)
		trackingmissile = 0;
	else
	{
		trackingmissile = node->obj.trackingmissile;
		node->obj.trackingmissile = 0;
		TrackingMissilePool = node->next;
		ReleaseNode(node);
	}

	return trackingmissile;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CTree2 - Return object
//////////////////////////////////////////////////////////////////////////

CTree2 *CObjectFactory::GetFreeTree2(void)
{
	CTree2 *tree2;
	PoolNode *node = Tree2Pool;

	if (!node)
		tree2 = 0;
	else
	{
		tree2 = node->obj.tree2;
		node->obj.tree2 = 0;
		Tree2Pool = node->next;
		ReleaseNode(node);
	}

	return tree2;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CTree3 - Return object
//////////////////////////////////////////////////////////////////////////

CTree3 *CObjectFactory::GetFreeTree3(void)
{
	CTree3 *tree3;
	PoolNode *node = Tree3Pool;

	if (!node)
		tree3 = 0;
	else
	{
		tree3 = node->obj.tree3;
		node->obj.tree3 = 0;
		Tree3Pool = node->next;
		ReleaseNode(node);
	}

	return tree3;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CIonCannon - Return object
//////////////////////////////////////////////////////////////////////////

CIonCannon *CObjectFactory::GetFreeIonCannon(void)
{
	CIonCannon *ioncannon;
	PoolNode *node = IonCannonPool;

	if (!node)
		ioncannon = 0;
	else
	{
		ioncannon = node->obj.ioncannon;
		node->obj.ioncannon= 0;
		IonCannonPool = node->next;
		ReleaseNode(node);
	}

	return ioncannon;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CScout - Return object
//////////////////////////////////////////////////////////////////////////

CScout *CObjectFactory::GetFreeScout(void)
{
	CScout *scout;
	PoolNode *node = ScoutPool;

	if (!node)
		scout = 0;
	else
	{
		scout = node->obj.scout;
		node->obj.scout = 0;
		ScoutPool = node->next;
		ReleaseNode(node);
	}

	return scout;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CConvoy - Return object
//////////////////////////////////////////////////////////////////////////

CConvoy *CObjectFactory::GetFreeConvoy(void)
{
	CConvoy *convoy;
	PoolNode *node = ConvoyPool;

	if (!node)
		convoy = 0;
	else
	{
		convoy = node->obj.convoy;
		node->obj.convoy = 0;
		ConvoyPool = node->next;
		ReleaseNode(node);
	}

	return convoy;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CEnemyTurret - Return object
//////////////////////////////////////////////////////////////////////////

CEnemyTurret *CObjectFactory::GetFreeEnemyTurret(void)
{
	CEnemyTurret *eturret;
	PoolNode *node = EnemyTurretPool;

	if (!node)
		eturret = 0;
	else
	{
		eturret = node->obj.eturret;
		node->obj.eturret = 0;
		EnemyTurretPool = node->next;
		ReleaseNode(node);
	}

	return eturret;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CFriendlyTurret - Return object
//////////////////////////////////////////////////////////////////////////

CFriendlyTurret *CObjectFactory::GetFreeFriendlyTurret(void)
{
	CFriendlyTurret *fturret;
	PoolNode *node = FriendlyTurretPool;

	if (!node)
		fturret = 0;
	else
	{
		fturret = node->obj.fturret;
		node->obj.fturret = 0;
		FriendlyTurretPool = node->next;
		ReleaseNode(node);
	}

	return fturret;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CMissileUpgrade - Return object
//////////////////////////////////////////////////////////////////////////

CMissileUpgrade *CObjectFactory::GetFreeMissileUpgrade(void)
{
	CMissileUpgrade *missileupgrade;
	PoolNode *node = MissileUpgradePool;

	if (!node)
		missileupgrade = 0;
	else
	{
		missileupgrade = node->obj.missileupgrade;
		node->obj.missileupgrade = 0;
		MissileUpgradePool = node->next;
		ReleaseNode(node);
	}

	return missileupgrade;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CTrkMissileUpgrade - Return object
//////////////////////////////////////////////////////////////////////////

CTrkMissileUpgrade *CObjectFactory::GetFreeTrkMissileUpgrade(void)
{
	CTrkMissileUpgrade *trkmissileupgrade;
	PoolNode *node = TrkMissileUpgradePool;

	if (!node)
		trkmissileupgrade = 0;
	else
	{
		trkmissileupgrade = node->obj.trkmissileupgrade;
		node->obj.trkmissileupgrade = 0;
		TrkMissileUpgradePool = node->next;
		ReleaseNode(node);
	}

	return trkmissileupgrade;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CPlasmaUpgrade - Return object
//////////////////////////////////////////////////////////////////////////

CPlasmaUpgrade *CObjectFactory::GetFreePlasmaUpgrade(void)
{
	CPlasmaUpgrade *plasmaupgrade;
	PoolNode *node = PlasmaUpgradePool;

	if (!node)
		plasmaupgrade = 0;
	else
	{
		plasmaupgrade = node->obj.plasmaupgrade;
		node->obj.plasmaupgrade = 0;
		PlasmaUpgradePool = node->next;
		ReleaseNode(node);
	}

	return plasmaupgrade;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CBuilding2 - Return object
//////////////////////////////////////////////////////////////////////////

CBuilding2 *CObjectFactory::GetFreeBuilding2(void)
{
	CBuilding2 *building2;
	PoolNode *node = Building2Pool;

	if (!node)
		building2 = 0;
	else
	{
		building2 = node->obj.building2;
		node->obj.building2 = 0;
		Building2Pool = node->next;
		ReleaseNode(node);
	}

	return building2;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CBuilding3 - Return object
//////////////////////////////////////////////////////////////////////////

CBuilding3 *CObjectFactory::GetFreeBuilding3(void)
{
	CBuilding3 *building3;
	PoolNode *node = Building3Pool;

	if (!node)
		building3 = 0;
	else
	{
		building3 = node->obj.building3;
		node->obj.building3 = 0;
		Building3Pool = node->next;
		ReleaseNode(node);
	}

	return building3;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CIonFlare - Return object
//////////////////////////////////////////////////////////////////////////

CIonFlare *CObjectFactory::GetFreeIonFlare(void)
{
	CIonFlare *ionflare;
	PoolNode *node = IonFlarePool;

	if (!node)
		ionflare = 0;
	else
	{
		ionflare = node->obj.ionflare;
		node->obj.ionflare = 0;
		IonFlarePool = node->next;
		ReleaseNode(node);
	}

	return ionflare;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CLaser - Return object
//////////////////////////////////////////////////////////////////////////

CLaser *CObjectFactory::GetFreeLaser(void)
{
	CLaser *laser;
	PoolNode *node = LaserPool;

	if (!node)
		laser = 0;
	else
	{
		laser = node->obj.laser;
		node->obj.laser = 0;
		LaserPool = node->next;
		ReleaseNode(node);
	}

	return laser;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CRock1 - Return object
//////////////////////////////////////////////////////////////////////////

CRock1 *CObjectFactory::GetFreeRock1(void)
{
	CRock1 *rock1;
	PoolNode *node = Rock1Pool;

	if (!node)
		rock1 = 0;
	else
	{
		rock1 = node->obj.rock1;
		node->obj.rock1 = 0;
		Rock1Pool = node->next;
		ReleaseNode(node);
	}

	return rock1;
}


//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CRock2 - Return object
//////////////////////////////////////////////////////////////////////////

CRock2 *CObjectFactory::GetFreeRock2(void)
{
	CRock2 *rock2;
	PoolNode *node = Rock2Pool;

	if (!node)
		rock2 = 0;
	else
	{
		rock2 = node->obj.rock2;
		node->obj.rock2 = 0;
		Rock2Pool = node->next;
		ReleaseNode(node);
	}

	return rock2;
}


//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CRock3 - Return object
//////////////////////////////////////////////////////////////////////////

CRock3 *CObjectFactory::GetFreeRock3(void)
{
	CRock3 *rock3;
	PoolNode *node = Rock3Pool;

	if (!node)
		rock3 = 0;
	else
	{
		rock3 = node->obj.rock3;
		node->obj.rock3 = 0;
		Rock3Pool = node->next;
		ReleaseNode(node);
	}

	return rock3;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CRock4 - Return object
//////////////////////////////////////////////////////////////////////////

CRock4 *CObjectFactory::GetFreeRock4(void)
{
	CRock4 *rock4;
	PoolNode *node = Rock4Pool;

	if (!node)
		rock4 = 0;
	else
	{
		rock4 = node->obj.rock4;
		node->obj.rock4 = 0;
		Rock4Pool = node->next;
		ReleaseNode(node);
	}

	return rock4;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CRock5 - Return object
//////////////////////////////////////////////////////////////////////////

CRock5 *CObjectFactory::GetFreeRock5(void)
{
	CRock5 *rock5;
	PoolNode *node = Rock5Pool;

	if (!node)
		rock5 = 0;
	else
	{
		rock5 = node->obj.rock5;
		node->obj.rock5 = 0;
		Rock5Pool = node->next;
		ReleaseNode(node);
	}

	return rock5;
}


//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CRock6 - Return object
//////////////////////////////////////////////////////////////////////////

CRock6 *CObjectFactory::GetFreeRock6(void)
{
	CRock6 *rock6;
	PoolNode *node = Rock6Pool;

	if (!node)
		rock6 = 0;
	else
	{
		rock6 = node->obj.rock6;
		node->obj.rock6 = 0;
		Rock6Pool = node->next;
		ReleaseNode(node);
	}

	return rock6;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CBomb - Return object
//////////////////////////////////////////////////////////////////////////

CBomb *CObjectFactory::GetFreeBomb(void)
{
	CBomb *bomb;
	PoolNode *node = BombPool;

	if (!node)
		bomb = 0;
	else
	{
		bomb = node->obj.bomb;
		node->obj.bomb = 0;
		BombPool = node->next;
		ReleaseNode(node);
	}

	return bomb;
}

//////////////////////////////////////////////////////////////////////////
/// Grab a free object from the pool, if none return none
/// 
/// Input:     void
///
/// Returns:   CBoss - Return object
//////////////////////////////////////////////////////////////////////////

CBoss *CObjectFactory::GetFreeBoss(void)
{
	CBoss *boss;
	PoolNode *node = BossPool;

	if (!node)
		boss = 0;
	else
	{
		boss = node->obj.boss;
		node->obj.boss = 0;
		BossPool = node->next;
		ReleaseNode(node);
	}

	return boss;
}


//////////////////////////////////////////////////////////////////////////
/// Release an object 
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseObject(PoolNode *node, PoolNode **pool)
{
	node->next = *pool;
	*pool = node;
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyPlasmaPool(void)
{
	PoolNode *node;

	while ((node = PlasmaPool))
	{
		PlasmaPool = PlasmaPool->next;
		delete node->obj.plasma;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyMedTankPool(void)
{
	PoolNode *node;

	while ((node = MedTankPool))
	{
		MedTankPool = MedTankPool->next;
		delete node->obj.medtank;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyHeavyTankPool(void)
{
	PoolNode *node;

	while ((node = HeavyTankPool))
	{
		HeavyTankPool = HeavyTankPool->next;
		delete node->obj.heavytank;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyMissileTankPool(void)
{
	PoolNode *node;

	while ((node = MissileTankPool))
	{
		MissileTankPool = MissileTankPool->next;
		delete node->obj.missiletank;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyMeleeTankPool(void)
{
	PoolNode *node;

	while ((node = MeleeTankPool))
	{
		MeleeTankPool = MeleeTankPool->next;
		delete node->obj.meleetank;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyMissilePool(void)
{
	PoolNode *node;

	while ((node = MissilePool))
	{
		MissilePool = MissilePool->next;
		delete node->obj.missile;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroySpherePool(void)
{
	PoolNode *node;

	while ((node = SpherePool))
	{
		SpherePool = SpherePool->next;
		delete node->obj.sphere;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyFighterPool(void)
{
	PoolNode *node;

	while ((node = FighterPool))
	{
		FighterPool = FighterPool->next;
		delete node->obj.fighter;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyShieldPowerUpPool(void)
{
	PoolNode *node;

	while ((node = ShieldPowerUpPool))
	{
		ShieldPowerUpPool = ShieldPowerUpPool->next;
		delete node->obj.shieldpowerup;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyMissilePackPool(void)
{
	PoolNode *node;

	while ((node = MissilePackPool))
	{
		MissilePackPool = MissilePackPool->next;
		delete node->obj.missilepack;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyTrackingMissilePackPool(void)
{
	PoolNode *node;

	while ((node = TrackingMissilePackPool))
	{
		TrackingMissilePackPool = TrackingMissilePackPool->next;
		delete node->obj.trackingmissilepack;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyTreePool(void)
{
	PoolNode *node;

	while ((node = TreePool))
	{
		TreePool = TreePool->next;
		delete node->obj.tree;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyBuildingPool(void)
{
	PoolNode *node;

	while ((node = BuildingPool))
	{
		BuildingPool = BuildingPool->next;
		delete node->obj.building;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyIonCannonPool(void)
{
	PoolNode *node;

	while ((node = IonCannonPool))
	{
		IonCannonPool = IonCannonPool->next;
		delete node->obj.ioncannon;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyTrackingMissilePool(void)
{
	PoolNode *node;

	while ((node = TrackingMissilePool))
	{
		TrackingMissilePool = TrackingMissilePool->next;
		delete node->obj.trackingmissile;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyTree2Pool(void)
{
	PoolNode *node;

	while ((node = Tree2Pool))
	{
		Tree2Pool = Tree2Pool->next;
		delete node->obj.tree2;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyTree3Pool(void)
{
	PoolNode *node;

	while ((node = Tree3Pool))
	{
		Tree3Pool = Tree3Pool->next;
		delete node->obj.tree3;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyScoutPool(void)
{
	PoolNode *node;

	while ((node = ScoutPool))
	{
		ScoutPool = ScoutPool->next;
		delete node->obj.scout;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyConvoyPool(void)
{
	PoolNode *node;

	while ((node = ConvoyPool))
	{
		ConvoyPool = ConvoyPool->next;
		delete node->obj.convoy;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyEnemyTurretPool(void)
{
	PoolNode *node;

	while ((node = EnemyTurretPool))
	{
		EnemyTurretPool = EnemyTurretPool->next;
		delete node->obj.eturret;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyFriendlyTurretPool(void)
{
	PoolNode *node;

	while ((node = FriendlyTurretPool))
	{
		FriendlyTurretPool = FriendlyTurretPool->next;
		delete node->obj.fturret;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyMissileUpgradePool(void)
{
	PoolNode *node;

	while ((node = MissileUpgradePool))
	{
		MissileUpgradePool = MissileUpgradePool->next;
		delete node->obj.missileupgrade;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyTrkMissileUpgradePool(void)
{
	PoolNode *node;

	while ((node = TrkMissileUpgradePool))
	{
		TrkMissileUpgradePool = TrkMissileUpgradePool->next;
		delete node->obj.trkmissileupgrade;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyPlasmaUpgradePool(void)
{
	PoolNode *node;

	while ((node = PlasmaUpgradePool))
	{
		PlasmaUpgradePool = PlasmaUpgradePool->next;
		delete node->obj.plasmaupgrade;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyBuilding2Pool(void)
{
	PoolNode *node;

	while ((node = Building2Pool))
	{
		Building2Pool = Building2Pool->next;
		delete node->obj.building2;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyBuilding3Pool(void)
{
	PoolNode *node;

	while ((node = Building3Pool))
	{
		Building3Pool = Building3Pool->next;
		delete node->obj.building3;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyIonFlarePool(void)
{
	PoolNode *node;

	while ((node = IonFlarePool))
	{
		IonFlarePool = IonFlarePool->next;
		delete node->obj.ionflare;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyLaserPool(void)
{
	PoolNode *node;

	while ((node = LaserPool))
	{
		LaserPool = LaserPool->next;
		delete node->obj.laser;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyRock1Pool(void)
{
	PoolNode *node;

	while ((node = Rock1Pool))
	{
		Rock1Pool = Rock1Pool->next;
		delete node->obj.rock1;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyRock2Pool(void)
{
	PoolNode *node;

	while ((node = Rock2Pool))
	{
		Rock2Pool = Rock2Pool->next;
		delete node->obj.rock2;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyRock3Pool(void)
{
	PoolNode *node;

	while ((node = Rock3Pool))
	{
		Rock3Pool = Rock3Pool->next;
		delete node->obj.rock3;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyRock4Pool(void)
{
	PoolNode *node;

	while ((node = Rock4Pool))
	{
		Rock4Pool = Rock4Pool->next;
		delete node->obj.rock4;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyRock5Pool(void)
{
	PoolNode *node;

	while ((node = Rock5Pool))
	{
		Rock5Pool = Rock5Pool->next;
		delete node->obj.rock5;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyRock6Pool(void)
{
	PoolNode *node;

	while ((node = Rock6Pool))
	{
		Rock6Pool = Rock6Pool->next;
		delete node->obj.rock6;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyBombPool(void)
{
	PoolNode *node;

	while ((node = BombPool))
	{
		BombPool = BombPool->next;
		delete node->obj.bomb;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Cleanup nodes AND objects
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::DestroyBossPool(void)
{
	PoolNode *node;

	while ((node = BossPool))
	{
		BossPool = BossPool->next;
		delete node->obj.boss;
		delete node;
	}
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a plasma object and return it
/// 
/// Input:     void
///
/// Returns:   CPlasmaShot* - Pointer to plasma object 
//////////////////////////////////////////////////////////////////////////

CPlasmaShot * CObjectFactory::CreatePlasmaShot()
{
	CPlasmaShot *shot = GetFreePlasma();
	if (!shot)
		shot = new CPlasmaShot(*GameMain.m_pPlasmaObject);
	else
		shot->Reset();

	// TO DO:
	// Do any required setup (if any) here

	GameMain.AddToObjList(shot);
	g_Quadtree.AddObject(shot->GetRenderObj());

	return shot;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a plasma object by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CPlasmaShot* shot - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleasePlasma(CPlasmaShot * shot)
{
	// Cleanup should be done in shot->Die(), here, just release back to pool
	PoolNode *node = GetFreeNode();

	node->obj.plasma = shot;
	ReleaseObject(node, &PlasmaPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a plasma object and return it
/// 
/// Input:     void
///
/// Returns:   CPlasmaShot* - Pointer to plasma object 
//////////////////////////////////////////////////////////////////////////

CMedTank *CObjectFactory::CreateMediumTank()
{
	CMedTank *tank = GetFreeMedTank();
	if (!tank)
   {
		tank = new CMedTank(*GameMain.m_pMedTankObject);
	}
   else
		tank->Reset();

	// TO DO:
	// Do any required setup (if any) here

	GameMain.AddToObjList(tank);
	g_Quadtree.AddObject(tank->GetRenderObj());

    GameMain.m_NumEnemiesAlive++; // TEMP

	return tank;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a plasma object by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CPlasmaShot* shot - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseMedTank(CMedTank * medtank)
{
	PoolNode *node = GetFreeNode();

	node->obj.medtank = medtank;
	ReleaseObject(node, &MedTankPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a melee tank and return it
/// 
/// Input:     void
///
/// Returns:   CMeleeTank* - The created melee tank
//////////////////////////////////////////////////////////////////////////

CMeleeTank *CObjectFactory::CreateMeleeTank()
{
   CMeleeTank *tank = GetFreeMeleeTank();
   if(!tank)
   {
      tank = new CMeleeTank(*GameMain.m_pMeleeTankObject);
   }
   else
      tank->Reset();

   GameMain.AddToObjList(tank);
   g_Quadtree.AddObject(tank->GetRenderObj());

   GameMain.m_NumEnemiesAlive++; // TEMP

   return tank;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a melee tank by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CMeleeTank* meleetank - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseMeleeTank(CMeleeTank * meleetank)
{
   PoolNode *node = GetFreeNode();

   node->obj.meleetank = meleetank;
   ReleaseObject(node, &MeleeTankPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a heavy tank and return it
/// 
/// Input:     void
///
/// Returns:   CHeavyTank* - The created melee tank
//////////////////////////////////////////////////////////////////////////

CHeavyTank *CObjectFactory::CreateHeavyTank()
{
   CHeavyTank *tank = GetFreeHeavyTank();
   if(!tank)
   {
      tank = new CHeavyTank(*GameMain.m_pHeavyTankObject);
   }
   else
      tank->Reset();

   GameMain.AddToObjList(tank);
   g_Quadtree.AddObject(tank->GetRenderObj());

   GameMain.m_NumEnemiesAlive++; // TEMP

   return tank;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a heavy tank by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CHeavyTank* heavytank - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseHeavyTank(CHeavyTank * heavytank)
{
   PoolNode *node = GetFreeNode();

   node->obj.heavytank = heavytank;
   ReleaseObject(node, &HeavyTankPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a missile tank and return it
/// 
/// Input:     void
///
/// Returns:   CMissileTank* - The created missile tank
//////////////////////////////////////////////////////////////////////////

CMissileTank *CObjectFactory::CreateMissileTank()
{
   CMissileTank *tank = GetFreeMissileTank();
   if(!tank)
   {
      tank = new CMissileTank(*GameMain.m_pMissileTankObject);
   }
   else
      tank->Reset();

   GameMain.AddToObjList(tank);
   g_Quadtree.AddObject(tank->GetRenderObj());

   GameMain.m_NumEnemiesAlive++; // TEMP

   return tank;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a missile tank by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CMissileTank* missiletank - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseMissileTank(CMissileTank * missiletank)
{
   PoolNode *node = GetFreeNode();

   node->obj.missiletank = missiletank;
   ReleaseObject(node, &MissileTankPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a sphere and return it
/// 
/// Input:     void
///
/// Returns:   CSphere* - The created sphere
//////////////////////////////////////////////////////////////////////////

CSphere *CObjectFactory::CreateSphere()
{
   CSphere *sphere = GetFreeSphere();
   if(!sphere)
   {
      sphere = new CSphere(*GameMain.m_pSphereObject);
   }
   else
      sphere->Reset();

   GameMain.AddToObjList(sphere);
   g_Quadtree.AddObject(sphere->GetRenderObj());

   //GameMain.m_NumEnemiesAlive++; // TEMP

   return sphere;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a sphere object by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CSphere* sphere - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseSphere(CSphere * sphere)
{
   PoolNode *node = GetFreeNode();

   node->obj.sphere = sphere;
   ReleaseObject(node, &SpherePool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a missile and return it
/// 
/// Input:     void
///
/// Returns:   CMissile* - The created missile
//////////////////////////////////////////////////////////////////////////

CMissile *CObjectFactory::CreateMissile()
{
   CMissile *missile = GetFreeMissile();
   if(!missile)
   {
      missile = new CMissile(*GameMain.m_pMissileObject);
   }
   else
      missile->Reset();

   GameMain.AddToObjList(missile);
   g_Quadtree.AddObject(missile->GetRenderObj());

   return missile;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a missile by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CMissile* missile - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseMissile(CMissile * missile)
{
   PoolNode *node = GetFreeNode();

   node->obj.missile = missile;
   ReleaseObject(node, &MissilePool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a fighter and return it
/// 
/// Input:     void
///
/// Returns:   CFighter* - The created fighter
//////////////////////////////////////////////////////////////////////////

CFighter *CObjectFactory::CreateFighter()
{
   CFighter *fighter = GetFreeFighter();
   if(!fighter)
   {
      fighter = new CFighter(*GameMain.m_pFighterObject);
   }
   else
      fighter->Reset();

   GameMain.AddToObjList(fighter);
   GameMain.m_NumEnemiesAlive++; // TEMP
   g_Quadtree.AddObject(fighter->GetRenderObj());

   return fighter;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a fighter by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CFighter* fighter - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseFighter(CFighter * fighter)
{
   PoolNode *node = GetFreeNode();

   node->obj.fighter = fighter;
   ReleaseObject(node, &FighterPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a ShieldPowerUp and return it
/// 
/// Input:     void
///
/// Returns:   CShieldPowerUp* - The created ShieldPowerUp
//////////////////////////////////////////////////////////////////////////

CShieldPowerUp *CObjectFactory::CreateShieldPowerUp()
{
   CShieldPowerUp *shieldpowerup = GetFreeShieldPowerUp();
   if(!shieldpowerup)
   {
      shieldpowerup = new CShieldPowerUp(*GameMain.m_pShieldPowerUpObject);
   }
   else
      shieldpowerup->Reset();

   GameMain.AddToObjList(shieldpowerup);
   g_Quadtree.AddObject(shieldpowerup->GetRenderObj());

   return shieldpowerup;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a ShieldPowerUp by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CShieldPowerUp* ShieldPowerUp - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseShieldPowerUp(CShieldPowerUp * shieldpowerup)
{
   PoolNode *node = GetFreeNode();

   node->obj.shieldpowerup = shieldpowerup;
   ReleaseObject(node, &ShieldPowerUpPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a TrackingMissilePack and return it
/// 
/// Input:     void
///
/// Returns:   CTrackingMissilePack* - The created TrackingMissilePack
//////////////////////////////////////////////////////////////////////////

CTrackingMissilePack *CObjectFactory::CreateTrackingMissilePack()
{
   CTrackingMissilePack *trackingmissilepack = GetFreeTrackingMissilePack();
   if(!trackingmissilepack)
   {
      trackingmissilepack = new CTrackingMissilePack(*GameMain.m_pTrackingMissilePackObject);
   }
   else
      trackingmissilepack->Reset();

   GameMain.AddToObjList(trackingmissilepack);
   g_Quadtree.AddObject(trackingmissilepack->GetRenderObj());

   return trackingmissilepack;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a TrackingMissilePack by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CTrackingMissilePack* TrackingMissilePack - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseTrackingMissilePack(CTrackingMissilePack * trackingmissilepack)
{
   PoolNode *node = GetFreeNode();

   node->obj.trackingmissilepack = trackingmissilepack;
   ReleaseObject(node, &TrackingMissilePackPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a MissilePack and return it
/// 
/// Input:     void
///
/// Returns:   CMissilePack* - The created MissilePack
//////////////////////////////////////////////////////////////////////////

CMissilePack *CObjectFactory::CreateMissilePack()
{
   CMissilePack *missilepack = GetFreeMissilePack();
   if(!missilepack)
   {
      missilepack = new CMissilePack(*GameMain.m_pMissilePackObject);
   }
   else
      missilepack->Reset();

   GameMain.AddToObjList(missilepack);
   g_Quadtree.AddObject(missilepack->GetRenderObj());

   return missilepack;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a MissilePack by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CMissilePack* MissilePack - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseMissilePack(CMissilePack * missilepack)
{
   PoolNode *node = GetFreeNode();

   node->obj.missilepack = missilepack;
   ReleaseObject(node, &MissilePackPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Tree and return it
/// 
/// Input:     void
///
/// Returns:   CTree* - The created Tree
//////////////////////////////////////////////////////////////////////////

CTree *CObjectFactory::CreateTree()
{
   CTree *tree = GetFreeTree();
   if(!tree)
   {
      tree = new CTree(*GameMain.m_pTreeObject);
   }
   else
      tree->Reset();

   GameMain.AddToObjList(tree);
   g_Quadtree.AddObject(tree->GetRenderObj());

   return tree;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Tree by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CTree* Tree - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseTree(CTree * tree)
{
   PoolNode *node = GetFreeNode();

   node->obj.tree = tree;
   ReleaseObject(node, &TreePool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Building and return it
/// 
/// Input:     void
///
/// Returns:   CBuilding* - The created Building
//////////////////////////////////////////////////////////////////////////

CBuilding *CObjectFactory::CreateBuilding()
{
   CBuilding *building = GetFreeBuilding();
   if(!building)
   {
      building = new CBuilding(*GameMain.m_pBuildingObject);
   }
   else
      building->Reset();

   GameMain.AddToObjList(building);

   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(building);
   g_Quadtree.AddObject(building->GetRenderObj());

   return building;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CBuilding* Building - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseBuilding(CBuilding * building)
{
   PoolNode *node = GetFreeNode();

   node->obj.building = building;
   ReleaseObject(node, &BuildingPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a IonCannon and return it
/// 
/// Input:     void
///
/// Returns:   CIonCannon * - The created IonCannon
//////////////////////////////////////////////////////////////////////////

CIonCannon *CObjectFactory::CreateIonCannon()
{
   CIonCannon *ioncannon = GetFreeIonCannon();
   if(!ioncannon)
   {
      ioncannon = new CIonCannon(*GameMain.m_pIonCannonObject);
   }
   else
      ioncannon->Reset();

   GameMain.AddToObjList(ioncannon);
   g_Quadtree.AddObject(ioncannon->GetRenderObj());

   return ioncannon;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CIonCannon* Building - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseIonCannon(CIonCannon * ioncannon)
{
   PoolNode *node = GetFreeNode();

   node->obj.ioncannon = ioncannon;
   ReleaseObject(node, &IonCannonPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a TrackingMissile and return it
/// 
/// Input:     void
///
/// Returns:   CTrackingMissile * - The created TrackingMissile
//////////////////////////////////////////////////////////////////////////

CTrackingMissile *CObjectFactory::CreateTrackingMissile()
{
   CTrackingMissile *trackingmissile = GetFreeTrackingMissile();
   if(!trackingmissile)
   {
      trackingmissile = new CTrackingMissile(*GameMain.m_pTrackingMissileObject);
   }
   else
      trackingmissile->Reset();

   GameMain.AddToObjList(trackingmissile);
   g_Quadtree.AddObject(trackingmissile->GetRenderObj());

   return trackingmissile;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CTrackingMissile* Building - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseTrackingMissile(CTrackingMissile * trackingmissile)
{
   PoolNode *node = GetFreeNode();

   node->obj.trackingmissile = trackingmissile;
   ReleaseObject(node, &TrackingMissilePool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Tree2 and return it
/// 
/// Input:     void
///
/// Returns:   CTree2 * - The created Tree2
//////////////////////////////////////////////////////////////////////////

CTree2 *CObjectFactory::CreateTree2()
{
   CTree2 *tree2 = GetFreeTree2();
   if(!tree2)
   {
      tree2 = new CTree2(*GameMain.m_pTree2Object);
   }
   else
      tree2->Reset();

   GameMain.AddToObjList(tree2);
   g_Quadtree.AddObject(tree2->GetRenderObj());

   return tree2;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CTree2* Building - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseTree2(CTree2 * tree2)
{
   PoolNode *node = GetFreeNode();

   node->obj.tree2 = tree2;
   ReleaseObject(node, &Tree2Pool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Tree3 and return it
/// 
/// Input:     void
///
/// Returns:   CTree3 * - The created Tree3
//////////////////////////////////////////////////////////////////////////

CTree3 *CObjectFactory::CreateTree3()
{
   CTree3 *tree3 = GetFreeTree3();
   if(!tree3)
   {
      tree3 = new CTree3(*GameMain.m_pTree3Object);
   }
   else
      tree3->Reset();

   GameMain.AddToObjList(tree3);
   g_Quadtree.AddObject(tree3->GetRenderObj());

   return tree3;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a  by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CTree2* Building - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseTree3(CTree3 * tree3)
{
   PoolNode *node = GetFreeNode();

   node->obj.tree3 = tree3;
   ReleaseObject(node, &Tree3Pool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Scout and return it
/// 
/// Input:     void
///
/// Returns:   CScout * - The created Scout
//////////////////////////////////////////////////////////////////////////

CScout *CObjectFactory::CreateScout()
{
   CScout *scout = GetFreeScout();
   if(!scout)
   {
      scout = new CScout(*GameMain.m_pScoutObject);
   }
   else
      scout->Reset();

   GameMain.AddToObjList(scout);
   g_Quadtree.AddObject(scout->GetRenderObj());

   GameMain.m_NumEnemiesAlive++; // TEMP
   return scout;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CScout* scout - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseScout(CScout * scout)
{
   PoolNode *node = GetFreeNode();

   node->obj.scout = scout;
   ReleaseObject(node, &ScoutPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Convoy and return it
/// 
/// Input:     void
///
/// Returns:   CConvoy * - The created Convoy
//////////////////////////////////////////////////////////////////////////

CConvoy *CObjectFactory::CreateConvoy()
{
   CConvoy *convoy = GetFreeConvoy();
   if(!convoy)
   {
      convoy = new CConvoy(*GameMain.m_pConvoyObject);
   }
   else
      convoy->Reset();

   GameMain.AddToObjList(convoy);
   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(convoy);
   g_Quadtree.AddObject(convoy->GetRenderObj());

   return convoy;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CConvoy* convoy - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseConvoy(CConvoy * convoy)
{
   PoolNode *node = GetFreeNode();

   node->obj.convoy = convoy;
   ReleaseObject(node, &ConvoyPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a EnemyTurret and return it
/// 
/// Input:     void
///
/// Returns:   CEnemyTurret * - The created EnemyTurret
//////////////////////////////////////////////////////////////////////////

CEnemyTurret *CObjectFactory::CreateEnemyTurret()
{
   CEnemyTurret *eturret = GetFreeEnemyTurret();
   if(!eturret)
   {
      eturret = new CEnemyTurret(*GameMain.m_pEnemyTurretObject);
   }
   else
      eturret->Reset();

   GameMain.AddToObjList(eturret);
   GameMain.m_NumEnemiesAlive++; // TEMP
   g_Quadtree.AddObject(eturret->GetRenderObj());

   return eturret;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CEnemyTurret* eturret - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseEnemyTurret(CEnemyTurret * eturret)
{
   PoolNode *node = GetFreeNode();

   node->obj.eturret = eturret;
   ReleaseObject(node, &EnemyTurretPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a FriendlyTurret and return it
/// 
/// Input:     void
///
/// Returns:   CFriendlyTurret * - The created FriendlyTurret
//////////////////////////////////////////////////////////////////////////

CFriendlyTurret *CObjectFactory::CreateFriendlyTurret()
{
   CFriendlyTurret *fturret = GetFreeFriendlyTurret();
   if(!fturret)
   {
      fturret = new CFriendlyTurret(*GameMain.m_pFriendlyTurretObject);
   }
   else
      fturret->Reset();

   GameMain.AddToObjList(fturret);
   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(fturret);
   g_Quadtree.AddObject(fturret->GetRenderObj());

   return fturret;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CFriendlyTurret* fturret - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseFriendlyTurret(CFriendlyTurret * fturret)
{
   PoolNode *node = GetFreeNode();

   node->obj.fturret = fturret;
   ReleaseObject(node, &FriendlyTurretPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a MissileUpgrade and return it
/// 
/// Input:     void
///
/// Returns:   CMissileUpgrade * - The created MissileUpgrade
//////////////////////////////////////////////////////////////////////////

CMissileUpgrade *CObjectFactory::CreateMissileUpgrade()
{
   CMissileUpgrade *missileupgrade = GetFreeMissileUpgrade();
   if(!missileupgrade)
   {
      missileupgrade = new CMissileUpgrade(*GameMain.m_pMissileUpgradeObject);
   }
   else
      missileupgrade->Reset();

   GameMain.AddToObjList(missileupgrade);
   g_Quadtree.AddObject(missileupgrade->GetRenderObj());

   return missileupgrade;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CMissileUpgrade* missileupgrade - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseMissileUpgrade(CMissileUpgrade * missileupgrade)
{
   PoolNode *node = GetFreeNode();

   node->obj.missileupgrade = missileupgrade;
   ReleaseObject(node, &MissileUpgradePool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a TrkMissileUpgrade and return it
/// 
/// Input:     void
///
/// Returns:   CTrkMissileUpgrade * - The created TrkMissileUpgrade
//////////////////////////////////////////////////////////////////////////

CTrkMissileUpgrade *CObjectFactory::CreateTrkMissileUpgrade()
{
   CTrkMissileUpgrade *trkmissileupgrade = GetFreeTrkMissileUpgrade();
   if(!trkmissileupgrade)
   {
      trkmissileupgrade = new CTrkMissileUpgrade(*GameMain.m_pTrkMissileUpgradeObject);
   }
   else
      trkmissileupgrade->Reset();

   GameMain.AddToObjList(trkmissileupgrade);
   g_Quadtree.AddObject(trkmissileupgrade->GetRenderObj());

   return trkmissileupgrade;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CTrkMissileUpgrade* trkmissileupgrade - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseTrkMissileUpgrade(CTrkMissileUpgrade * trkmissileupgrade)
{
   PoolNode *node = GetFreeNode();

   node->obj.trkmissileupgrade = trkmissileupgrade;
   ReleaseObject(node, &TrkMissileUpgradePool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a PlasmaUpgrade and return it
/// 
/// Input:     void
///
/// Returns:   CPlasmaUpgrade * - The created PlasmaUpgrade
//////////////////////////////////////////////////////////////////////////

CPlasmaUpgrade *CObjectFactory::CreatePlasmaUpgrade()
{
   CPlasmaUpgrade *plasmaupgrade = GetFreePlasmaUpgrade();
   if(!plasmaupgrade)
   {
      plasmaupgrade = new CPlasmaUpgrade(*GameMain.m_pPlasmaUpgradeObject);
   }
   else
      plasmaupgrade->Reset();

   GameMain.AddToObjList(plasmaupgrade);
   g_Quadtree.AddObject(plasmaupgrade->GetRenderObj());

   return plasmaupgrade;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CPlasmaUpgrade* plasmaupgrade - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleasePlasmaUpgrade(CPlasmaUpgrade * plasmaupgrade)
{
   PoolNode *node = GetFreeNode();

   node->obj.plasmaupgrade = plasmaupgrade;
   ReleaseObject(node, &PlasmaUpgradePool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Building2 and return it
/// 
/// Input:     void
///
/// Returns:   CBuilding2 * - The created Building2
//////////////////////////////////////////////////////////////////////////

CBuilding2 *CObjectFactory::CreateBuilding2()
{
   CBuilding2 *building2 = GetFreeBuilding2();
   if(!building2)
   {
      building2 = new CBuilding2(*GameMain.m_pBuildingObject2);
   }
   else
      building2->Reset();

   GameMain.AddToObjList(building2);
   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(building2);
   g_Quadtree.AddObject(building2->GetRenderObj());

   return building2;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CBuilding2* building2 - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseBuilding2(CBuilding2 * building2)
{
   PoolNode *node = GetFreeNode();

   node->obj.building2 = building2;
   ReleaseObject(node, &Building2Pool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Building3 and return it
/// 
/// Input:     void
///
/// Returns:   CBuilding3 * - The created Building3
//////////////////////////////////////////////////////////////////////////

CBuilding3 *CObjectFactory::CreateBuilding3()
{
   CBuilding3 *building3 = GetFreeBuilding3();
   if(!building3)
   {
      building3 = new CBuilding3(*GameMain.m_pBuildingObject3);
   }
   else
      building3->Reset();

   GameMain.AddToObjList(building3);
   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(building3);
   g_Quadtree.AddObject(building3->GetRenderObj());

   return building3;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CBuilding3* building3 - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseBuilding3(CBuilding3 * building3)
{
   PoolNode *node = GetFreeNode();

   node->obj.building3 = building3;
   ReleaseObject(node, &Building3Pool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a IonFlare and return it
/// 
/// Input:     void
///
/// Returns:   CIonFlare * - The created IonFlare
//////////////////////////////////////////////////////////////////////////

CIonFlare *CObjectFactory::CreateIonFlare()
{
   CIonFlare *IonFlare = GetFreeIonFlare();
   if(!IonFlare)
   {
      IonFlare = new CIonFlare(*GameMain.m_pIonFlareObject);
   }
   else
      IonFlare->Reset();

   GameMain.AddToObjList(IonFlare);
   g_Quadtree.AddObject(IonFlare->GetRenderObj());

   return IonFlare;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CIonFlare* IonFlare - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseIonFlare(CIonFlare * IonFlare)
{
   PoolNode *node = GetFreeNode();

   node->obj.ionflare = IonFlare;
   ReleaseObject(node, &IonFlarePool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Laser and return it
/// 
/// Input:     void
///
/// Returns:   CLaser * - The created Laser
//////////////////////////////////////////////////////////////////////////

CLaser *CObjectFactory::CreateLaser()
{
   CLaser *laser = GetFreeLaser();
   if(!laser)
   {
      laser = new CLaser(*GameMain.m_pLaserObject);
   }
   else
      laser->Reset();

   GameMain.AddToObjList(laser);
   g_Quadtree.AddObject(laser->GetRenderObj());

   return laser;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CLaser* Laser - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseLaser(CLaser * laser)
{
   PoolNode *node = GetFreeNode();
   node->obj.laser = laser;
   ReleaseObject(node, &LaserPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Rock1 and return it
/// 
/// Input:     void
///
/// Returns:   CRock1 * - The created Rock1
//////////////////////////////////////////////////////////////////////////

CRock1 *CObjectFactory::CreateRock1()
{
   CRock1 *rock1 = GetFreeRock1();
   if(!rock1)
   {
      rock1 = new CRock1(*GameMain.m_pRock1Object);
   }
   else
      rock1->Reset();

   GameMain.AddToObjList(rock1);
   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(rock1);
   g_Quadtree.AddObject(rock1->GetRenderObj());

   return rock1;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CRock1* Rock1 - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseRock1(CRock1 * rock1)
{
   PoolNode *node = GetFreeNode();
   node->obj.rock1 = rock1;
   ReleaseObject(node, &Rock1Pool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Rock2 and return it
/// 
/// Input:     void
///
/// Returns:   CRock2 * - The created Rock2
//////////////////////////////////////////////////////////////////////////

CRock2 *CObjectFactory::CreateRock2()
{
   CRock2 *rock2 = GetFreeRock2();
   if(!rock2)
   {
      rock2 = new CRock2(*GameMain.m_pRock2Object);
   }
   else
      rock2->Reset();

   GameMain.AddToObjList(rock2);
   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(rock2);
   g_Quadtree.AddObject(rock2->GetRenderObj());

   return rock2;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CRock2* Rock2 - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseRock2(CRock2 * rock2)
{
   PoolNode *node = GetFreeNode();
   node->obj.rock2 = rock2;
   ReleaseObject(node, &Rock2Pool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Rock3 and return it
/// 
/// Input:     void
///
/// Returns:   CRock3 * - The created Rock3
//////////////////////////////////////////////////////////////////////////

CRock3 *CObjectFactory::CreateRock3()
{
   CRock3 *rock3 = GetFreeRock3();
   if(!rock3)
   {
      rock3 = new CRock3(*GameMain.m_pRock3Object);
   }
   else
      rock3->Reset();

   GameMain.AddToObjList(rock3);
   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(rock3);
   g_Quadtree.AddObject(rock3->GetRenderObj());

   return rock3;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CRock3* Rock3 - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseRock3(CRock3 * rock3)
{
   PoolNode *node = GetFreeNode();
   node->obj.rock3 = rock3;
   ReleaseObject(node, &Rock3Pool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Rock4 and return it
/// 
/// Input:     void
///
/// Returns:   CRock4 * - The created Rock4
//////////////////////////////////////////////////////////////////////////

CRock4 *CObjectFactory::CreateRock4()
{
   CRock4 *rock4 = GetFreeRock4();
   if(!rock4)
   {
      rock4 = new CRock4(*GameMain.m_pRock4Object);
   }
   else
      rock4->Reset();

   GameMain.AddToObjList(rock4);
   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(rock4);
   g_Quadtree.AddObject(rock4->GetRenderObj());

   return rock4;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CRock4* Rock4 - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseRock4(CRock4 * rock4)
{
   PoolNode *node = GetFreeNode();
   node->obj.rock4 = rock4;
   ReleaseObject(node, &Rock4Pool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Rock5 and return it
/// 
/// Input:     void
///
/// Returns:   CRock5 * - The created Rock5
//////////////////////////////////////////////////////////////////////////

CRock5 *CObjectFactory::CreateRock5()
{
   CRock5 *rock5 = GetFreeRock5();
   if(!rock5)
   {
      rock5 = new CRock5(*GameMain.m_pRock5Object);
   }
   else
      rock5->Reset();

   GameMain.AddToObjList(rock5);
   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(rock5);
   g_Quadtree.AddObject(rock5->GetRenderObj());

   return rock5;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CRock5* Rock5 - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseRock5(CRock5 * rock5)
{
   PoolNode *node = GetFreeNode();
   node->obj.rock5 = rock5;
   ReleaseObject(node, &Rock5Pool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Rock6 and return it
/// 
/// Input:     void
///
/// Returns:   CRock6 * - The created Rock6
//////////////////////////////////////////////////////////////////////////

CRock6 *CObjectFactory::CreateRock6()
{
   CRock6 *rock6 = GetFreeRock6();
   if(!rock6)
   {
      rock6 = new CRock6(*GameMain.m_pRock6Object);
   }
   else
      rock6->Reset();

   GameMain.AddToObjList(rock6);
   if (g_GameOptions.GetAI())
      GameMain.AddToCollisionAvoidanceList(rock6);
   g_Quadtree.AddObject(rock6->GetRenderObj());

   return rock6;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CRock6* Rock6 - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseRock6(CRock6 * rock6)
{
   PoolNode *node = GetFreeNode();
   node->obj.rock6 = rock6;
   ReleaseObject(node, &Rock6Pool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Boss and return it
/// 
/// Input:     void
///
/// Returns:   CBoss * - The created Boss
//////////////////////////////////////////////////////////////////////////

CBoss *CObjectFactory::CreateBoss()
{
   CBoss *boss = GetFreeBoss();
   if(!boss)
   {
      boss = new CBoss(*GameMain.m_pBossObject);
   }
   else
      boss->Reset();

   GameMain.AddToObjList(boss);
   g_Quadtree.AddObject(boss->GetRenderObj());

   return boss;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CBoss* Boss - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseBoss(CBoss * boss)
{
   PoolNode *node = GetFreeNode();
   node->obj.boss = boss;
   ReleaseObject(node, &BossPool);
}

//////////////////////////////////////////////////////////////////////////
/// Obtain allocated memory for a Bomb and return it
/// 
/// Input:     void
///
/// Returns:   CBomb * - The created Bomb
//////////////////////////////////////////////////////////////////////////

CBomb *CObjectFactory::CreateBomb()
{
   CBomb *bomb = GetFreeBomb();
   if(!bomb)
   {
      bomb = new CBomb(*GameMain.m_pBombObject);
   }
   else
      bomb->Reset();

   GameMain.AddToObjList(bomb);
   g_Quadtree.AddObject(bomb->GetRenderObj());

   return bomb;
}

//////////////////////////////////////////////////////////////////////////
/// Destroy a Building by removing it from object list and putting 
/// back into the pool.
/// 
/// Input:     CBomb* Bomb - Object to destroy
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReleaseBomb(CBomb * bomb)
{
   PoolNode *node = GetFreeNode();
   node->obj.bomb = bomb;
   ReleaseObject(node, &BombPool);
}

//////////////////////////////////////////////////////////////////////////
/// Release an object back into the pool
/// 
/// Input:     CGameObject *obj - the object to release
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::ReturnAndNoRefund(CGameObject *obj)
{
   // Determine the type of object and release it
	switch (obj->GetType())
	{
	case OBJ_PLASMA:
		{
			CPlasmaShot *temp = (CPlasmaShot*)obj;
			ReleasePlasma(temp);
			break;
		}
   case OBJ_MISSILE:
		{
			CMissile *temp = (CMissile*)obj;
			ReleaseMissile(temp);
			break;
		}
	case OBJ_MEDTANK:
		{
			CMedTank *temp = (CMedTank*)obj;
			ReleaseMedTank(temp);
			break;
		}
	case OBJ_KAMITANK:
		{
			CMeleeTank *temp = (CMeleeTank*)obj;
			ReleaseMeleeTank(temp);
			break;
		}
   case OBJ_HVYTANK:
		{
			CHeavyTank *temp = (CHeavyTank*)obj;
			ReleaseHeavyTank(temp);
			break;
		}
   case OBJ_MSLETANK:
		{
			CMissileTank *temp = (CMissileTank*)obj;
			ReleaseMissileTank(temp);
			break;
		}
	case OBJ_SPHERE:
		{
			CSphere *temp = (CSphere*)obj;
			ReleaseSphere(temp);
			break;
		}
	case OBJ_SHDPWRUP:
		{
			CShieldPowerUp *temp = (CShieldPowerUp*)obj;
			ReleaseShieldPowerUp(temp);
			break;
		}
   case OBJ_MSLPACK:
		{
			CMissilePack *temp = (CMissilePack*)obj;
			ReleaseMissilePack(temp);
			break;
		}
   case OBJ_TRKMSLPACK:
		{
			CTrackingMissilePack *temp = (CTrackingMissilePack*)obj;
			ReleaseTrackingMissilePack(temp);
			break;
		}
   case OBJ_TREE:
		{
			CTree *temp = (CTree*)obj;
			ReleaseTree(temp);
			break;
		}
   case OBJ_BUILDING:
		{
			CBuilding *temp = (CBuilding*)obj;
			ReleaseBuilding(temp);
			break;
		}
   case OBJ_IONCANNON:
		{
			CIonCannon *temp = (CIonCannon*)obj;
			ReleaseIonCannon(temp);
			break;
		}
   case OBJ_TRKMISSILE:
		{
			CTrackingMissile *temp = (CTrackingMissile*)obj;
			ReleaseTrackingMissile(temp);
			break;
		}
   case OBJ_TREE2:
		{
			CTree2 *temp = (CTree2*)obj;
			ReleaseTree2(temp);
			break;
		}
   case OBJ_TREE3:
		{
			CTree3 *temp = (CTree3*)obj;
			ReleaseTree3(temp);
			break;
		}
   case OBJ_SCOUT:
		{
			CScout *temp = (CScout*)obj;
			ReleaseScout(temp);
			break;
		}
   case OBJ_CONVOY:
		{
			CConvoy *temp = (CConvoy*)obj;
			ReleaseConvoy(temp);
			break;
		}
   case OBJ_ETURRET:
		{
			CEnemyTurret *temp = (CEnemyTurret*)obj;
			ReleaseEnemyTurret(temp);
			break;
		}
   case OBJ_MSLPWRUP:
		{
			CMissileUpgrade *temp = (CMissileUpgrade*)obj;
			ReleaseMissileUpgrade(temp);
			break;
		}
   case OBJ_TRKMSLPWRUP:
		{
			CTrkMissileUpgrade *temp = (CTrkMissileUpgrade*)obj;
			ReleaseTrkMissileUpgrade(temp);
			break;
		}
   case OBJ_PLASMAPWRUP:
		{
			CPlasmaUpgrade *temp = (CPlasmaUpgrade*)obj;
			ReleasePlasmaUpgrade(temp);
			break;
		}
   case OBJ_BUILDING2:
		{
			CBuilding2 *temp = (CBuilding2*)obj;
			ReleaseBuilding2(temp);
			break;
		}
   case OBJ_BUILDING3:
		{
			CBuilding3 *temp = (CBuilding3*)obj;
			ReleaseBuilding3(temp);
			break;
		}
   case OBJ_IONFLARE:
      {
         CIonFlare *temp = (CIonFlare*)obj;
         ReleaseIonFlare(temp);
         break;
      }
   case OBJ_LASER:
      {
         CLaser *temp = (CLaser*)obj;
         ReleaseLaser(temp);
         break;
      }
   case OBJ_ROCK1:
	  {
		 CRock1 *temp = (CRock1*)obj;
		 ReleaseRock1(temp);
		 break;
	  }
   case OBJ_ROCK2:
	  {
		 CRock2 *temp = (CRock2*)obj;
		 ReleaseRock2(temp);
		 break;
	  }
   case OBJ_ROCK3:
	  {
		 CRock3 *temp = (CRock3*)obj;
		 ReleaseRock3(temp);
		 break;
	  }
   case OBJ_ROCK4:
	  {
		 CRock4 *temp = (CRock4*)obj;
		 ReleaseRock4(temp);
		 break;
	  }
   case OBJ_ROCK5:
	  {
		 CRock5 *temp = (CRock5*)obj;
		 ReleaseRock5(temp);
		 break;
	  }
   case OBJ_ROCK6:
	  {
		 CRock6 *temp = (CRock6*)obj;
		 ReleaseRock6(temp);
		 break;
	  }
   case OBJ_BOMB:
	  {
		 CBomb *temp = (CBomb*)obj;
		 ReleaseBomb(temp);
		 break;
	  }
   case OBJ_BOSS:
      {
         CBoss *temp = (CBoss*)obj;
         ReleaseBoss(temp);
         break;
      }
    case OBJ_FIGHTER:
      {
         CFighter *temp = (CFighter*)obj;
         ReleaseFighter(temp);
         break;
      }
    case OBJ_FTURRET:
      {
        CFriendlyTurret *temp = (CFriendlyTurret*)obj;
        ReleaseFriendlyTurret(temp);
        break;
      }
    default:
        bool debug = true;
   }
}

//////////////////////////////////////////////////////////////////////////
/// Shutdown all the memory allocated in this module
/// 
/// Input:     void
///
/// Returns:   void
//////////////////////////////////////////////////////////////////////////

void CObjectFactory::Shutdown(void)
{
	DestroyPlasmaPool();
	DestroyMedTankPool();
   DestroyHeavyTankPool();
   DestroyMissilePackPool();
   DestroyMissilePool();
   DestroyMeleeTankPool();
   DestroyMissileTankPool();
   DestroyFighterPool();
   DestroySpherePool();
   DestroyShieldPowerUpPool();
   DestroyTrackingMissilePackPool();
   DestroyMissilePool();
   DestroyTreePool();
   DestroyBuildingPool();
   DestroyIonCannonPool();
   DestroyTrackingMissilePool();
   DestroyTree2Pool();
   DestroyTree3Pool();
   DestroyScoutPool();
   DestroyConvoyPool();
   DestroyEnemyTurretPool();
   DestroyFriendlyTurretPool();
   DestroyMissileUpgradePool();
   DestroyTrkMissileUpgradePool();
   DestroyPlasmaUpgradePool();
   DestroyBuildingPool();
   DestroyBuilding2Pool();
   DestroyBuilding3Pool();
   DestroyIonFlarePool();
   DestroyLaserPool();
   DestroyRock1Pool();
   DestroyRock2Pool();
   DestroyRock3Pool();
   DestroyRock4Pool();
   DestroyRock5Pool();
   DestroyRock6Pool();
   DestroyBombPool();
   DestroyBossPool();
	DestroyFreeNodes();
}





















