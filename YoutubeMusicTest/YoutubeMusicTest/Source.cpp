#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <string>
#include <vector>

#include "resource.h"

HWND HWND_YOUTUBE;

HWND g_hwnd;
HINSTANCE g_hInst;

std::vector<HWND> Visualizer_HWNDS;

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void SHOW_CONSOLE(bool show = true, bool noclose = false) {
	static bool show_state = false;
	if (show && !show_state) {
		std::cout.clear();
		FILE *stream;
		AllocConsole();
		AttachConsole(GetCurrentProcessId());
		freopen_s(&stream, "CONOUT$", "w", stdout);
		if (noclose)
			EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_GRAYED);
	}
	if (!show)
		FreeConsole();
	show_state = show;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE/* hPrevInstance*/, LPSTR/* args*/, int/* iCmdShow*/) {
	g_hInst = hInstance;

	SHOW_CONSOLE();

	DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProc);

	return 0;
}
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	TCHAR text[255];
	GetWindowText(hwnd, text, 255);
	std::wstring window_text(text);
	printf("%ls\n", window_text.c_str());
	if (window_text.find(L"YouTube Music") != std::string::npos) {
		HWND_YOUTUBE = hwnd;
		return FALSE;
	}
	HWND_YOUTUBE = NULL;
	return TRUE;
}

void post_right_click(POINT pt, int state, HWND wnd) {
	if (state == 0 || state < 0)
		PostMessage(wnd, WM_RBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
	if (state == 0 || state > 0)
		PostMessage(wnd, WM_RBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			g_hwnd = hwnd;

			RECT rcclient;
			GetClientRect(hwnd, &rcclient);

			EnumWindows(EnumWindowsProc, NULL);

			break;
		}
		case WM_PAINT: {
			RECT rcclient;
			GetClientRect(hwnd, &rcclient);

			HDC hDC = GetDC(hwnd);

			HDC hDCmem = CreateCompatibleDC(hDC);

			HBITMAP hbmScreen = CreateCompatibleBitmap(hDC, rcclient.right, rcclient.bottom);
			HBITMAP hbmOldBitmap = (HBITMAP)SelectObject(hDCmem, hbmScreen);

			/////////////////////////////////////////////////////////////

			HBRUSH backbrush = CreateSolidBrush(RGB(10, 10, 10));
			FillRect(hDCmem, &rcclient, backbrush);
			DeleteObject(backbrush);

			/////////////////////////////////////////////////////////////

			BitBlt(hDC, 0, 0, rcclient.right, rcclient.bottom, hDCmem, 0, 0, SRCCOPY);

			//cleanup
			SelectObject(hDCmem, hbmOldBitmap);
			DeleteObject(hbmScreen);

			DeleteDC(hDCmem);
			ReleaseDC(hwnd, hDC);

			break;
		}
		case WM_ERASEBKGND: {
			return TRUE;
		}
		case WM_COMMAND: {
			HWND ihwnd = (HWND)lParam;
			UINT iid = LOWORD(wParam);
			switch (HIWORD(wParam)) {
				case BN_CLICKED: {
					switch (iid) {
						
					}
					break;
				}
			}
			break;
		}
		case WM_CHAR: {
			break;
		}
		case WM_SIZE: {
			break;
		}
		case WM_LBUTTONDOWN: {
			break;
		}
		case WM_LBUTTONUP: {
			/*static bool show = true;
			ShowWindow(HWND_YOUTUBE, show ? SW_SHOW : SW_HIDE);
			show = !show;*/
			RECT rc;
			GetWindowRect(HWND_YOUTUBE, &rc);
			post_right_click({rc.left+ 2,rc.top + 39}, 0, HWND_YOUTUBE);

			break;
		}
		case WM_RBUTTONUP: {

			break;
		}
		case WM_MOUSEMOVE: {
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			EndDialog(hwnd, 0);
			break;
		}
		default: return FALSE;
	}
	return TRUE;
}

