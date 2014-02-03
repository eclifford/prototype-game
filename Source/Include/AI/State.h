/// \file State.h
/// Agent state header file
//////////////////////////////////////////////////////////////////////////

#pragma once

// #includes here
#include <string>
#include <list>
#include <ITERATOR>
#include "AI\StateTransition.h"
using namespace std;

/// An header for a state for the FSM
/// All the state stuff goes in here.

class CFSM;

class CState
{
   public:
      CState();
      ~CState();
      void        Initialize();
      void        SetStateName(string StateName);
      void        SetStateIndex(int StateIndex);
      void        SetStateMachine(CFSM* Machine);
      void        SetUpdateFunc(void (*pf)(void*));
      void        SetEnterFunc(void (*pf)(void*));
      void        SetExitFunc(void (*pf)(void*));
      CFSM*       GetFSM();
      const char* GetName();
      int         GetIndex();
      int         GetTransition(void* pObj);
      void        AddTransition(int ToIndex, bool (*pf)(void*));
      void        Update(void* pObj);
      void        OnEnter(void* pObj);
      void        OnExit(void* pObj);

   protected:
      
   private:
      string                     m_StateName;
      int                        m_StateIndex;      
      CFSM*                      m_pSubStateMachine;
      list<CStateTransition*>    m_Transitions;
      list<CStateTransition*>
               ::iterator        m_TransIterator;
      bool                       m_Entered;
      void                       (*m_UpdateFunc)(void*);
      void                       (*m_OnEnterFunc)(void*);
      void                       (*m_OnExitFunc)(void*);
};
