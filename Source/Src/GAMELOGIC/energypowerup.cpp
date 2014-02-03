/// \file energypowerup.cpp
/// The energy powerup
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/energypowerup.h"

CEnergyPowerUp::CEnergyPowerUp()
{

}

CEnergyPowerUp::~CEnergyPowerUp()
{

}

void CEnergyPowerUp::Setup(int energyRestored)
{
   m_Active = true;
	m_Type = OBJ_ENGYPWRUP;
   m_AmountRestored = energyRestored;
}

void CEnergyPowerUp::Update()
{
   
}

void CEnergyPowerUp::Collide(const CGameObject *obj)
{
   // Object is the terrain 
   if(!obj)
   {
      // DO NOTHING
   }
   
   // Object is a game object
   else
   {
      switch(obj->GetType())
      {
         case OBJ_PLAYER:
         {
            // Kill the powerup
            Die();         
         }
         break;
      } 
   } 
}

void CEnergyPowerUp::Die(void)
{
   m_Active = false;
   RemoveFromQuadtree();
}

void CEnergyPowerUp::Reset(void)
{
   
}