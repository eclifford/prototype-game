/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File:			   “InputManger.CPP”
// Author:			Stephen Murrish
// Creation Date:	Nov 11, 2002
// Purpose:			Implementation of direct input manager
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "Input/InputManager.h"
#include "core/oglwindow.h"

#include "utils\mmgr.h"

CInputManager* CInputManager::m_pInstance = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

///////////////////////
// name:	constructor
// mod:		Nov 11, 2002
// in:		none
// out:		none
// return:	none
// purpose:	create instance of class and initialize parameters;
///////////////////////
CInputManager::CInputManager()
{
   m_TimeLimit = 0.0f;
}


///////////////////////
// name:	destructor
// mod:		Nov 11, 2002
// in:		none
// out:		none
// return:	none
// purpose:	free memory
///////////////////////
CInputManager::~CInputManager()
{
}


CInputManager* CInputManager::GetInstance()
{
	if (m_pInstance == NULL)
		m_pInstance = new CInputManager;

	return m_pInstance;
}


///////////////////////
// name:	Init
// mod:		Nov 11, 2002
// in:		none
// out:		none
// return:	none
// purpose:	initialize direct input8
///////////////////////
void		CInputManager::Init()
{

	m_hi           = COpenGLWindow::GetInstance()->GetHINSTANCE();
	m_hw           = COpenGLWindow::GetInstance()->GetHWND();
   m_ScreenHeight = COpenGLWindow::GetInstance()->GetScreenHeight();
   m_ScreenWidth  = COpenGLWindow::GetInstance()->GetScreenWidth();

	if (FAILED(DirectInput8Create(m_hi, DIRECTINPUT_VERSION, IID_IDirectInput8,
					(void **)&m_diObj, NULL)))
	{
		MessageBox(m_hw, "Failed to create DI object", "Err", MB_OK);
		return;
	}

	InitKeyboard();
	InitMouse();
   
}


///////////////////////
// name:	InitKeyboard
// mod:		Nov 11, 2002
// in:		none
// out:		none
// return:	none
// purpose:	initialize keyboard
///////////////////////
void		CInputManager::InitKeyboard()
{
	// create device
	m_diObj->CreateDevice(GUID_SysKeyboard, &m_diKeyboard, NULL);

	// set the data format for the keyboard
	m_diKeyboard->SetDataFormat(&c_dfDIKeyboard);

	// set the Cooperative level for the keyboard 
	m_diKeyboard->SetCooperativeLevel(m_hw, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// acquire the device
	m_diKeyboard->Acquire();
}


///////////////////////
// name:	InitMouse
// mod:		Nov 11, 2002
// in:		none
// out:		none
// return:	none
// purpose:	initialize mouse
///////////////////////
void		CInputManager::InitMouse()
{
	// create device
	m_diObj->CreateDevice(GUID_SysMouse, &m_diMouse, NULL);

	// setup the device
	m_diMouse->SetDataFormat(&c_dfDIMouse);

	// set coop level
	m_diMouse->SetCooperativeLevel(m_hw, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// acquire the device
	m_diMouse->Acquire();
}



void		CInputManager::Shutdown(void)
{
	// release Objects
	if (m_diMouse)
	{
		// release our chokehold on the keyboard
		m_diMouse->Unacquire();
		SAFERELEASE(m_diMouse)
	}

	if (m_diKeyboard)
	{
		// release our chokehold on the keyboard
		m_diKeyboard->Unacquire();
		SAFERELEASE(m_diKeyboard)
	}

	SAFERELEASE(m_diObj);
	
	delete m_pInstance;
}



void CInputManager::PollMouse()
{

	HRESULT hr;

	memset(&m_mState, 0, sizeof(m_mState));

	if(FAILED(hr = m_diMouse->GetDeviceState(sizeof(m_mState), (LPVOID)&m_mState)))
	{
		if (hr == DIERR_INPUTLOST)
			m_diMouse->Acquire();
	}
}



bool CInputManager::PollMouseClick()
{
   if (m_TimeLimit > 0)
   {
      if (g_time - m_TimerTime < m_TimeLimit)
      {  
         return false;
      }
   }

	if (m_mState.rgbButtons[0] & 0x080)
	{
      m_TimerTime = g_time;
   	return true;
	}
   return false;
}



bool CInputManager::PollMouseRClick()
{

	if (m_mState.rgbButtons[1] & 0x080)
		return true;

	return false;
}


bool CInputManager::PollMouseMClick()
{

   if (m_mState.rgbButtons[1] & 0x080)
      return true;

   return false;
}


void CInputManager::PollKeyboard()
{
	memset(&m_kState, 0, sizeof(m_kState));			// erase anything inside of it

	HRESULT hr;
	if (FAILED(hr = m_diKeyboard->GetDeviceState(sizeof(m_kState), (LPVOID)&m_kState)))
	{
		if (hr == DIERR_INPUTLOST)
			m_diKeyboard->Acquire();
	}


}

bool CInputManager::GetKey(int dikvalue)
{

   if (m_TimeLimit > 0)
   {
      if (g_time - m_TimerTime < m_TimeLimit)
      {  
         return false;
      }
   }

	if (m_kState[dikvalue] & 0x080)
   {
      m_TimerTime = g_time;
		return true;
   }
		
	return false;	
}


void CInputManager::PollMouseMove(float &x, float &y, float &z)
{

	x += m_mState.lX;
	y += m_mState.lY;
   z += m_mState.lZ;
}


void CInputManager::SetTimerInput(float timelimit)
{
   m_TimeLimit = timelimit;
   m_TimerTime = g_time;
}


void CInputManager::ResetTimer()
{
   m_TimerTime = g_time;
}

