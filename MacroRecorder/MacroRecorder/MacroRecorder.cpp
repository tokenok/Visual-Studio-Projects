#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <thread>

#include "resource.h"

#include "C:\CPPlibs\common\f\common.h"

BOOL CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam);

HHOOK kbhook;
HHOOK mhook;

HWND g_hwnd;

HMENU g_menu;

NOTIFYICONDATA g_notifyIconData;

const UINT WM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated");
#define WM_TRAYICON (WM_USER + 1)

const UINT SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
const UINT SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);

void Minimize() {
	ShowWindow(g_hwnd, SW_HIDE);
}
void Restore() {
	SetWindowPos(g_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	SetForegroundWindow(g_hwnd);
}
void InitNotifyIconData(HWND hwnd) {
	g_notifyIconData = {0};
	g_notifyIconData.cbSize = sizeof(g_notifyIconData);
	g_notifyIconData.hWnd = hwnd;
	g_notifyIconData.uID = 5000;
	g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	g_notifyIconData.uCallbackMessage = WM_TRAYICON;
//	g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0, LR_SHARED);
	g_notifyIconData.uTimeout = 0;
	g_notifyIconData.uVersion = NOTIFYICON_VERSION;
	memcpy(g_notifyIconData.szInfoTitle, L"Macro Recorder", 64);
	memcpy(g_notifyIconData.szInfo, L"made by token", 256);
	memcpy(g_notifyIconData.szTip, L"Macro Recorder", 128);
	g_notifyIconData.dwInfoFlags = NIIF_INFO;
	Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
	DestroyIcon(g_notifyIconData.hIcon);
}
void ChangeShellIcon(UINT id) {
	g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(id), IMAGE_ICON, 0, 0, LR_SHARED);
	Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
	DestroyIcon(g_notifyIconData.hIcon);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR args, int iCmdShow) {
	SHOW_CONSOLE();

	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, hookProc, NULL, 0);
	mhook = SetWindowsHookEx(WH_MOUSE_LL, hookProc, NULL, 0);

	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOGMAIN), NULL, (DLGPROC)DialogProc, 0);

	UnhookWindowsHookEx(kbhook);
	UnhookWindowsHookEx(mhook);
}

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd)) {
		Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);
		Restore();
		return FALSE;
	}
	switch (message) {
		case WM_INITDIALOG:	{
			g_hwnd = hwnd;

	//		InitCommonControls();

			g_menu = CreatePopupMenu();

			InitNotifyIconData(hwnd);
			Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

			SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			SetLayeredWindowAttributes(hwnd, NULL, NULL, NULL);

			break;
		}
		case WM_ERASEBKGND:{
			return 0;
		}
		case WM_COMMAND:{
			switch (wParam) {
				
			}
			break;
		}
		case WM_SYSCOMMAND:{
			switch (wParam) {
				case SC_MINIMIZE:{
					Minimize();
					return TRUE;
				}
			}
			break;
		}
		case WM_TRAYICON:{
			switch (lParam) {
				case WM_LBUTTONUP:{
					if (ShowWindow(hwnd, SW_SHOW))
						Minimize();
					else
						Restore();
					break;
				}
				case WM_RBUTTONDOWN:{
					enum {
						close = 1,
						dump,
						auto_dump
					};
					
					InsertMenu(g_menu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, auto_dump, TEXT("Auto Dump"));
					InsertMenu(g_menu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, dump, TEXT("Manual Dump"));
					InsertMenu(g_menu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, close, TEXT("Exit"));
					POINT pt = getcursorpos();
					UINT clicked = TrackPopupMenu(g_menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, pt.x, pt.y, NULL, hwnd, NULL);
					switch (clicked) {
						case close: {
							PostQuitMessage(0);
						}
						case dump: {
							
							break;
						}
						case auto_dump: {
							
							break;
						}
					}
					DeleteMenu(g_menu, 0, MF_BYPOSITION);
					DeleteMenu(g_menu, 0, MF_BYPOSITION);
					DeleteMenu(g_menu, 0, MF_BYPOSITION);

					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY:{
			Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);

			DestroyMenu(g_menu);

			EndDialog(hwnd, 0);
			break;
		}
	}
	return FALSE;
}

class Recorder {	
	map<DWORD, INPUT> events;
	
	bool is_recording;

	bool is_playing;

	public:

	Recorder() {
		is_recording = false;
		is_playing = false;
	}
	~Recorder() {
		events.clear();
	}

	void reset() {
		events.clear();
	}

	void start() {
		reset();

		is_recording = true;
	}

	void stop() {
		is_recording = false;

		if (events.size())
			events.erase(prev(events.end()));
	}

	bool play() {
		if (!events.size())
			return false;

		DWORD t = events.begin()->first;

		is_playing = true;

		for (auto a : events) {
			if (!is_playing)
				break;

			SendInput(1, &a.second, sizeof(a.second));

			this_thread::sleep_for(chrono::milliseconds(a.first - t));
			t = a.first;
		}

		is_playing = false;

		return true;
	}

	bool isRecording() {
		return is_recording;
	}

	void addEvent(UINT time, INPUT e) {
		events[time] = e;
	}

	map<DWORD, INPUT>& getEvents() {
		return this->events;
	}
};

Recorder g_recorder;

enum Hotkeys {
	HK_RUN = 113, //F2
	HK_STOP = 115, //F4
	HK_RECORD = 114, //F3
	HK_LOAD = 0,
	HK_SAVE = 0
};

bool ProcessHotkeys(const KBDLLHOOKSTRUCT &kbhs, WPARAM wParam) {
	DWORD key = kbhs.vkCode;
	if ((GetKeyState(VK_MENU) & 0x8000) != 0) key += 256;
	if ((GetKeyState(VK_CONTROL) & 0x8000) != 0) key += 256 * 2;
	if ((GetKeyState(VK_SHIFT) & 0x8000) != 0) key += 256 * 4;
	if ((GetKeyState(VK_LWIN) & 0x8000) != 0) key += 256 * 8;
	if ((GetKeyState(VK_RWIN) & 0x8000) != 0) key += 256 * 8;

	if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
		if (key == HK_RUN) {
			if (!g_recorder.isRecording())
				g_recorder.play();
			return true;
		}
		else if (key == HK_STOP) {
			g_recorder.stop();

			auto m = g_recorder.getEvents();

			for (auto a : m) {
				INPUT i = a.second;
				if (a.second.type == INPUT_KEYBOARD) {
					KEYBDINPUT ki = i.ki;
					cout << "keyboard: " << ki.wVk << " " << ki.time << '\n';
				}
				else if (a.second.type == INPUT_MOUSE) {
					MOUSEINPUT mi = i.mi;
					cout << "mouse: " << mi.dx << ", " << mi.dy << " " << mi.time << '\n';
				}
				else {
					cout << "other: \n";
				}
			}

			return true;
		}
		else if (key == HK_RECORD) {
			if (g_recorder.isRecording())
				g_recorder.stop();
			else
				g_recorder.start();
			return true;
		}
		else if (key == HK_LOAD) {

			return true;
		}
		else if (key == HK_SAVE) {

			return true;
		}
		else
			return false;
	}
	return false;
}

LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN
		|| wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
		KBDLLHOOKSTRUCT kbhs = *((KBDLLHOOKSTRUCT*)lParam);

		if (!ProcessHotkeys(kbhs, wParam) && g_recorder.isRecording()) {
			INPUT i;
			i.type = INPUT_KEYBOARD;
			i.ki.wVk = kbhs.vkCode;
			i.ki.wScan = kbhs.scanCode;
			i.ki.dwFlags = 
				wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN ? KEYEVENTF_EXTENDEDKEY 
				: wParam == WM_KEYUP || wParam == WM_SYSKEYUP ? KEYEVENTF_KEYUP 
				: 0;
			i.ki.time = GetTickCount();
			i.ki.dwExtraInfo = kbhs.dwExtraInfo;

			g_recorder.addEvent(i.ki.time, i);
		}
	}
	if (wParam == WM_MOUSEMOVE || wParam == WM_MOUSEWHEEL ||
		wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
		wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP) {
		MSLLHOOKSTRUCT ms = *((MSLLHOOKSTRUCT*)lParam);
		
		if (g_recorder.isRecording()) {
			INPUT i;
			i.type = INPUT_MOUSE;
			i.mi.dx = (LONG)((double)ms.pt.x * (65535.0 / (double)SCREEN_WIDTH));
			i.mi.dy = (LONG)((double)ms.pt.y * (65535.0 / (double)SCREEN_HEIGHT));
			i.mi.dwFlags = MOUSEEVENTF_ABSOLUTE
				| (wParam == WM_LBUTTONDOWN ? MOUSEEVENTF_LEFTDOWN : wParam == WM_LBUTTONUP ? MOUSEEVENTF_LEFTUP
				: wParam == WM_RBUTTONDOWN ? MOUSEEVENTF_RIGHTDOWN : wParam == WM_RBUTTONUP ? MOUSEEVENTF_RIGHTUP
				: wParam == WM_MOUSEMOVE ? MOUSEEVENTF_MOVE
				: wParam == WM_MOUSEWHEEL ? MOUSEEVENTF_WHEEL 
				: 0);
			i.mi.time = GetTickCount();
			i.mi.mouseData = (short)HIWORD(ms.mouseData);;
			i.mi.dwExtraInfo = ms.dwExtraInfo;

			g_recorder.addEvent(i.mi.time, i);
		}
		
	}

	return CallNextHookEx(kbhook, code, wParam, lParam);
}