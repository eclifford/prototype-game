/// \file rock.h
/// A stationary rock
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "gamelogic/stationaryobjects.h"

class CRock1: public CStationaryObject
{
	private:
	public:
      CRock1(CRock1& rock) : CStationaryObject(rock)
      {
         m_Type = OBJ_ROCK1;
	     m_StaticObj = true;
      }

      CRock1() {};
	  ~CRock1() {};

      void Setup(void);
      void Update(void);
	  void Collide(const CGameObject *obj);
	  void Reset(void);
	  void Die(void){};
      void TakeDamage(int damage) {};

};	

class CRock2: public CStationaryObject
{
	private:
	public:
      CRock2(CRock2& rock) : CStationaryObject(rock)
      {
         m_Type = OBJ_ROCK2;
	     m_StaticObj = true;
      }

      CRock2() {};
	  ~CRock2() {};

      void Setup(void);
      void Update(void);
	  void Collide(const CGameObject *obj);
	  void Reset(void);
	  void Die(void) {};
      void TakeDamage(int damage) {};

};	

class CRock3: public CStationaryObject
{
	private:
	public:
      CRock3(CRock3& rock) : CStationaryObject(rock)
      {
         m_Type = OBJ_ROCK3;
	     m_StaticObj = true;
      }

      CRock3() {};
	  ~CRock3() {};

      void Setup(void);
      void Update(void);
	  void Collide(const CGameObject *obj);
	  void Reset(void);
	  void Die(void) {};
      void TakeDamage(int damage) {};

};	

class CRock4: public CStationaryObject
{
	private:
	public:
      CRock4(CRock4& rock) : CStationaryObject(rock)
      {
         m_Type = OBJ_ROCK4;
	     m_StaticObj = true;
      }

      CRock4() {};
	  ~CRock4() {};

      void Setup(void);
      void Update(void);
	  void Collide(const CGameObject *obj);
	  void Reset(void);
	  void Die(void){};
      void TakeDamage(int damage) {};

};	

class CRock5: public CStationaryObject
{
	private:
	public:
      CRock5(CRock5& rock) : CStationaryObject(rock)
      {
         m_Type = OBJ_ROCK5;
	     m_StaticObj = true;
      }

      CRock5() {};
	  ~CRock5() {};

      void Setup(void);
      void Update(void);
	  void Collide(const CGameObject *obj);
	  void Reset(void);
	  void Die(void) {};
      void TakeDamage(int damage) {};

};	

class CRock6: public CStationaryObject
{
	private:
	public:
      CRock6(CRock6& rock) : CStationaryObject(rock)
      {
         m_Type = OBJ_ROCK6;
	     m_StaticObj = true;
      }

      CRock6() {};
	  ~CRock6() {};

      void Setup(void);
      void Update(void);
	  void Collide(const CGameObject *obj);
	  void Reset(void);
	  void Die(void) {};
      void TakeDamage(int damage) {};

};								  