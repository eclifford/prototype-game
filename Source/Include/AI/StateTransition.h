/// \file StateTransition.h
/// Agent state transition header file
//////////////////////////////////////////////////////////////////////////

#pragma once

// #includes here
#include <string>
#include <list>
#include "AI\StateTransition.h"
using namespace std;

/// A header for a state's transitions
/// All the state transition stuff goes in here.

class CState;

class CStateTransition
{
   public:
      CStateTransition();
      ~CStateTransition();
      void        SetToState(int Index);
      void        SetTransitionFunc(bool (*pf)(void*));
      int         Transition(void* pObj);
      int         GetToStateIndex();
      const char* GetToStateName();

   protected:
      
      
   private:
      int      m_ToStateIndex;
      bool     (*m_pTransFunc)(void*);
};

