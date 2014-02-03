#pragma once

#include "gamelogic\gameobject.h"
#include "collision\rdc.h"

class CCollisionSystem
{
private:
	CRDC    m_Pruner;

	void CheckTerrainCollision(CGameObject *obj);
public:
	CCollisionSystem(void);
	~CCollisionSystem(void) {}

	void CheckCollisions(CGameObject *list);
	bool CheckForAnyCollision(CGameObject *list);
};

