#pragma once

#include "gamelogic\gameobject.h"
#include "physics\physics.h"

enum {RDC_START, RDC_END};


// A list of these is returned for OBB test
struct RDC_Pair
{
	CGameObject *objA;
	CGameObject *objB;
	RDC_Pair *next;
};

// Used in both interval and active lists
struct RDC_Node
{
	CGameObject *obj;
	RDC_Node *IntrNext;
	RDC_Node *ActiveNext;

	float x;
	int type;
};

class CRDC
{
private:
	RDC_Node *IntrList;
	RDC_Node *ActiveList;
	RDC_Pair *PairList;

	RDC_Node *NodePool;
	RDC_Pair *PairPool;

	void SetupPools(int n);
	RDC_Node *GetFreeNode(void);
	void ReleaseNode(RDC_Node *node);
	RDC_Pair *GetFreePair(void);
	void ReleasePair(RDC_Pair *pair);

	void IntrListInsert(CGameObject *obj);
	bool TestInterval(float n1, float r1, float n2, float r2);
	bool SweepAndPrune(bool FastExit);
public:
	CRDC(void);
	~CRDC(void);

	void Init(int n);
	RDC_Pair *CheckCollisions(void);
	bool CheckForAnyCollision(void);
	void AddObj(CGameObject *obj);
	void ReleaseAllPairs(void);
	void ReleaseAllNodes(void);
};