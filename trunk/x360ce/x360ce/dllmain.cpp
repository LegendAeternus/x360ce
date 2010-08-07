/*  x360ce - XBOX360 Controler Emulator
 *  Copyright (C) 2002-2010 ToCA Edit
 *
 *  x360ce is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  x360ce is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with x360ce.
 *  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "stdafx.h"
#include "globals.h"
#include "Utils.h"
#include "Config.h"
#include "DirectInput.h"
#include "FakeAPI.h"

HINSTANCE hX360ceInstance = NULL;
HINSTANCE hNativeInstance = NULL;

HWND hWnd = NULL;
DWORD dwAppPID = NULL;

void LoadOriginalDll()
{

	TCHAR buffer[MAX_PATHW];

	// Getting path to system dir and to xinput1_3.dll
	GetSystemDirectory(buffer,MAX_PATHW);

	// Append dll name
	_tcscat_s(buffer,sizeof(buffer),_T("\\xinput1_3.dll"));

	// try to load the system's dinput.dll, if pointer empty
	if (!hNativeInstance) hNativeInstance = LoadLibrary(buffer);

	// Debug
	if (!hNativeInstance)
	{
		ExitProcess(0); // exit the hard way
	}

}

BOOL RegisterWindowClass(HINSTANCE hinstance) 
{ 
	WNDCLASSEX wcx; 

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.lpfnWndProc = DefWindowProc;     // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = hinstance;         // handle to instance 
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);              // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);                    // predefined arrow 
	wcx.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);    // white background brush 
	wcx.hIconSm = (HICON) LoadImage(hinstance, // small class icon 
		MAKEINTRESOURCE(5),
		IMAGE_ICON, 
		GetSystemMetrics(SM_CXSMICON), 
		GetSystemMetrics(SM_CYSMICON), 
		LR_DEFAULTCOLOR); 
	wcx.lpszMenuName =  _T("x360ceMenu");    // name of menu resource
	wcx.lpszClassName = _T("x360ceWClass");  // name of window class 

	// Register the window class. 

	return RegisterClassEx(&wcx); 
} 

BOOL Createx360ceWindow(HINSTANCE hInst)
{
	if(RegisterWindowClass(hInst))
	{
		hWnd = CreateWindow( 
			_T("x360ceWClass"),  // name of window class
			_T("x360ce"),        // title-bar string 
			WS_OVERLAPPEDWINDOW, // top-level window 
			CW_USEDEFAULT,       // default horizontal position 
			CW_USEDEFAULT,       // default vertical position 
			CW_USEDEFAULT,       // default width 
			CW_USEDEFAULT,       // default height 
			(HWND) NULL,         // no owner window 
			(HMENU) NULL,        // use class menu 
			hX360ceInstance,     // handle to application instance 
			(LPVOID) NULL);      // no window-creation data 
		return TRUE;
	}
	else
	{
		WriteLog(_T("[CORE]    RegisterWindowClass Failed"));
		return FALSE;
	}
}

VOID InitInstance(HMODULE hModule) 
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	for (int i=0;i<XUSER_MAX_COUNT;i++){
		ZeroMemory(&Gamepad[i],sizeof(DINPUT_GAMEPAD));
		Gamepad[i].id1 = X360CE_ID1;
		Gamepad[i].id2 = X360CE_ID2;
		Gamepad[i].forcepercent = 100;
		Gamepad[i].gamepadtype = 1;
	}

	hX360ceInstance = (HINSTANCE) hModule;
	dwAppPID=GetCurrentProcessId();
	InitConfig(_T("x360ce.ini"));

#if SVN_MODS != 0 
	WriteLog(_T("[CORE]    x360ce %d.%d.%d.%d (modded) started by process %s PID %d"),VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH,SVN_REV,PIDName(dwAppPID),dwAppPID);
#else 
	WriteLog(_T("[CORE]    x360ce %d.%d.%d.%d started by process %s PID %d"),VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH,SVN_REV,PIDName(dwAppPID),dwAppPID);
#endif

	if(!Createx360ceWindow(hX360ceInstance))
	{
		WriteLog(_T("[CORE]    x360ce window not created, ForceFeedback will be disabled !"));
	}

	if(wFakeAPI)
	{
		if(wFakeWMI) FakeWMI(true);
		if(wFakeDI) FakeDI(true);
		if(wFakeWinTrust) FakeWinTrust(true);
	}
}

VOID ExitInstance() 
{   
	if(wFakeAPI)
	{
		if(wFakeWMI) FakeWMI(false);
		if(wFakeDI) FakeDI(false);
		if(wFakeWinTrust) FakeWinTrust(false);
	}

	ReleaseDirectInput();

	delete[] logfilename;

	if (hNativeInstance)
	{
		FreeLibrary(hNativeInstance);
		hNativeInstance = NULL;  
		CloseHandle(hNativeInstance);
	}

	if(IsWindow(hWnd)) DestroyWindow(hWnd);

	UnregisterClass(_T("x360ceWClass"),hX360ceInstance);
}

extern "C" BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID reserved)
{
	UNREFERENCED_PARAMETER(reserved);

	if (dwReason == DLL_PROCESS_ATTACH ) 
	{
		DisableThreadLibraryCalls(hModule);
		InitInstance(hModule);
	}

	else if (dwReason == DLL_PROCESS_DETACH) 
	{
		WriteLog(_T("[CORE]    x360ce terminating, bye"));
		ExitInstance();
	}
	return TRUE;
}