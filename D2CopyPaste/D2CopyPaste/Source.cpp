#include <Windows.h>
#include <iostream>
#include <thread>

#include "C:\CPPlibs\common\common.h"

#include "resource.h"

TCHAR className[] = TEXT("D2CopyPaste");
TCHAR classTitle[] = TEXT("D2 Copy Paste");
UINT WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");
#define WM_TRAYICON (WM_USER+1)

HWND g_hwnd;
HHOOK kbhook;
HMENU g_menu;
NOTIFYICONDATA g_notifyIconData;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam);

void sendkey(wchar_t ch, HWND wnd) {
	PostMessage(wnd, WM_CHAR, ch, 0x00000001 | (LPARAM)((OemKeyScan(ch)) << 16));
}
void sendkeys(wstring s, HWND wnd, UINT TIME) {
	for (UINT i = 0; i < s.length(); i++) {
		sendkey(s[i], wnd);
		if (TIME)
			this_thread::sleep_for(chrono::milliseconds(TIME));
	}
}

void InitNotifyIconData() {
	memset(&g_notifyIconData, 0, sizeof(NOTIFYICONDATA));
	g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	g_notifyIconData.hWnd = g_hwnd;
	g_notifyIconData.uID = 5000;
	g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	g_notifyIconData.uCallbackMessage = WM_TRAYICON;
	g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(TOKENICO), IMAGE_ICON, 0, 0, LR_SHARED);
	memcpy(g_notifyIconData.szTip, classTitle, 128);
	Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
}

ATOM init_register_class(HINSTANCE hInstance) {
	WNDCLASSEX wnd = { 0 };
	wnd.hInstance = hInstance;
	wnd.lpszClassName = className;
	wnd.lpfnWndProc = WndProc;
	wnd.style = NULL;
	wnd.cbSize = sizeof(WNDCLASSEX);
	return RegisterClassEx(&wnd);
}

void Minimize() {
	ShowWindow(g_hwnd, SW_HIDE);
}
void Restore() {
	ShowWindow(g_hwnd, SW_SHOW);
	SetForegroundWindow(g_hwnd);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int iCmdShow) {
	HANDLE Mutex_handle = CreateMutex(NULL, TRUE, (LPCWSTR)"D2CopyPasteMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("Can only run one instance at a time"), TEXT("Error"), MB_OK);
		return 0;
	}

	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

	init_register_class(hInstance);
	g_hwnd = CreateWindow(className, classTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 350, 150, 400, 220, NULL, NULL, hInstance, NULL);

	InitNotifyIconData();
	Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
	UnhookWindowsHookEx(kbhook);

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HINSTANCE hInst;
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd)) {
		InitNotifyIconData();
		Minimize();
		init_register_class(hInst);
		return 0;
	}
	switch (message) {
		case WM_CREATE: {
			hInst = ((LPCREATESTRUCT)lParam)->hInstance;

			g_menu = CreatePopupMenu();
			AppendMenu(g_menu, MF_STRING, 3000, TEXT("Exit"));

			break;
		}
		case WM_TRAYICON: {
			switch (lParam) {
				case WM_LBUTTONUP: {
					if (ShowWindow(g_hwnd, SW_SHOW))
						Minimize();
					else
						Restore();
					break;
				}
				case WM_RBUTTONUP: {
					POINT pt;
					GetCursorPos(&pt);
					SetForegroundWindow(hwnd);
					UINT clicked = TrackPopupMenu(g_menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, pt.x, pt.y, 0, hwnd, NULL);
					switch (clicked) {
						case 3000: {
							PostQuitMessage(0);
							break;
						}
					}
					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			ShowWindow(hwnd, SW_HIDE);
			PostQuitMessage(0);
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT key = *((KBDLLHOOKSTRUCT*)lParam);
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			
		if ((GetKeyState(VK_MENU) & 0x8000) != 0) key.vkCode += 256;
		if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) key.vkCode += 256 * 4;		
		if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) key.vkCode += 256 * 2;		
		if ((GetKeyState(VK_LWIN) & 0x8000) != 0) key.vkCode += 256 * 8;		
		if ((GetKeyState(VK_RWIN) & 0x8000) != 0) key.vkCode += 256 * 8;		

		if (key.vkCode == 598) { //CTRL + V			
			if (getwindowtext(GetForegroundWindow()).find("Diablo II") != string::npos) {
				wstring ws = GetClipboardTextW();
				ws.erase(std::remove(ws.begin(), ws.end(), L'\r'), ws.end());
				ws.erase(std::remove(ws.begin(), ws.end(), L'\n'), ws.end());
				sendkeys(ws, GetForegroundWindow(), 0);
			}
		}
	}

	return CallNextHookEx(kbhook, code, wParam, lParam);
}

