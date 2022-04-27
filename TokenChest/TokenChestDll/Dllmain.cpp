#include "Dll.h"

#include <iostream>
#include <string>
#include <memory>

#include "DllDump.h"

#include "Registry.h"
#include "common.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)
#pragma warning(disable: 4201)

using namespace std;

WndInfo g_info;
HWND g_TokenChest = NULL;
string g_TokenChestPath = "";
string ORIGINALCAPTION = "";

DLL_API LRESULT CALLBACK msgretProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		CWPRETSTRUCT msg = *((CWPRETSTRUCT*)lParam);		
		switch (msg.message) {
			case WM_CLOSE:{
				DumpGame(NULL, (DWORD)GetModuleHandle(L"D2Client.dll"));

				break;
			}
		}
	}
	return CallNextHookEx(0, nCode, wParam, lParam);
};

DLL_API int DUMP() {
	return getDumps();
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {	
	WndInfo* info = (WndInfo*)lParam;
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	if (pid == info->pid/* && getwindowtext(hwnd) == "Diablo II"*/) {
		info->hwnd = hwnd;
		return FALSE;
	}
	return TRUE;
}

HWND WINAPI getTokenHackDir(std::string *path) {
	CRegistry hregkey;
	if (!hregkey.Open(CREG_PATH, CREG_ROOTKEY))
		MessageBox(NULL, L"Could not open registry key", L"ERROR", MB_OK);
	*path = wstr_to_str(hregkey[CREG_KEY_TOKENCHEST_PATH]);
	hregkey.Close();

	return FindWindow(NULL, L"TokenChest");
}

INT APIENTRY DllMain(HMODULE /*hDLL*/, DWORD Reason, LPVOID /*Reserved*/) {
	switch (Reason) {
		case DLL_PROCESS_ATTACH:{
#ifdef _DEBUG
			SHOW_CONSOLE(true, true);
#endif

			g_info.pid = GetCurrentProcessId();
			EnumWindows(EnumWindowsProc, (LPARAM)&g_info);
			ORIGINALCAPTION = getwindowtext(g_info.hwnd);
			SetWindowText(g_info.hwnd, str_to_wstr(ORIGINALCAPTION + "        (injected by TokenChest)").c_str());

			g_TokenChest = getTokenHackDir(&g_TokenChestPath);

			if (int ret = initStringTables()) {
				MessageBox(NULL, str_to_wstr("Can't init string tables (locale strings) [" + int_to_str(ret) + "].\n").c_str(), L"ERROR", MB_OK);
				break;
			}
			if (!initBins()) {
				MessageBox(NULL, str_to_wstr("Error with one or more .bin files.\nAbort.\n").c_str(), L"ERROR", MB_OK);
				break;
			}
			if (!initStatOrder()) {
				MessageBox(NULL, str_to_wstr("Can't init Stat Order.\n").c_str(), L"ERROR", MB_OK);
				break;
			}
			
			break;
		}
		case DLL_PROCESS_DETACH:{
#ifdef _DEBUG
			FreeConsole();
#endif

			SetWindowText(g_info.hwnd, str_to_wstr(ORIGINALCAPTION).c_str());

			closeStringTables();

			break;
		}
	}
	return TRUE;
}

