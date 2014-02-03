/// \file rock.h
/// A stationary rock
//////////////////////////////////////////////////////////////////////////

#include "gamelogic/rock.h"

#include "utils\mmgr.h"

void CRock1::Setup(void)
{
   m_Active = true;
   m_RadarType = RADAR_ROCK;
}

void CRock1::Update(void)
{
   FollowTerrain();
   CGameObject::Update();
}

void CRock1::Collide(const CGameObject *obj)
{
   // Object is the terrain 
   if(!obj)
   {
      // DO NOTHING
   }
   
   // Object is a game object
   else
   {

   } 
}

void CRock1::Reset(void)
{
   
}

void CRock2::Setup(void)
{
   m_Active = true;
   m_RadarType = RADAR_ROCK;
}

void CRock2::Update(void)
{
   FollowTerrain();
   CGameObject::Update();
}

void CRock2::Collide(const CGameObject *obj)
{
   // Object is the terrain 
   if(!obj)
   {
      // DO NOTHING
   }
   
   // Object is a game object
   else
   {

   } 
}

void CRock2::Reset(void)
{
   
}

void CRock3::Setup(void)
{
   m_Active = true;
   m_RadarType = RADAR_ROCK;
}

void CRock3::Update(void)
{
   FollowTerrain();
   CGameObject::Update();
}

void CRock3::Collide(const CGameObject *obj)
{
   // Object is the terrain 
   if(!obj)
   {
      // DO NOTHING
   }
   
   // Object is a game object
   else
   {

   } 
}

void CRock3::Reset(void)
{
   
}

void CRock4::Setup(void)
{
   m_Active = true;
   m_RadarType = RADAR_ROCK;
}

void CRock4::Update(void)
{
   FollowTerrain();
   CGameObject::Update();
}

void CRock4::Collide(const CGameObject *obj)
{
   // Object is the terrain 
   if(!obj)
   {
      // DO NOTHING
   }
   
   // Object is a game object
   else
   {

   } 
}

void CRock4::Reset(void)
{
   
}

void CRock5::Setup(void)
{
   m_Active = true;
   m_RadarType = RADAR_ROCK;
}

void CRock5::Update(void)
{
   FollowTerrain();
   CGameObject::Update();
}

void CRock5::Collide(const CGameObject *obj)
{
   // Object is the terrain 
   if(!obj)
   {
      // DO NOTHING
   }
   
   // Object is a game object
   else
   {

   } 
}

void CRock5::Reset(void)
{
   
}

void CRock6::Setup(void)
{
   m_Active = true;
   m_RadarType = RADAR_ROCK;
}

void CRock6::Update(void)
{
   FollowTerrain();
   CGameObject::Update();
}

void CRock6::Collide(const CGameObject *obj)
{
   // Object is the terrain 
   if(!obj)
   {
      // DO NOTHING
   }
   
   // Object is a game object
   else
   {

   } 
}

void CRock6::Reset(void)
{
   
}







