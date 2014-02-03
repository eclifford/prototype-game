/////////////////////////////////////////////////
// File: "directshowvideo.cpp"
//
// Author: Jesse Edmunds (JE)
//
// Date Created: 11/11/2002
//
// Purpose: CDirectShow class implementation
/////////////////////////////////////////////////

#define VC_EXTRALEAN
#include <windows.h>
#include <dxerr8.h>

#pragma comment(lib, "dxerr8.lib")
#pragma comment(lib, "strmiids.lib")



#include "graphics\directshow.h"
#include "graphics\ddutils.h"

#include "utils\mmgr.h"

// see header
HRESULT CDirectShow::Init(HWND hWnd, int iMaxMedia)
{
	// initialize com
	HRESULT hr;
	if (FAILED(hr = CoInitialize(NULL)))
		return DXTRACE_ERR(TEXT("CoInitialize"), hr);
	
	m_hWnd = hWnd; // store hwnd
	
	m_aMedia = new CMedia[iMaxMedia]; // create the array
	
	// set them all to null for safety
	for (int i = 0; i < iMaxMedia; i++)
	{
		ZeroMemory(&m_aMedia[i], sizeof(m_aMedia[i]));
	}
	
	m_iMaxMedia = iMaxMedia;
	
	m_bInitialized = true;
					 
	return hr;
}

// see header
HRESULT CDirectShow::Load(char *strMediaFilePath, int *piIndexOut)
{
	HRESULT hr = S_OK;
	
	// find a free space in the array of media files
	for (int i = 0; i < m_iMaxMedia; i++)
	{
		if (m_aMedia[i].m_bInitialized == false)
		{
			// convert the file path to a wide char and store it in the array
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, strMediaFilePath, -1,
								m_aMedia[i].m_wstrFilePath, _MAX_PATH);
			m_aMedia[i].m_bInitialized = true; // its initialized
			break;
		}
	}
		
	*piIndexOut = m_iNumMedia; // give them the index
	m_iNumMedia++; // increment media counter	
	return hr;
}

// see header
HRESULT CDirectShow::Play(int iIndex, RECT *prcVideoArea)
{
	HRESULT hr;

	// check for invalid index
	if (iIndex < 0 || iIndex >= m_iNumMedia)
		return E_FAIL;
	
	// make sures its intialized
	if (m_aMedia[iIndex].m_bInitialized == false)
		return E_FAIL;
	
	// dont try to play it if its already playing
	if (m_aMedia[iIndex].m_bPlaying == true)
		return E_FAIL;
	
	DeallocateMedia(m_iCurrentMedia); // deallocate the current media
	
	// create the instance and check for errors
	if (FAILED(hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, 
									 (void **)&m_pGraphBuilder)))
		return DXTRACE_ERR(TEXT("CoCreateInstance"), hr);
		
	// load the file
	if (FAILED(hr = m_pGraphBuilder->RenderFile(m_aMedia[iIndex].m_wstrFilePath, NULL)))
		return DXTRACE_ERR(TEXT("RenderFile"), hr);
	
	if (prcVideoArea)
	{		
		// get the video window interface
		if (FAILED(hr = m_pGraphBuilder->QueryInterface(IID_IVideoWindow, 
						(void **)&m_aMedia[iIndex].m_pVideoWindow)))
			return DXTRACE_ERR(TEXT("QueryInterface"), hr);
		
		// set the video window's owner to our hwd
		if (FAILED(hr = m_aMedia[iIndex].m_pVideoWindow->put_Owner((OAHWND)m_hWnd)))
			return DXTRACE_ERR(TEXT("put_Owner"), hr);

		m_aMedia[iIndex].m_pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);

			// set the video window's size
			if (FAILED(hr = m_aMedia[iIndex].m_pVideoWindow->SetWindowPosition(prcVideoArea->left,
																			prcVideoArea->top,
																			prcVideoArea->right,
																			prcVideoArea->bottom)))
				return DXTRACE_ERR(TEXT("SetWindowPosition"), hr);
	}
	
	// get the media events interface	
	if (FAILED(hr = m_pGraphBuilder->QueryInterface(IID_IMediaEvent,
																	(void **)
																	&m_aMedia[iIndex].m_pMediaEvent)))
		return DXTRACE_ERR(TEXT("QueryInterface"), hr);
		
	// set the owner window to receive event notifications
	if (FAILED(hr = m_aMedia[iIndex].m_pMediaEvent->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPHNOTIFY,
																   0)))
		return DXTRACE_ERR(TEXT("SetNotifyWindow"), hr);
	
	// get the media control interface	
	if (FAILED(hr = m_pGraphBuilder->QueryInterface(IID_IMediaControl, 
					(void **)&m_aMedia[iIndex].m_pMediaControl)))
		return DXTRACE_ERR(TEXT("QueryInterface"), hr);
		
	// get the media seeking interface	
	if (FAILED(hr = m_pGraphBuilder->QueryInterface(IID_IMediaSeeking, 
					(void **)&m_pMediaSeeking)))
		return DXTRACE_ERR(TEXT("QueryInterface"), hr);
		
	// get the basic audio interface
	if (FAILED(hr = m_pGraphBuilder->QueryInterface(IID_IBasicAudio, 
					(void **)&m_pBasicAudio)))
		return DXTRACE_ERR(TEXT("QueryInterface"), hr);
	
	
	// run the media control
	if (FAILED(hr = m_aMedia[iIndex].m_pMediaControl->Run()))
		return DXTRACE_ERR(TEXT("Run"), hr);
		
	m_iCurrentMedia = iIndex; // update the current one
	
	m_aMedia[iIndex].m_bPlaying = true;
	
	// set volume and pan to defaults
	SetVolume(iIndex, 50);
	SetPan(iIndex, 0);
		
	return hr;	
}

