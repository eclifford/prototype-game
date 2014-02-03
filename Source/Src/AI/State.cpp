#include "AI\FSM.h"
#include "AI\StateTransition.h"

#include "utils\mmgr.h"

CState::CState()
{
   Initialize();
}


CState::~CState()
{
   while(!m_Transitions.empty())
   {
      delete m_Transitions.back();
      m_Transitions.pop_back();
   }

   if (m_pSubStateMachine)
      delete m_pSubStateMachine;
}


void CState::Initialize()
{
      m_StateName             = "";
      m_StateIndex            = -1;
      m_pSubStateMachine      = 0;
      m_Entered               = false;
      m_UpdateFunc            = 0;
      m_OnEnterFunc           = 0;
      m_OnExitFunc            = 0;
      m_Transitions.clear();
}


void  CState::SetStateName(string StateName)
{
   m_StateName = StateName;
}


void  CState::SetStateIndex(int StateIndex)
{
   m_StateIndex = StateIndex;
}


void  CState::SetStateMachine(CFSM* Machine)
{
   m_pSubStateMachine = Machine;
}


void  CState::SetUpdateFunc(void (*pf)(void*))
{
   m_UpdateFunc = pf;
}


void  CState::SetEnterFunc(void (*pf)(void*))
{
   m_OnEnterFunc = pf;
}


void  CState::SetExitFunc(void (*pf)(void*))
{
   m_OnExitFunc = pf;
}


void  CState::AddTransition(int ToIndex, bool (*pf)(void*))
{
   CStateTransition* Temp = new CStateTransition;
   Temp->SetToState(ToIndex);
   Temp->SetTransitionFunc(pf);
   m_Transitions.push_back(Temp);
}



void  CState::Update(void* pObj)
{
   if (m_UpdateFunc)
   {
      if  (m_Entered)
         (*m_UpdateFunc)(pObj);
      else
      {
         OnEnter(pObj);
         (*m_UpdateFunc)(pObj);
      }
   }
}


void  CState::OnEnter(void* pObj)
{
   if (m_OnEnterFunc)
      (*m_OnEnterFunc)(pObj);

   m_Entered = true;
}


void  CState::OnExit(void* pObj)
{
   if (m_OnExitFunc)
      (*m_OnExitFunc)(pObj);
   m_Entered = false;
}

int CState::GetIndex()
{
   return m_StateIndex;
}

const char* CState::GetName()
{
   return m_StateName.c_str();
}

CFSM* CState::GetFSM()
{
   return m_pSubStateMachine;
}

int CState::GetTransition(void* pObj)
{
   int Trans = -1;
   m_TransIterator = m_Transitions.begin();

   while (m_TransIterator != m_Transitions.end())
   {
      Trans = (*m_TransIterator)->Transition(pObj);
      if ( Trans != -1)
         break;
      m_TransIterator++;
   }

   if (Trans != -1)
      OnExit(pObj);

   return Trans;
}

