#ifndef TokenDLL
#define TokenDLL

#include <Windows.h>
#include <iostream>
#include "common.h"

#define DLL_NAME TEXT("Tokenhackdll.dll")

UINT WM_DLL_HOOKPROC = RegisterWindowMessage(TEXT("dllhooked"));

#define DLL_API __declspec(dllexport)

extern "C"{
	DLL_API LRESULT CALLBACK dllcallwndProc(int code, WPARAM wParam, LPARAM lParam);
	DLL_API LRESULT CALLBACK dllcallwndretProc(int code, WPARAM wParam, LPARAM lParam);
	DLL_API LRESULT CALLBACK dllgetmessageProc(int code, WPARAM wParam, LPARAM lParam);
}

BOOL InjectDll(wchar_t *dllName, HWND wnd, HHOOK* callwndhook, HHOOK* callwndrethook, HHOOK* getmessagehook) {
	HMODULE dll_handle = LoadLibrary(dllName);
	if (dll_handle == NULL) {
		MessageBox(NULL, str_to_wstr("LoadLibrary() failed or\nTokenhackdll.dll could not be found " + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HOOKPROC dll_callwndProc = (HOOKPROC)GetProcAddress(dll_handle, "_dllcallwndProc@12");
	if (dll_callwndProc == NULL)
		MessageBox(NULL, str_to_wstr("GetProcAddress() failed " + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);

	*callwndhook = SetWindowsHookEx(WH_CALLWNDPROC, dll_callwndProc, dll_handle, GetWindowThreadProcessId(wnd, NULL));
	if (!*callwndhook)
		MessageBox(NULL, str_to_wstr("SetWindowsHookEx() failed " + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HOOKPROC dll_callwndretProc = (HOOKPROC)GetProcAddress(dll_handle, "_dllcallwndretProc@12");
	if (dll_callwndretProc == NULL)
		MessageBox(NULL, str_to_wstr("GetProcAddress() failed " + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);

	*callwndrethook = SetWindowsHookEx(WH_CALLWNDPROCRET, dll_callwndretProc, dll_handle, GetWindowThreadProcessId(wnd, NULL));
	if (!*callwndrethook)
		MessageBox(NULL, str_to_wstr("SetWindowsHookEx() failed " + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	HOOKPROC dll_getmessageProc = (HOOKPROC)GetProcAddress(dll_handle, "_dllgetmessageProc@12");
	if (dll_getmessageProc == NULL)
		MessageBox(NULL, str_to_wstr("GetProcAddress() failed " + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);

	*getmessagehook = SetWindowsHookEx(WH_GETMESSAGE, dll_getmessageProc, dll_handle, GetWindowThreadProcessId(wnd, NULL));
	if (!*getmessagehook)
		MessageBox(NULL, str_to_wstr("SetWindowsHookEx() failed " + error_code_to_text(GetLastError())).c_str(), TEXT("Error"), MB_OK);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FreeLibrary(dll_handle);

	return TRUE;
}


#endif