// see header
HRESULT CDirectShow::Stop(int iIndex)
{
	HRESULT hr;

	// check for invalid index
	if (iIndex < 0 || iIndex >= m_iNumMedia)
		return E_FAIL;
	
	// run the media control
	if (FAILED(hr = m_aMedia[iIndex].m_pMediaControl->Stop()))
		return DXTRACE_ERR(TEXT("Stop"), hr);
		
	m_aMedia[iIndex].m_bPlaying = false;
		
	return hr;	
}

// see header
HRESULT CDirectShow::HandleEvent()
{
	HRESULT hr = S_OK;
	
	if (m_bInitialized == false)
		return E_FAIL;
	
	long lEventCode, lParamOne, lParamTwo; // temporary parameters for calling getevent
	
	// check for valid media event
	if (m_aMedia[m_iCurrentMedia].m_pMediaEvent == NULL)
		return E_FAIL;
	
	// call getevent until it returns a failure code
	while (SUCCEEDED(m_aMedia[m_iCurrentMedia].m_pMediaEvent->GetEvent(&lEventCode, &lParamOne, &lParamTwo, 0)))
	{
		// free the event params
		if (FAILED(hr = m_aMedia[m_iCurrentMedia].m_pMediaEvent->FreeEventParams(lEventCode, lParamOne, lParamTwo)))
			return DXTRACE_ERR(TEXT("FreeEventParams"), hr);
			
		// if the sample stops playing is is stopped by the user then unload it
		if (lEventCode == EC_COMPLETE || lEventCode == EC_USERABORT)
		{
			m_aMedia[m_iCurrentMedia].m_pMediaControl->Stop();
			
			LONGLONG llPos = 0;
			if (FAILED(hr = m_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
														  &llPos, AM_SEEKING_NoPositioning)))
				DXTRACE_ERR(TEXT("SetPositions"), hr);
				
			m_aMedia[m_iCurrentMedia].m_bPlaying = false;
			//Unload(m_iCurrentMedia);
			break;
		}
	}
	
	return hr;
}

