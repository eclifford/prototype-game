/// \file projectile.cpp 
/// The base class for all projectiles in the game
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/projectile.h"
#include "core/gamemain.h"
#include "math3d/intersection.h"
#include "gamelogic/player.h"

#include "utils\mmgr.h"

extern CGameMain GameMain;
extern CPlayer g_Player;

CProjectile::CProjectile()
{

}

CProjectile::~CProjectile()
{

}

void CProjectile::AreaOfEffectDamage(float radius, float damage)
{
   CGameObject *obj = GameMain.m_ObjList;
   const Sphere* pProjectileSphere = GetSphere();
   radius = radius * radius;

	// If object is active, update then go to next
	while (obj)
	{
		if (obj->IsActive())
		{
         const Sphere* pObjectSphere = obj->GetSphere();
         
         // Get the distance bewteen the two objects
         float  Dist = DistanceSquare(&pProjectileSphere->Center, &(pObjectSphere->Center));

         // We want the outside of the bounding volume
         Dist -= (pObjectSphere->Radius * pObjectSphere->Radius); 
         if(Dist <= 0)
            Dist = 0;
         
         int Type = obj->GetType();
         if(Dist <= radius)
         {
            if(m_ShotOwner == ENEMY_SHOT || m_ShotOwner == ION_CANNON)
            {
               if (Type == OBJ_PLAYER || Type == OBJ_CONVOY || Type == OBJ_FTURRET || Type == OBJ_BUILDING || Type == OBJ_BUILDING2 || Type == OBJ_BUILDING3)
               {
                    float ratio = 0.0f;
                      if(Dist == 0)
                         ratio = 0.0f;
                      else
                         ratio = Dist / radius;
                      float tempdamage = damage;
                      tempdamage -= damage * ratio;		 
                    
                    // special case for player  
                    if (Type == OBJ_PLAYER)
                    {
                        if(m_ShotOwner == ION_CANNON)
                        {
                            CPlayer* player = (CPlayer*)obj;
                            player->IonHitsShield((int)tempdamage);
                        }
                        else
                            obj->TakeDamage(tempdamage, m_ProjectileOwner);
                    }
                    // all other cases
                    else
                        obj->TakeDamage(tempdamage, m_ProjectileOwner);
                      
               }
            }   
            if (m_ShotOwner == PLAYER_SHOT || m_ShotOwner == FRIENDLY_SHOT || m_ShotOwner == ION_CANNON)
            {
                if (Type == OBJ_MEDTANK || Type == OBJ_HVYTANK || Type == OBJ_BOSS || Type ==  OBJ_KAMITANK || Type == OBJ_SCOUT || Type == OBJ_MSLETANK ||
                    Type == OBJ_SPHERE || Type == OBJ_FIGHTER || Type == OBJ_ETURRET)
                {
                    float ratio = 0.0f;
                      if(Dist == 0)
                         ratio = 0.0f;
                      else
                         ratio = Dist / radius;
                      float tempdamage = damage;
                      tempdamage -= damage * ratio;
                        obj->TakeDamage(tempdamage, m_ProjectileOwner);
                }
               
            }
            
            // all shots damage trees
            if (Type == OBJ_TREE || Type == OBJ_TREE2 || Type == OBJ_TREE3)
            {
                float ratio = 0.0f;
                if(Dist == 0)
                    ratio = 0.0f;
                else
                    ratio = Dist / radius;
                float tempdamage = damage;
                tempdamage -= damage * ratio;
                obj->TakeDamage(tempdamage, m_ProjectileOwner);
            }
             
         }
         obj = obj->m_Next; 
		}
		else
		{
         obj = obj->m_Next;  
		}
	}
}
