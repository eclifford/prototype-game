/// \file FSM.CPP
/// Finite State Machine implementation
//////////////////////////////////////////////////////////////////////////

#include "AI\FSM.H"

#include "utils\mmgr.h"

//////////////////////////////////////////////////////////////////////////
/// Constructor for the finite state machine class
/// 
/// Input:     none
///            none
///
/// Returns:   nothing
//////////////////////////////////////////////////////////////////////////
CFSM::CFSM()
{
   Initialize();
}

//////////////////////////////////////////////////////////////////////////
/// Destructor for the finite state machine class
/// 
/// Input:     none
///            none
///
/// Returns:   nothing
//////////////////////////////////////////////////////////////////////////
CFSM::~CFSM()
{

}

void CFSM::Initialize()
{
      m_NumStates = 0;
      m_CurrentState = -1;
      m_CurrentStateName = "";
}


void CFSM::SetCurrentState(int StateIndex)
{
   m_CurrentState = StateIndex;
}


int      CFSM::AddState(string StateName)
{
   m_CurrentState = m_NumStates;
   
   m_State[m_CurrentState].SetStateIndex(m_CurrentState);
   m_State[m_CurrentState].SetStateName(StateName);

   m_NumStates ++;

   return m_CurrentState;
}


CFSM*    CFSM::AddStateMachine(string StateName)
{
   // get index from name
   int Index = GetStateIndex(StateName);

   // create a new State machine 
   CFSM* pNewFSM = new CFSM;

   // put new state machine into the state specified by the parameter
   m_State[Index].SetStateMachine(pNewFSM);

   return m_State[Index].GetFSM();
}


CFSM*    CFSM::AddStateMachine(int StateIndex)
{
    // create a new State machine 
   CFSM* pNewFSM = new CFSM;

   // put new state machine into the state specified by the parameter
   m_State[StateIndex].SetStateMachine(pNewFSM);

   return m_State[StateIndex].GetFSM();
}


void     CFSM::AddTransitionCondition(int FromStateIndex, int ToStateIndex, bool (*pf)(void*))
{
   m_State[FromStateIndex].AddTransition(ToStateIndex, pf);
}


void     CFSM::AddTransitionCondition(string FromState, string ToState, bool (*pf)(void*))
{
   int FromIndex, ToIndex;

   FromIndex   = GetStateIndex(FromState);
   ToIndex     = GetStateIndex(ToState);

   m_State[FromIndex].AddTransition(ToIndex, pf);
}


void     CFSM::AddOnEnter(int StateIndex, void (*pf)(void*))
{
   m_State[StateIndex].SetEnterFunc(pf);
}

void     CFSM::AddOnExit(int StateIndex, void (*pf)(void*))
{
   m_State[StateIndex].SetExitFunc(pf);
}


void     CFSM::AddUpdate(int StateIndex, void (*pf)(void*))
{
   m_State[StateIndex].SetUpdateFunc(pf);
}


void     CFSM::AddOnEnter(string State, void (*pf)(void*))
{
   int Index = GetStateIndex(State);
   m_State[Index].SetEnterFunc(pf);
}

void     CFSM::AddOnExit(string State, void (*pf)(void*))
{
   int Index = GetStateIndex(State);
   m_State[Index].SetExitFunc(pf);
}



void     CFSM::AddUpdate(string State, void (*pf)(void*))
{
   int Index = GetStateIndex(State);
   m_State[Index].SetUpdateFunc(pf);
}


int      CFSM::GetCurrentStateIndex()
{
   return m_CurrentState;
}


const char*   CFSM::GetCurrentStateName()
{
   return m_State[m_CurrentState].GetName();
}


int      CFSM::GetNumStates()
{
   return m_NumStates;
}

void     CFSM::Update()
{
   int Trans;

   m_State[m_CurrentState].Update(m_pObj);

   // test for a transition
   Trans = m_State[m_CurrentState].GetTransition(m_pObj);

   if (Trans != -1)
   {
      m_CurrentState = Trans;
      return;
   }

   // if the state's got a sub-state machine then go through it
   if (m_State[m_CurrentState].GetFSM() != 0)
      m_State[m_CurrentState].GetFSM()->Update();
}


int          CFSM::GetStateIndex(string Name)
{
   int Counter;
   int Index = -1;

   for (Counter = 0; Counter < m_NumStates; Counter++)
   {
      if (!strcmp(Name.c_str(), m_State[Counter].GetName()))
      {
         Index = Counter;
         break;
      }
   }

   return Index;
}


const char*  CFSM::GetStateName(int Index)
{
   return m_State[Index].GetName();
}


void CFSM::SetObjectToModify(void* pObj)
{
   m_pObj = pObj;
}
