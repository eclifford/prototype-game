#include <math.h>

#include "collision\rdc.h"
#include "math3d\intersection.h"

#include "utils\mmgr.h"

CRDC::CRDC(void)
{
	IntrList = 0;
	ActiveList = 0;
	PairList = 0;
	NodePool = 0;
	PairPool = 0;
}

CRDC::~CRDC(void)
{
	RDC_Node *node;
	RDC_Pair *pair;

	ReleaseAllNodes();
	while ((node = NodePool))
	{
		NodePool = NodePool->IntrNext;
		delete node;
	}

	ReleaseAllPairs();
	while ((pair = PairPool))
	{
		PairPool = PairPool->next;
		delete pair;
	}

}

void CRDC::SetupPools(int n)
{
	RDC_Node *node;
	RDC_Pair *pair;

	for (int i = 0; i < n * 2; i++)
	{
		node = new RDC_Node;
		node->ActiveNext = 0;
		node->IntrNext = NodePool;
		NodePool = node;
	}

	for (int j = 0; j < n; j++)
	{
		pair = new RDC_Pair;
		pair->next = PairPool;
		PairPool = pair;
	}
}

RDC_Node *CRDC::GetFreeNode(void)
{
	RDC_Node *node;

	if (!NodePool)
		node = new RDC_Node;
	else
	{
		node = NodePool;
		NodePool = NodePool->IntrNext;
	}

	return node;
}

void CRDC::ReleaseNode(RDC_Node *node)
{
	node->IntrNext = NodePool;
	NodePool = node;
}

RDC_Pair *CRDC::GetFreePair(void)
{
	RDC_Pair *pair;

	if (!PairPool)
		pair = new RDC_Pair;
	else
	{
		pair = PairPool;
		PairPool = PairPool->next;
	}

	return pair;
}

void CRDC::ReleasePair(RDC_Pair *Pair)
{
	Pair->next = PairPool;
	PairPool = Pair;
}

void CRDC::IntrListInsert(CGameObject *obj)
{
	RDC_Node *curr, *prev;
	RDC_Node *start = GetFreeNode();
	RDC_Node *end = GetFreeNode();
	const Sphere *sphere = obj->GetSphere();

	// Set up nodes
	start->ActiveNext = 0;
	start->obj = obj;
	start->type = RDC_START;
	start->x = sphere->Center.x - sphere->Radius;

	// Set up nodes
	end->ActiveNext = 0;
	end->obj = obj;
	end->type = RDC_END;
	end->x = sphere->Center.x + sphere->Radius;

	// Insert start node
	if (!IntrList || IntrList->x >= start->x)
	{
		start->IntrNext = IntrList;
		IntrList = start;
	}
	else
	{
		prev = IntrList;
		curr = prev->IntrNext;

		while (curr && curr->x < start->x)
		{
			prev = curr;
			curr = curr->IntrNext;
		}

		start->IntrNext = prev->IntrNext;
		prev->IntrNext = start;
	}

	// Insert end node
	prev = start;
	curr = start->IntrNext;

	while (curr && curr->x <= end->x)
	{
		prev = curr;
		curr = curr->IntrNext;
	}

	end->IntrNext = prev->IntrNext;
	prev->IntrNext = end;
}

void CRDC::Init(int n)
{
	SetupPools(n);
}

bool CRDC::TestInterval(float n1, float r1, float n2, float r2)
{
	return fabs(n1 - n2) <= r1 + r2;
}

bool CRDC::SweepAndPrune(bool FastExit)
{
	RDC_Node *node;
	bool HaveCollision = false;

	// Construct pair list
	while ((node = IntrList))
	{
		IntrList = node->IntrNext;

		if (node->type == RDC_START)
		{
			// Make a pair if intervals intersect
			RDC_Node *active = ActiveList;
			RDC_Pair *pair;
			
			while (active)
			{
				if (!active->obj->IsStaticObj() || !node->obj->IsStaticObj())
				{
					const Sphere *s1 = active->obj->GetSphere();
					const Sphere *s2 = node->obj->GetSphere();

					// Prune Z then Y
					if (TestInterval(s1->Center.z, s1->Radius, s2->Center.z, s2->Radius) 
						&& TestInterval(s1->Center.y, s1->Radius, s2->Center.y, s2->Radius))
					{
						// Finally, check sphere to sphere
						if (SphereToSphere(s1, s2))
						{
							if (!FastExit)
							{
								pair = GetFreePair();
								pair->objA = active->obj;
								pair->objB = node->obj;

								pair->next = PairList;
								PairList = pair;
							}
							else
							{
								OBB *boxA, *boxB;

								boxA = active->obj->GetOBB();
								boxB = node->obj->GetOBB();

								if (OBBToOBB(boxA, boxB))
								{
									HaveCollision = true;
									break;
								}
							}
						}
					}
				}
				active = active->ActiveNext;
			}

			// Put node on active list
			node->ActiveNext = ActiveList;
			ActiveList = node;
		}
		else
		{
			// Take obj out of active list
			RDC_Node *curr, *prev;
			
			if (ActiveList->obj == node->obj)
			{
				curr = ActiveList;
				ActiveList = ActiveList->ActiveNext;
			}
			else
			{
				prev = ActiveList;
				curr = ActiveList->ActiveNext;

				while (curr->obj != node->obj)
				{
					prev = curr;
					curr = curr->ActiveNext;
				}

				prev->ActiveNext = curr->ActiveNext;
			}

			// Put both nodes back in the pool
			ReleaseNode(curr);
			ReleaseNode(node);
		}
		if (FastExit && HaveCollision)
			break;
	}

	return HaveCollision;
}

RDC_Pair *CRDC::CheckCollisions(void)
{
	SweepAndPrune(false);
	return PairList;
}

bool CRDC::CheckForAnyCollision(void)
{
	bool ret = SweepAndPrune(true);
	ReleaseAllNodes();
	ReleaseAllPairs();
	return ret;
}

void CRDC::AddObj(CGameObject *obj)
{
	IntrListInsert(obj);
}

void CRDC::ReleaseAllPairs(void)
{
	RDC_Pair *pair;
	while (PairList)
	{
		pair = PairList;
		PairList = PairList->next;
		ReleasePair(pair);
	}
}

void CRDC::ReleaseAllNodes(void)
{
	RDC_Node *node;
	while (IntrList)
	{
		node = IntrList;
		IntrList = IntrList->IntrNext;
		ReleaseNode(node);
	}
}
