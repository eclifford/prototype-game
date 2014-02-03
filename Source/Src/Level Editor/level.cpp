#include <cstdio>

#include "level editor\level.h"
#include "graphics\ptgfile.h"
#include "graphics\quadtree.h"
#include "graphics\terrain.h"
#include "core\gamemain.h"
#include "gamelogic\objectfactory.h"
#include "level editor\editormain.h"
#include "utils\utils.h"
#include "math3d\intersection.h"
#include "gamelogic\player.h"

#include "utils\mmgr.h"

extern CTerrain* g_pCurrLevel;
extern CQuadtree g_Quadtree;
extern CGameMain GameMain;
extern CObjectFactory g_ObjectFactory;
extern CFrustum g_Frustum;
extern CEditorMain EditorMain;
extern CPlayer g_Player;

static const char* pPTLFileHeader = "PTLFILE";
static const unsigned char PTLFileHeaderSize = 8;

bool LoadRandomLevel(const RandomLevelInfo* pInfo)
{
   if (!pInfo)
   {
      // generate the terrain
      unsigned int Size = 513;
      float* pHeights = new float[Size * Size];
      memset(pHeights, 0, Size * Size * 4);
      MakeDiamondSquareTerrain(pHeights, 512, 2, 1.0f, RandomFloatRange(.6f, 1.0f));
      FilterHeightField(pHeights, 513, RandomFloatRange(0.3f, .7f));
   
      g_pCurrLevel->Initialize(pHeights, Size, "resources\\images\\grass11.bmp", 17, 32, 16,
                               4.0f, 400.0f, 4.0f);

      delete [] pHeights;

      AABB TerrainAABB;
      g_pCurrLevel->GetAABB(&TerrainAABB);
      CVector2f Max;
      Max.x = TerrainAABB.Max.x;
      Max.y = TerrainAABB.Max.z;
      CVector2f Min;
      Min.x = TerrainAABB.Min.x;
      Min.y = TerrainAABB.Min.z;
      g_Quadtree.Initialize(&Min, &Max, TerrainAABB.Min.y - 100.0f, TerrainAABB.Max.y + 100.0f, 6,
                           &g_Frustum, GameMain.m_pRenderList);         

      g_pCurrLevel->AddPatchesToQuadtree(&g_Quadtree);

      unsigned int NumTrees = rand() % 301 + 300;
      float Offset = 50.0f;
      AABB TerrainBox;
      g_pCurrLevel->GetAABB(&TerrainBox);
      unsigned int NumCollisions = 0;

       NumCollisions = 0;
      // generate building 1s
      unsigned int NumBuilding1s = 0;
      unsigned int i;
      for (i = 0; i < NumBuilding1s; i++)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateBuilding();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2.0f), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
      
         

         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && SphereToSphere(pGameObject->GetSphere(),
                         pCurr->GetSphere()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               GameMain.RemoveFromCollisionAvoidanceList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumBuilding1s;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }
         
         if (i == NumBuilding1s)
            continue;

         float FlattenSize = pGameObject->GetSphere()->Radius; 
         Min.x = -FlattenSize + pGameObject->GetSphere()->Center.x;
         Min.y = -FlattenSize + pGameObject->GetSphere()->Center.z;

         Max.x = FlattenSize + pGameObject->GetSphere()->Center.x;
         Max.y = FlattenSize + pGameObject->GetSphere()->Center.z;

         g_pCurrLevel->FlattenSquare(&Min, &Max);
      
         // loop through all the game objects and place them
         // oriented above the terrain                        
         CGameObject* pCurrentObject = GameMain.m_ObjList;
         while (pCurrentObject)
         {
            pCurrentObject->FollowTerrain();

            pCurrentObject = pCurrentObject->m_Next;
         }

         pGameObject->Setup();
      }

      NumCollisions = 0;
      // generate building 3s
      unsigned int NumBuilding3s = 0;
      for (i = 0; i < NumBuilding3s; i++)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateBuilding3();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2.0f), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
      
         

         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && SphereToSphere(pGameObject->GetSphere(),
                         pCurr->GetSphere()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               GameMain.RemoveFromCollisionAvoidanceList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumBuilding3s;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }

         if (i == NumBuilding3s)
            continue;

         float FlattenSize = pGameObject->GetSphere()->Radius; 
         Min.x = -FlattenSize + pGameObject->GetSphere()->Center.x;
         Min.y = -FlattenSize + pGameObject->GetSphere()->Center.z;

         Max.x = FlattenSize + pGameObject->GetSphere()->Center.x;
         Max.y = FlattenSize + pGameObject->GetSphere()->Center.z;

         g_pCurrLevel->FlattenSquare(&Min, &Max);
      
         // loop through all the game objects and place them
         // oriented above the terrain                        
         CGameObject* pCurrentObject = GameMain.m_ObjList;
         while (pCurrentObject)
         {
            pCurrentObject->FollowTerrain();

            pCurrentObject = pCurrentObject->m_Next;
         }

         pGameObject->Setup();
      }

      NumCollisions = 0;
      // generate building 2s
      unsigned int NumBuilding2s = 0;
      for (i = 0; i < NumBuilding2s; i++)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateBuilding2();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2.0f), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
      
         

         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && SphereToSphere(pGameObject->GetSphere(),
                         pCurr->GetSphere()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               GameMain.RemoveFromCollisionAvoidanceList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumBuilding2s;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }

         if (i == NumBuilding2s)
            continue;

         float FlattenSize = pGameObject->GetSphere()->Radius; 
         Min.x = -FlattenSize + pGameObject->GetSphere()->Center.x;
         Min.y = -FlattenSize + pGameObject->GetSphere()->Center.z;

         Max.x = FlattenSize + pGameObject->GetSphere()->Center.x;
         Max.y = FlattenSize + pGameObject->GetSphere()->Center.z;

         g_pCurrLevel->FlattenSquare(&Min, &Max);
      
         // loop through all the game objects and place them
         // oriented above the terrain                        
         CGameObject* pCurrentObject = GameMain.m_ObjList;
         while (pCurrentObject)
         {
            pCurrentObject->FollowTerrain();

            pCurrentObject = pCurrentObject->m_Next;
         }

         pGameObject->Setup();
      }

      NumCollisions = 0;
      // generate friendly turrets
      unsigned int NumFriendlyTurrets = rand() % 5;
      for (i = 0; i < NumFriendlyTurrets; i++)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateFriendlyTurret();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2.0f), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f + 500.0f,
                                           TerrainBox.Max.x - Radius - 50.0f - 500.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f + 500.0f,
                                           TerrainBox.Max.z - Radius - 50.0f - 500.0f));

         pGameObject->SetPosition(&Position, 0);
      
         

         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && SphereToSphere(pGameObject->GetSphere(),
                         pCurr->GetSphere()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               GameMain.RemoveFromCollisionAvoidanceList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumFriendlyTurrets;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }

         if (i == NumFriendlyTurrets)
            continue;

         float FlattenSize = pGameObject->GetSphere()->Radius; 
         Min.x = -FlattenSize + pGameObject->GetSphere()->Center.x;
         Min.y = -FlattenSize + pGameObject->GetSphere()->Center.z;

         Max.x = FlattenSize + pGameObject->GetSphere()->Center.x;
         Max.y = FlattenSize + pGameObject->GetSphere()->Center.z;

         g_pCurrLevel->FlattenSquare(&Min, &Max);
      
         // loop through all the game objects and place them
         // oriented above the terrain                        
         CGameObject* pCurrentObject = GameMain.m_ObjList;
         while (pCurrentObject)
         {
            pCurrentObject->FollowTerrain();

            pCurrentObject = pCurrentObject->m_Next;
         }

         pGameObject->Setup();
      }

      NumCollisions = 0;
      // generate enemy turrets
      unsigned int NumEnemyTurrets = rand() % 10;
      for (i = 0; i < NumEnemyTurrets; i++)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateEnemyTurret();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2.0f), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
      
         

         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && SphereToSphere(pGameObject->GetSphere(),
                         pCurr->GetSphere()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               //GameMain.RemoveFromCollisionAvoidanceList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumEnemyTurrets;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }

         if (i == NumEnemyTurrets)
            continue;

         float FlattenSize = pGameObject->GetSphere()->Radius; 
         Min.x = -FlattenSize + pGameObject->GetSphere()->Center.x;
         Min.y = -FlattenSize + pGameObject->GetSphere()->Center.z;

         Max.x = FlattenSize + pGameObject->GetSphere()->Center.x;
         Max.y = FlattenSize + pGameObject->GetSphere()->Center.z;

         g_pCurrLevel->FlattenSquare(&Min, &Max);
      
         // loop through all the game objects and place them
         // oriented above the terrain                        
         CGameObject* pCurrentObject = GameMain.m_ObjList;
         while (pCurrentObject)
         {
            pCurrentObject->FollowTerrain();

            pCurrentObject = pCurrentObject->m_Next;
         }

         pGameObject->Setup();
      }

      // generate the trees
      for (i = 0; i < NumTrees; i++)
      {
         int TreeType = rand() % 3;
         CGameObject* pGameObject;

         switch (TreeType)
         {
            case 0:
               pGameObject = g_ObjectFactory.CreateTree();
               break;
            case 1:
               pGameObject = g_ObjectFactory.CreateTree2();
               break;
            case 2:
               pGameObject = g_ObjectFactory.CreateTree3();
               break;   
         }

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
      
         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && OBBToOBB(pGameObject->GetOBB(),
                         pCurr->GetOBB()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > NumTrees)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumTrees;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }

         if (i == NumTrees)
            continue;

         pGameObject->Setup();
      }

      NumCollisions = 0;
      // generate medium tanks
      unsigned int NumMedTanks = rand() % 11 + 10;
      for (i = 0; i < NumMedTanks; i++)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateMediumTank();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
      
         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && OBBToOBB(pGameObject->GetOBB(),
                         pCurr->GetOBB()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumMedTanks;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }

         if (i == NumMedTanks)
            continue;

         pGameObject->Setup();
      }

      NumCollisions = 0;
      // generate kamikaze tanks
      unsigned int NumKamikazeTanks = rand() % 11 + 10;
      for (i = 0; i < NumKamikazeTanks; ++i)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateMeleeTank();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
      
         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && OBBToOBB(pGameObject->GetOBB(),
                         pCurr->GetOBB()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumMedTanks;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }

         if (i == NumKamikazeTanks)
            continue;

         pGameObject->Setup();
      }

      NumCollisions = 0;
      // generate flyers tanks
      unsigned int NumFlyers = rand() % 11 + 5;
      for (i = 0; i < NumFlyers; ++i)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateFighter();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
      
         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && OBBToOBB(pGameObject->GetOBB(),
                         pCurr->GetOBB()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumFlyers;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }

         if (i == NumFlyers)
            continue;

         pGameObject->Setup();

         pGameObject->Awaken();

         pGameObject->MoveUp(150.0f, 0);
      }

      NumCollisions = 0;
      // generate scout tanks
      unsigned int NumScoutTanks = rand() % 26 + 10;
      for (i = 0; i < NumScoutTanks; i++)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateScout();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
                                                         
         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && OBBToOBB(pGameObject->GetOBB(),
                         pCurr->GetOBB()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumScoutTanks;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }

         if (i == NumScoutTanks)
            continue;

         pGameObject->Setup();
      }

      NumCollisions = 0;
      // generate heavy tanks
      unsigned int NumHeavyTanks = rand() % 11 + 5;
      for (i = 0; i < NumHeavyTanks; i++)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateHeavyTank();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
      
         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && OBBToOBB(pGameObject->GetOBB(),
                         pCurr->GetOBB()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumHeavyTanks;
               }

               break;                           
            }
         
            pCurr = pCurr->m_Next;   
         }

         if (i == NumHeavyTanks)
            continue;

         pGameObject->Setup();
      }

      NumCollisions = 0;
      // generate missile tanks
      unsigned int NumMissileTanks = rand() % 11 + 5;
      for (i = 0; i < NumMissileTanks; i++)
      {
         CGameObject* pGameObject = g_ObjectFactory.CreateMissileTank();

         
         float Radius = pGameObject->GetSphere()->Radius;
         pGameObject->RotateLocalY(RandomFloatRange(0.0f, PI * 2), 0);
         
         Point3f Position(RandomFloatRange(TerrainBox.Min.x + Radius + 50.0f,
                                           TerrainBox.Max.x - Radius - 50.0f),
                                           0.0f,
                          RandomFloatRange(TerrainBox.Min.z + Radius + 50.0f,
                                           TerrainBox.Max.z - Radius - 50.0f));

         pGameObject->SetPosition(&Position, 0);
      
         pGameObject->FollowTerrain();
      
         // check to make sure we aren't colliding with anything

         CGameObject* pCurr = GameMain.m_ObjList;
         while (pCurr)
         {
            if (pCurr != pGameObject && OBBToOBB(pGameObject->GetOBB(),
                         pCurr->GetOBB()))
            {
               GameMain.RemoveFromObjList(pGameObject);
               pGameObject->RemoveFromQuadtree();
               
               g_ObjectFactory.ReturnAndNoRefund(pGameObject);

               --i;

               NumCollisions++;

               if (NumCollisions > 500)
               {
                  //MessageBox(NULL, "Cannot place more trees.", "Error", MB_OK);
                  i = NumMissileTanks;
               }

               break;                           
            }
            
            pCurr = pCurr->m_Next;   
         }

         if (i == NumMissileTanks)
            continue;

         pGameObject->Setup();
      }
   }

   return true;
}

