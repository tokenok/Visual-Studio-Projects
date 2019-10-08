#ifndef TokenDLL
#define TokenDLL

#include <Windows.h>
#include <iostream>
#include <thread>

#include "..//Tokenhack_public\window.h"
#include "..//Tokenhack_public\common.h"

#define DLL_NAME TEXT("Tokenhackdll.dll")

UINT WM_DLL_HOOKPROC = RegisterWindowMessage(TEXT("dllhooked"));

//#define DLL_API __declspec(dllexport)

extern "C"{
	__declspec(dllexport) LRESULT CALLBACK dllcallwndProc(int code, WPARAM wParam, LPARAM lParam);
	__declspec(dllexport) LRESULT CALLBACK dllcallwndretProc(int code, WPARAM wParam, LPARAM lParam);
	__declspec(dllexport) LRESULT CALLBACK dllgetmessageProc(int code, WPARAM wParam, LPARAM lParam);
}

BOOL InjectDll(wchar_t *dllName, HWND wnd) {
	HMODULE dll_handle = LoadLibrary(dllName);
	if (dll_handle == NULL) {
		MessageBox(NULL, str_to_wstr("LoadLibrary() failed or Tokenhackdll.dll could not be found\n" + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HOOKPROC dll_callwndProc = (HOOKPROC)GetProcAddress(dll_handle, "_dllcallwndProc@12");
	if (dll_callwndProc == NULL)
		MessageBox(NULL, str_to_wstr("GetProcAddress() failed (_dllcallwndProc@12)\n " + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);
	else {
		if (!SetWindowsHookEx(WH_CALLWNDPROC, dll_callwndProc, dll_handle, GetWindowThreadProcessId(wnd, NULL)))
			MessageBox(NULL, str_to_wstr("SetWindowsHookEx() failed (WH_CALLWNDPROC)\n " + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HOOKPROC dll_callwndretProc = (HOOKPROC)GetProcAddress(dll_handle, "_dllcallwndretProc@12");
	if (dll_callwndretProc == NULL)
		MessageBox(NULL, str_to_wstr("GetProcAddress() failed (_dllcallwndretProc@12) \n" + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);
	else {
		if (!SetWindowsHookEx(WH_CALLWNDPROCRET, dll_callwndretProc, dll_handle, GetWindowThreadProcessId(wnd, NULL)))
			MessageBox(NULL, str_to_wstr("SetWindowsHookEx() failed (WH_CALLWNDPROCRET) \n" + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HOOKPROC dll_getmessageProc = (HOOKPROC)GetProcAddress(dll_handle, "_dllgetmessageProc@12");
	if (dll_getmessageProc == NULL)
		MessageBox(NULL, str_to_wstr("GetProcAddress() failed (_dllgetmessageProc@12) \n" + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);
	else {
		
		if (!SetWindowsHookEx(WH_GETMESSAGE, dll_getmessageProc, dll_handle, GetWindowThreadProcessId(wnd, NULL)))
			MessageBox(NULL, str_to_wstr("SetWindowsHookEx() failed (WH_GETMESSAGE) \n" + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FreeLibrary(dll_handle);

	return TRUE;
}


#endif

