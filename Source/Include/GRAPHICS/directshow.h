/////////////////////////////////////////////////////////
// File: "directshowvideo.h"
//
// Author: Jesse Edmunds (JE)
//
// Date Created: 11/10/2002
//
// Purpose: CDirectShow class declaration
/////////////////////////////////////////////////////////

#include <dshow.h>

#define WM_GRAPHNOTIFY WM_APP + 1 // graph notify constant for window messages

// directshow video wrapper class
class CDirectShow
{
	private:
		IGraphBuilder *m_pGraphBuilder; // the graph builder
		IMediaSeeking *m_pMediaSeeking; // media seeking
		IBasicAudio *m_pBasicAudio; // volume and panning control
		HWND m_hWnd; // window handle
		int m_iNumMedia; // the number of media we have loaded
		int m_iMaxMedia; // the max media we can load
		bool m_bInitialized; // whether or not we're initialized
		int m_iCurrentMedia; // the current media file playing
		
		class CMedia
		{
			public:
				IMediaEventEx *m_pMediaEvent; // the media event
				IMediaControl *m_pMediaControl; // the media control
				IVideoWindow  *m_pVideoWindow; // where video plays
				int m_iVolume; // the volume
				bool m_bPlaying; // whether or not it's playing
				bool m_bLoop; // whether or not to loop
				int m_iPan; // the panning
				bool m_bInitialized; // whether ot not it's initialized
				wchar_t m_wstrFilePath[_MAX_PATH]; // the file name
		};
		
		CMedia * m_aMedia; // the array of media to store
		
		////////////////////////////////////////////
		// Function: "ChangeCurrentMedia"
		//
		// Last Modified: 11/10/2002
		//
		// Output: modifies member variables
		//
		// Input: iIndex - the index of the new media to change to
		//
		// Purpose: to change the current media
		////////////////////////////////////////////
		HRESULT DeallocateMedia(int iIndex);	
			
	public:
		////////////////////////////////////////////
		// Function: "CDirectShow"
		//
		// Last Modified: 11/10/2002
		//
		// Output: modifies member variables
		//
		// Purpose: to set stuff to NULL
		////////////////////////////////////////////
		CDirectShow()
		{
			// zero 'em out
			m_pGraphBuilder = NULL;
			m_pMediaSeeking = NULL;
			m_pBasicAudio = NULL;
			m_hWnd = NULL;
			m_iNumMedia = 0;
			m_iMaxMedia = 0;
			m_bInitialized = false;
			m_iCurrentMedia = 0;
		}
		////////////////////////////////////////////
		// Function: "~CDirectShow"
		//
		// Last Modified: 11/10/2002
		//
		// Output: modifies member variables
		//
		// Purpose: to destroy stuff
		////////////////////////////////////////////
		~CDirectShow();
		
      bool IsPlaying(int iIndex);

		////////////////////////////////////////////
		// Function: "Init"
		//
		// Last Modified: 11/10/2002
		//
		// Output: modifies member variables
		//
		// Returns: HRESULT - error code
		//
		// Purpose: to set up
		////////////////////////////////////////////
		HRESULT Init(HWND hWnd, int iMaxMedia);
		
		////////////////////////////////////////////
		// Function: "Load"
		//
		// Last Modified: 11/10/2002
		//
		// Input: char *strVideoFile - the file name
		//		  int *piIndexOut - output index
		//
		// Output: modifies member variables
		//
		// Returns: HRESULT - error code
		//
		// Purpose: to load a video
		////////////////////////////////////////////
		HRESULT Load(char *strMediaFilePath, int *piIndexOut);
		
		////////////////////////////////////////////
		// Function: "Unload"
		//
		// Last Modified: 11/10/2002
		//
		// Input: int iIndex - the index we want to modify
		//
		// Output: modifies member variables
		//
		// Returns: HRESULT - error code
		//
		// Purpose: to unload a video
		////////////////////////////////////////////
		HRESULT Unload(int iIndex);
		
		////////////////////////////////////////////
		// Function: "Play"
		//
		// Last Modified: 11/10/2002
		//
		// Input: int iIndex - the index we want
		//
		// Returns: HRESULT - error code
		//
		// Purpose: to play a video
		////////////////////////////////////////////
		HRESULT Play(int iIndex, RECT *prcVideoWindow = NULL);
		
		////////////////////////////////////////////
		// Function: "Stop"
		//
		// Last Modified: 11/10/2002
		//
		// Input: int iIndex - the index we want
		//
		// Returns: HRESULT - error code
		//
		// Purpose: to stop a video
		////////////////////////////////////////////
		HRESULT Stop(int iIndex);
		
		////////////////////////////////////////////
		// Function: "HandleEvent"
		//
		// Last Modified: 11/10/2002
		//
		// Input: int iIndex - the index we want
		//
		// Returns: HRESULT - error code
		//
		// Purpose: to handle window events
		////////////////////////////////////////////
		HRESULT HandleEvent();
		
		////////////////////////////////////////////
		// Function: "DestroyObjects"
		//
		// Last Modified: 11/10/2002
		//
		// Output: modifies member variables
		//
		// Returns: HRESULT - error code
		//
		// Purpose: to free memory
		////////////////////////////////////////////
		HRESULT DestroyObjects();
		
		////////////////////////////////////////////
		// Function: "SetPan"
		//
		// Last Modified: 11/10/2002
		//
		// Input: int iPan - the pan setting
		//
		// Output: modifies member variables
		//
		// Returns: HRESULT - error code
		//
		// Purpose: to free memory
		////////////////////////////////////////////
		HRESULT SetPan(int iIndex, int iPan);

		////////////////////////////////////////////
		// Function: "SetVolume"
		//
		// Last Modified: 11/10/2002
		//
		// Input: int iVolume - the volume
		//
		// Output: modifies member variables
		//
		// Returns: HRESULT - error code
		//
		// Purpose: to free memory
		////////////////////////////////////////////
		HRESULT SetVolume(int iIndex, int iVolume);
};