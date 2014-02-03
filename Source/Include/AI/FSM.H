/// \file FSM.h
/// Finite State Machine header file
//////////////////////////////////////////////////////////////////////////

#pragma once

// #includes here
#include <string>
#include <list>
#include "AI\state.h"
using namespace std;

#define MAX_STATES 20

/// An header for a dynamically configurable finite state machine.
/// Call functions to add states, conditions, and what not to the machine.

class CFSM
{
   public:
      CFSM();
      ~CFSM();

      void        Initialize();
      void        SetCurrentState(int StateIndex);

      /// Add a new state to the state machine, returns index of state
      int         AddState(string StateName);

      /// Add a state machine to an already added state, this creates a hierarchy of FSMs
      CFSM*       AddStateMachine(string StateName);
      CFSM*       AddStateMachine(int StateIndex);
      void        AddTransitionCondition(int FromStateIndex, int ToStateIndex, bool (*pf)(void*));
      void        AddTransitionCondition(string FromState, string ToState, bool (*pf)(void*));
      void        AddOnEnter(int StateIndex, void (*pf)(void*));
      void        AddOnExit(int StateIndex, void (*pf)(void*));
      void        AddUpdate(int StateIndex, void (*pf)(void*));
      void        AddOnEnter(string State, void (*pf)(void*));
      void        AddOnExit(string State, void (*pf)(void*));
      void        AddUpdate(string State, void (*pf)(void*));
      int         GetCurrentStateIndex();
      const char* GetCurrentStateName();
      int         GetStateIndex(string Name);
      const char* GetStateName(int Index);
      int         GetNumStates();
      void        Update();
      void        SetObjectToModify(void* pObj);

   protected:
      void*    m_pObj;
      
   private:
      CState   m_State[MAX_STATES];
      int      m_NumStates;
      int      m_CurrentState;
      string   m_CurrentStateName;
};

