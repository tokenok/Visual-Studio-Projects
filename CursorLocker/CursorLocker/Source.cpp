#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <Commctrl.h>
#pragma comment(lib, "Comctl32.lib")

#include "resource.h"

#include "C:\CPPlibs\common\f\common.h"

using namespace std;

HWND g_hwnd;

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK kbHookProc(int code, WPARAM wParam, LPARAM lParam);

void func1() {
	static bool toggle = false;

	toggle = !toggle;
	
	if (toggle) {
		int screenX = GetSystemMetrics(SM_CXSCREEN);
		int screenY = GetSystemMetrics(SM_CYSCREEN);

		RECT rc = { 0, 0, screenX, screenY };

		cout << screenX << " " << screenY << '\n';

		ClipCursor(&rc);
	}
	else {
		ClipCursor(NULL);
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int iCmdShow) {
	//SHOW_CONSOLE(true, false);

	HHOOK kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)kbHookProc, NULL, NULL);

	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProc, 0);

	UnhookWindowsHookEx(kbhook);

	return 0;
}
BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			g_hwnd = hwnd;

			InitCommonControls();

			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			EndDialog(hwnd, 0);
			DestroyWindow(hwnd);
			g_hwnd = NULL;
			break;
		}
	}

	return FALSE;
}
LRESULT CALLBACK kbHookProc(int code, WPARAM wParam, LPARAM lParam) {
	static bool is_cap = false;

	if (code < 1) {
		LPKBDLLHOOKSTRUCT key = (LPKBDLLHOOKSTRUCT)lParam;

		int modifier_key = 0;

		if ((GetKeyState(VK_MENU) & 0x8000) != 0) modifier_key += 256;
		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) modifier_key += 256 * 4;
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) modifier_key += 256 * 2;
		if ((GetKeyState(VK_LWIN) & 0x8000) != 0 || (GetKeyState(VK_RWIN) & 0x8000) != 0) modifier_key += 256 * 8;

		const int ALT = 256, CTRL = 512, SHIFT = 1024, WINDOWS = 2048;
		
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			if (modifier_key == ALT && key->vkCode == VK_F3) {
				func1();
			}
		}
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}