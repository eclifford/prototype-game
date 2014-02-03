#include "AI\StateTransition.h"

#include "utils\mmgr.h"

CStateTransition::CStateTransition()
{
}

CStateTransition::~CStateTransition()
{
}

void CStateTransition::SetToState(int Index)
{
   m_ToStateIndex = Index;
}

void CStateTransition::SetTransitionFunc(bool (*pf)(void*))
{
   m_pTransFunc = pf;
}


int CStateTransition::Transition(void* pObj)
{
   int TransTo = -1;

   if ((*m_pTransFunc)(pObj))
      TransTo = m_ToStateIndex;

   return TransTo;
}