bool SaveLevel(const char* pPTLFilePath, unsigned int NumObjects)
{
   FILE* pFile = fopen(pPTLFilePath, "wb");

   if (!pFile)
      return false;

   // write out the header
   fwrite(pPTLFileHeader, PTLFileHeaderSize, 1, pFile);

   // write out the number of objects
   fwrite(&NumObjects, sizeof(unsigned int), 1, pFile);

   // write out the object types and their frames
   CGameObject* pGameObject = GameMain.m_ObjList;
   int Type;
   while (pGameObject)
	{
      Type = pGameObject->GetType();
      fwrite(&Type, sizeof(int), 1, pFile);
      fwrite(pGameObject->GetFrame(0), sizeof(CObjFrame), 1, pFile);      
		pGameObject = pGameObject->m_Next;
	}

   // write out the min and max y
   float MinY = g_Quadtree.GetMinY();
   fwrite(&MinY, sizeof(MinY), 1, pFile);
   
   float MaxY = g_Quadtree.GetMaxY();
   fwrite(&MaxY, sizeof(MaxY), 1, pFile);

   // write out the terrain
   g_pCurrLevel->SaveToFile(pFile);
   
   // done saving the map
   fclose(pFile);

   return true;
}

struct SLevelObject
{
   int m_Type;
   CObjFrame m_Frame;
};

