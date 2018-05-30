#include <Windows.h>
#include <iostream>

#include "C:\CPPlibs\common\f\common.h"
#include "resource.h"

using namespace std;

LRESULT CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

//LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam);

HHOOK mhook;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE/* hPrevInstance*/, LPSTR/* args*/, int/* iCmdShow*/) {
	SHOW_CONSOLE();
	//mhook = SetWindowsHookEx(WH_MSGFILTER, hookProc, 0, 0);

	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProc, 0);

	return 0;
}

LRESULT CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			RegisterTouchWindow(hwnd, 0);
			
			break;
		}
		case WM_TOUCH: {
			cout << "touch" << '\n';
			break;
		}
		case WM_DESTROY:
		case WM_CLOSE: {
			EndDialog(hwnd, 0);
			break;
		}
	}

	return FALSE;
}

//LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam) {
//	MSLLHOOKSTRUCT ms = *((MSLLHOOKSTRUCT*)lParam);
//
//
//
//	return CallNextHookEx(0, code, wParam, lParam);
//}