// see header
HRESULT CDirectShow::Unload(int iIndex)
{
	// if its our current one then deallocate it
	if (iIndex == m_iCurrentMedia)
		DeallocateMedia(iIndex);
		
	// free media controls
	if (m_aMedia[iIndex].m_pMediaControl)
	{
		m_aMedia[iIndex].m_pMediaControl->Stop();
		m_aMedia[iIndex].m_pMediaControl->Release();
		m_aMedia[iIndex].m_pMediaControl = NULL;
	}

	// free video window
	if (m_aMedia[iIndex].m_pVideoWindow)
	{
		m_aMedia[iIndex].m_pVideoWindow->put_Visible(OAFALSE);
		m_aMedia[iIndex].m_pVideoWindow->put_Owner(NULL);
		m_aMedia[iIndex].m_pVideoWindow->Release();
		m_aMedia[iIndex].m_pVideoWindow = NULL;
	}
	
	SAFE_RELEASE(m_aMedia[iIndex].m_pMediaEvent); // free the media events
	
	// zero out the memory of the rest
	ZeroMemory(&m_aMedia[iIndex], sizeof(m_aMedia[iIndex]));
	
	m_iNumMedia--;
	
	return S_OK;
}

// see header
HRESULT CDirectShow::DestroyObjects()
{
	if (m_bInitialized == false)
		return E_FAIL;

	for (int i = 0; i < m_iMaxMedia; i++)
	{
		Unload(i); // unload 'em all
	}
	
	SAFE_RELEASE(m_pBasicAudio);
	SAFE_RELEASE(m_pMediaSeeking);
	SAFE_RELEASE(m_pGraphBuilder);
	
	CoUninitialize();
	
	delete [] m_aMedia;
	m_aMedia = 0;
	
	m_bInitialized = false;
	
	return S_OK;
}

// see header
CDirectShow::~CDirectShow()
{
	DestroyObjects(); // kill 'em
}

// see header
HRESULT CDirectShow::DeallocateMedia(int iIndex)
{
	HRESULT hr = S_OK;
	
	if (m_bInitialized == false)
		return E_FAIL;
	
	if (m_pBasicAudio)
	{
		LONGLONG llPos = 0;
		if (FAILED(hr = m_pMediaSeeking->SetPositions(&llPos, AM_SEEKING_AbsolutePositioning,
															&llPos, AM_SEEKING_NoPositioning)))
					DXTRACE_ERR(TEXT("SetPositions"), hr);
	}
	
	// safe release basic audio and media seeking
	SAFE_RELEASE(m_pBasicAudio);
	SAFE_RELEASE(m_pMediaSeeking);
	
	// free the media control
	if (m_aMedia[iIndex].m_pMediaControl)
	{
		if (FAILED(hr = m_aMedia[iIndex].m_pMediaControl->Stop()))
			DXTRACE_ERR("Stop", hr);
    	
		// release it	
		m_aMedia[iIndex].m_pMediaControl->Release();
		m_aMedia[iIndex].m_pMediaControl = NULL;
	}
    
	// safe release the media event and the graph builder
	SAFE_RELEASE(m_aMedia[iIndex].m_pMediaEvent);
	SAFE_RELEASE(m_pGraphBuilder);
	
	return hr;
}

HRESULT CDirectShow::SetVolume(int iIndex, int iVolume)
{
	// check for proper bounds
	if (iVolume > 100 || iVolume < 0)
		return E_FAIL;
		
	// convert to decibal volume
	iVolume = -15 * (100 - iVolume);
	
	HRESULT hr = S_OK;
	
	if (FAILED(hr = m_pBasicAudio->put_Volume((long)iVolume)))
		DXTRACE_ERR(TEXT("put_Volume"), hr);
	
	return hr;
}

HRESULT CDirectShow::SetPan(int iIndex, int iPan)
{
	// check for bad pan
	if (iPan > 100 || iPan < -100)
		return E_FAIL;
		
	// convert to decibals
	iPan *= 100;
	
	HRESULT hr = S_OK;
	

	if (FAILED(hr = m_pBasicAudio->put_Balance((long)iPan)))
		DXTRACE_ERR(TEXT("put_Balance"), hr);
	
	return hr;
}

bool CDirectShow::IsPlaying(int iIndex)
{
   return m_aMedia[iIndex].m_bPlaying;
}