bool LoadLevel(const char* pPTLFilePath, CRenderList* pRenderList,
               unsigned int* pNumObjects)
{
   FILE* pFile = fopen(pPTLFilePath, "rb");

   // read in the header
   char Header[PTLFileHeaderSize];
   fread(Header, PTLFileHeaderSize, 1, pFile);
   if (strcmp(Header, pPTLFileHeader))
   {
      MessageBox(NULL, "Error loading level file.", "Loading Error", MB_OK);
      fclose(pFile);
      return false;
   } 

   // read in the number of objects
   unsigned int NumObjects;
   fread(&NumObjects, sizeof(NumObjects), 1, pFile);

   // if they passed in a valid num objects pointer, then give them the num objects
   if (pNumObjects)
      *pNumObjects = NumObjects;

   // read in the object types and their frames
   SLevelObject* pLevelObjects = new SLevelObject[NumObjects];
   for (unsigned int i = 0; i < NumObjects; ++i)
   {
      //read in the type
      fread(&pLevelObjects[i].m_Type, sizeof(int), 1, pFile);
      
      // read in the frame
      fread(&pLevelObjects[i].m_Frame, sizeof(CObjFrame), 1, pFile);      
	}
   
   // read in the min and max y
   float MinY;
   fread(&MinY, sizeof(float), 1, pFile);
   float MaxY;
   fread(&MaxY, sizeof(float), 1, pFile);

   // read in the terrain
   g_pCurrLevel->Initialize(pFile);

   // done loading the level file
   fclose(pFile);

   // load the quadtree 
   AABB TerrainAABB;
   g_pCurrLevel->GetAABB(&TerrainAABB);
   
   CVector2f Max;
   Max.x = TerrainAABB.Max.x;
   Max.y = TerrainAABB.Max.z;
   
   CVector2f Min;
   Min.x = TerrainAABB.Min.x;
   Min.y = TerrainAABB.Min.z;
   
   g_Quadtree.Initialize(&Min, &Max, MinY, MaxY, 7, &g_Frustum, pRenderList);         
   
   g_pCurrLevel->AddPatchesToQuadtree(&g_Quadtree);

   CGameObject* pGameObject;
   for (i = 0; i < NumObjects; ++i)
   {   	
      // create the object based on its type
      switch (pLevelObjects[i].m_Type)
      {
         case OBJ_PLAYER:
             g_Quadtree.AddObject(g_Player.GetRenderObj());
             GameMain.AddToObjList(&g_Player);
             EditorMain.m_PlayerStart = true;
             pGameObject = &g_Player;
             break;
         case OBJ_MEDTANK:
            pGameObject = g_ObjectFactory.CreateMediumTank();  
            break;
         case OBJ_HVYTANK:
            pGameObject = g_ObjectFactory.CreateHeavyTank();
            break;
         case OBJ_MSLETANK:
            pGameObject = g_ObjectFactory.CreateMissileTank();  
            break;
         case OBJ_KAMITANK:
            pGameObject = g_ObjectFactory.CreateMeleeTank();  
            break;
         case OBJ_SPHERE:
            pGameObject = g_ObjectFactory.CreateSphere();
            break;
         case OBJ_FTURRET:
            pGameObject = g_ObjectFactory.CreateFriendlyTurret();
            break;
         case OBJ_ETURRET:
            pGameObject = g_ObjectFactory.CreateEnemyTurret();  
            break;
         case OBJ_TREE:
            pGameObject = g_ObjectFactory.CreateTree();  
            break;
         case OBJ_TREE2:
            pGameObject = g_ObjectFactory.CreateTree2();  
            break;
         case OBJ_TREE3:
            pGameObject = g_ObjectFactory.CreateTree3();  
            break;
         case OBJ_BUILDING:
            pGameObject = g_ObjectFactory.CreateBuilding();  
            break;
         case OBJ_BUILDING2:
            pGameObject = g_ObjectFactory.CreateBuilding2();
            break;
         case OBJ_BUILDING3:
            pGameObject = g_ObjectFactory.CreateBuilding3();
            break;
         case OBJ_SHDPWRUP:
            pGameObject = g_ObjectFactory.CreateShieldPowerUp();
            break;
         case OBJ_TRKMSLPWRUP:
            pGameObject = g_ObjectFactory.CreateTrkMissileUpgrade();
            break;
         case OBJ_MSLPWRUP:
            pGameObject = g_ObjectFactory.CreateMissileUpgrade();
            break;
         case OBJ_PLASMAPWRUP:
            pGameObject = g_ObjectFactory.CreatePlasmaUpgrade();
            break; 
         case OBJ_FIGHTER:
            pGameObject = g_ObjectFactory.CreateFighter();
            break;
         case OBJ_MSLPACK:
            pGameObject = g_ObjectFactory.CreateMissilePack();
            break;
         case OBJ_TRKMSLPACK:
            pGameObject = g_ObjectFactory.CreateTrackingMissilePack();
            break;
         case OBJ_SCOUT:
            pGameObject = g_ObjectFactory.CreateScout();
            break;
         case OBJ_ROCK1:
            pGameObject = g_ObjectFactory.CreateRock1();
            break;
         case OBJ_ROCK2:
            pGameObject = g_ObjectFactory.CreateRock2();
            break;
         case OBJ_ROCK3:
            pGameObject = g_ObjectFactory.CreateRock3();
            break;
         case OBJ_ROCK4:
            pGameObject = g_ObjectFactory.CreateRock4();
            break;
         case OBJ_ROCK5:
            pGameObject = g_ObjectFactory.CreateRock5();
            break;
         case OBJ_ROCK6:
            pGameObject = g_ObjectFactory.CreateRock6();
            break;

      }

      pGameObject->SetFrame(&pLevelObjects[i].m_Frame, 0);
	}

   delete [] pLevelObjects;

   return true;
}

