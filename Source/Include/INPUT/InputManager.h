/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File:			   “InputManger.h”
// Author:			Stephen Murrish
// Creation Date:	Nov 11, 2002
// Purpose:			header of direct input manager
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INPUTMANAGER_H__454CEA21_BFBA_4E5E_9995_FF23E7D66AB3__INCLUDED_)
#define AFX_INPUTMANAGER_H__454CEA21_BFBA_4E5E_9995_FF23E7D66AB3__INCLUDED_

#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>						   //	Needed for Windows Applications.
#include <dinput.h>							// direct input
#include <stdio.h>
#include "graphics\texturefonts.h"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#ifndef SAFERELEASE
#define SAFERELEASE(p) if (p) { p->Release(); p = NULL; }
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern float g_time;

class CInputManager  
{
	private:
		static CInputManager*   m_pInstance;
		HWND					      m_hw;
		HINSTANCE				   m_hi;
		LPDIRECTINPUT8			   m_diObj;			   // main di objecto
		LPDIRECTINPUTDEVICE8	   m_diKeyboard;		// the main keyboard device
		LPDIRECTINPUTDEVICE8	   m_diMouse;			// the main mouse device

		char					      m_kState[256];		// the keyboard array
		DIMOUSESTATE			   m_mState;			// mouse data structure
      int                     m_ScreenWidth;
      int                     m_ScreenHeight;
		CInputManager();

      float                   m_TimeLimit;
      float                   m_TimerTime;
      CTextureFonts           m_TF;

	public:
		virtual				~CInputManager();
		static	         CInputManager*	GetInstance();
		void					Init();
		void					InitKeyboard();
		void					InitMouse();
		void					PollMouse();
		bool					PollMouseClick();
		bool					PollMouseRClick();
      bool              PollMouseMClick();
		void					PollMouseMove(float &x, float &y, float &z);
		void					PollKeyboard();
		bool              GetKey(int dikvalue);
      void					Shutdown();
      void              SetTimerInput(float timelimit);
      void              ResetTimer();
};

#endif // !defined(AFX_INPUTMANAGER_H__454CEA21_BFBA_4E5E_9995_FF23E7D66AB3__INCLUDED_)
