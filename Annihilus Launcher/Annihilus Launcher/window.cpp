#include <Windows.h>
#include <vector>

#include "window.h"
#include "resource.h"

const TCHAR className[] = TEXT("Annihilus Launcher");
const UINT WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

HWND g_hwnd;

HWND launch_btn;
HWND settings_btn;
HWND close_btn;
HWND skiptobnet_btn;
HWND windowmode_btn;
HWND showconsole_btn;

HHOOK kbhook;
HHOOK mhook;
HCURSOR idle_cursor;
HCURSOR press_cursor;
HCURSOR loading_cursor;
HMENU g_menu;



NOTIFYICONDATA g_notifyIconData;

bool move_window = false; //right click on control then use arrow keys to move, shift makes it move faster, ctrl + arrow keys resize the window
HWND lastwindow = NULL;

std::vector<HWND> old_d2windowlist;
std::vector<HWND> new_d2windowlist;

DWORD WMC_HOTKEY = 368;

ATOM register_class(HINSTANCE hInstance) {
	WNDCLASSEX wnd = {0};
	wnd.hInstance = hInstance;
	wnd.lpszClassName = className;
	wnd.lpfnWndProc = WndProc;
	wnd.style = NULL;
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.hIconSm = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(ANI_ICON));
	return RegisterClassEx(&wnd);
}

void InitNotifyIconData() {
	memset(&g_notifyIconData, 0, sizeof(NOTIFYICONDATA));
	g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	g_notifyIconData.hWnd = g_hwnd;
	g_notifyIconData.uID = 5000;
	g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	g_notifyIconData.uCallbackMessage = WM_TRAYICON;
	g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(ANI_ICON), IMAGE_ICON, 0, 0, LR_SHARED);
	g_notifyIconData.uTimeout = 0;
	g_notifyIconData.uVersion = NOTIFYICON_VERSION;
	memcpy(g_notifyIconData.szInfoTitle, VERSION, 64);
	memcpy(g_notifyIconData.szInfo, TEXT("Click icon to see menu"), 256);
	memcpy(g_notifyIconData.szTip, VERSION, 128);
	g_notifyIconData.dwInfoFlags = NIIF_INFO;
	Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
}

void Minimize() {
	ShowWindow(g_hwnd, SW_HIDE);
}
void Restore() {
	SetWindowPos(g_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetForegroundWindow(g_hwnd);
}