void FilterHeightBand( float* fpBand, int iStride, int iCount, float fFilter )
{
	float v= fpBand[0];
	int j  = iStride;
	int i;

	//go through the height band and apply the erosion filter
	for( i=0; i<iCount-1; i++ )
	{
		fpBand[j]= fFilter*v + ( 1-fFilter )*fpBand[j];
		
		v = fpBand[j];
		j+= iStride;
	}
}

//--------------------------------------------------------------
// Name:			CTERRAIN::FilterHeightfTempBuffer - private
// Description:		Apply the erosion filter to an entire buffer
//					of height values
// Arguments:		-fpHeightData: the height values to be filtered
//					-fFilter: the filter strength
// Return Value:	None
//--------------------------------------------------------------
void FilterHeightField( float* fpHeightData, int m_iSize, float fFilter )
{
	int i;

	//erode left to right
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[m_iSize*i], 1, m_iSize, fFilter );
	
	//erode right to left
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[m_iSize*i+m_iSize-1], -1, m_iSize, fFilter );

	//erode top to bottom
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[i], m_iSize, m_iSize, fFilter);

	//erode from bottom to top
	for( i=0; i<m_iSize; i++ )
		FilterHeightBand( &fpHeightData[m_iSize*(m_iSize-1)+i], -m_iSize, m_iSize, fFilter );
}


static float avgDiamondVals (int i, int j, int stride,
			     int size, int subSize, float *fa)
{
    /* In this diagram, our input stride is 1, the i,j location is
       indicated by "X", and the four value we want to average are
       "*"s:
           .   *   .

           *   X   *

           .   *   .
       */

    /* In order to support tiled surfaces which meet seamless at the
       edges (that is, they "wrap"), We need to be careful how we
       calculate averages when the i,j diamond center lies on an edge
       of the array. The first four 'if' clauses handle these
       cases. The final 'else' clause handles the general case (in
       which i,j is not on an edge).
     */
    if (i == 0)
	return ((float) (fa[(i*size) + j-stride] +
			 fa[(i*size) + j+stride] +
			 fa[((subSize-stride)*size) + j] +
			 fa[((i+stride)*size) + j]) * .25f);
    else if (i == size-1)
	return ((float) (fa[(i*size) + j-stride] +
			 fa[(i*size) + j+stride] +
			 fa[((i-stride)*size) + j] +
			 fa[((0+stride)*size) + j]) * .25f);
    else if (j == 0)
	return ((float) (fa[((i-stride)*size) + j] +
			 fa[((i+stride)*size) + j] +
			 fa[(i*size) + j+stride] +
			 fa[(i*size) + subSize-stride]) * .25f);
    else if (j == size-1)
	return ((float) (fa[((i-stride)*size) + j] +
			 fa[((i+stride)*size) + j] +
			 fa[(i*size) + j-stride] +
			 fa[(i*size) + 0+stride]) * .25f);
    else
	return ((float) (fa[((i-stride)*size) + j] +
			 fa[((i+stride)*size) + j] +
			 fa[(i*size) + j-stride] +
			 fa[(i*size) + j+stride]) * .25f);
}

static float avgSquareVals (int i, int j, int stride, int size, float *fa)
{
    /* In this diagram, our input stride is 1, the i,j location is
       indicated by "*", and the four value we want to average are
       "X"s:
           X   .   X

           .   *   .

           X   .   X
       */
    return ((float) (fa[((i-stride)*size) + j-stride] +
		     fa[((i-stride)*size) + j+stride] +
		     fa[((i+stride)*size) + j-stride] +
		     fa[((i+stride)*size) + j+stride]) * .25f);
}

void MakeDiamondSquareTerrain(float *fa, int size, int seedValue, float heightScale,
                              float h)
{
    int	i, j;
    int	stride;
    int	oddline;
    int subSize;
	float ratio, scale;


    /* subSize is the dimension of the array in terms of connected line
       segments, while size is the dimension in terms of number of
       vertices. */
    subSize = size;
    size++;
    
    /* initialize random number generator */
    //srandom (seedValue);
    

	/* Set up our roughness constants.
	   Random numbers are always generated in the range 0.0 to 1.0.
	   'scale' is multiplied by the randum number.
	   'ratio' is multiplied by 'scale' after each iteration
	   to effectively reduce the randum number range.
	   */
	ratio = (float) pow (2.f,-h);
	scale = heightScale * ratio;

    /* Seed the first four values. For example, in a 4x4 array, we
       would initialize the data points indicated by '*':

           *   .   .   .   *

           .   .   .   .   .

           .   .   .   .   .

           .   .   .   .   .

           *   .   .   .   *

       In terms of the "diamond-square" algorithm, this gives us
       "squares".

       We want the four corners of the array to have the same
       point. This will allow us to tile the arrays next to each other
       such that they join seemlessly. */

    stride = subSize / 2;
    
    /*
    fa[(0*size)+0] =
      fa[(subSize*size)+0] =
        fa[(subSize*size)+subSize] =
          fa[(0*size)+subSize] = 0.f;
    */

    memset(fa, 0, size * size);

    /* Now we add ever-increasing detail based on the "diamond" seeded
       values. We loop over stride, which gets cut in half at the
       bottom of the loop. Since it's an int, eventually division by 2
       will produce a zero result, terminating the loop. */
    while (stride) {
		/* Take the existing "square" data and produce "diamond"
		   data. On the first pass through with a 4x4 matrix, the
		   existing data is shown as "X"s, and we need to generate the
	       "*" now:

               X   .   .   .   X

               .   .   .   .   .

               .   .   *   .   .

               .   .   .   .   .

               X   .   .   .   X

	      It doesn't look like diamonds. What it actually is, for the
	      first pass, is the corners of four diamonds meeting at the
	      center of the array. */
		for (i=stride; i<subSize; i+=stride) {
			for (j=stride; j<subSize; j+=stride) {
				fa[(i * size) + j] =
					scale * RandomFloatRange(-.5f, .5f) +
					avgSquareVals (i, j, stride, size, fa);
				j += stride;
			}
			i += stride;
		}


		/* Take the existing "diamond" data and make it into
	       "squares". Back to our 4X4 example: The first time we
	       encounter this code, the existing values are represented by
	       "X"s, and the values we want to generate here are "*"s:

               X   .   *   .   X

               .   .   .   .   .

               *   .   X   .   *

               .   .   .   .   .

               X   .   *   .   X

	       i and j represent our (x,y) position in the array. The
	       first value we want to generate is at (i=2,j=0), and we use
	       "oddline" and "stride" to increment j to the desired value.
	       */
		oddline = 0;
		for (i=0; i<subSize; i+=stride) {
		    oddline = (oddline == 0);
			for (j=0; j<subSize; j+=stride) {
				if ((oddline) && !j) j+=stride;

				/* i and j are setup. Call avgDiamondVals with the
				   current position. It will return the average of the
				   surrounding diamond data points. */
				fa[(i * size) + j] =
					scale * RandomFloatRange(-.5f, .5f) +
					avgDiamondVals (i, j, stride, size, subSize, fa);

				/* To wrap edges seamlessly, copy edge values around
				   to other side of array */
				if (i==0)
					fa[(subSize*size) + j] =
						fa[(i * size) + j];
				if (j==0)
					fa[(i*size) + subSize] =
						fa[(i * size) + j];

				j+=stride;
			}
		}


		/* reduce random number range. */
		scale *= ratio;
		stride >>= 1;
    }


}