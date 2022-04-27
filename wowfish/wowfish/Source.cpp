#define NOMINMAX
#include <Windows.h>

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <random>
#include <algorithm>

#include "resource.h"

#include "C:\CPPlibs\common\common.h"

const TCHAR className[] = TEXT("wowfish");

HWND g_hwnd;

HHOOK kbhook;

LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam);

NOTIFYICONDATA g_notifyIconData;
HMENU g_menu;

const UINT WM_TASKBARCREATED = RegisterWindowMessageW(L"TaskbarCreated");
#define WM_TRAYICON (WM_USER + 1)

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

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
	memcpy(g_notifyIconData.szInfoTitle, L"wowfish", 64);
	memcpy(g_notifyIconData.szInfo, L"wowfish", 256);
	memcpy(g_notifyIconData.szTip, L"wowfish", 128);
	g_notifyIconData.dwInfoFlags = NIIF_INFO;
	Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
}

void move_mouse_to_poiont_safe(POINT pt) {
	POINT cur = getcursorpos();

	double m = (double)(pt.y - cur.y) / (double)(pt.x - cur.x);
	double b = pt.y - (m * pt.x);

	int steps = std::abs(pt.x - cur.x);
	int xdir = 1 * (pt.x < cur.x ? -1 : 1);

	for (int i = 0; i < steps; i++) {
		cur.x += xdir;
		cur.y = m * cur.x + b;

		SetCursorPos(cur.x, cur.y);

		this_thread::sleep_for(chrono::nanoseconds(rand() % 400 + 100));
	}	
}

void isolate_red(BYTE* bmp, int w, int h, std::vector<POINT>* red_pixels) {
	auto is_red = [](COLORREF col) -> bool {
		byte r, g, b;
		GetRGB(col, &r, &g, &b);
		double f = 1.6;
		return (r > 100 && r > ((double)g * f) && r > ((double)b * f));
	};

	for (int i = h / 3; i < 2 * h / 3; i++) {
		for (int j = w; j < 2 * w; j += 3) {
			int pos = i * 3 * w + j;
			COLORREF col = RGB(bmp[pos], bmp[pos + 1], bmp[pos + 2]);
			bool bred = is_red(col);
			COLORREF newcol = bred ? col : RGB(0, 0, 0);
			bmp[pos + 0] = GetRValue(newcol);
			bmp[pos + 1] = GetGValue(newcol);
			bmp[pos + 2] = GetBValue(newcol);
			if (bred) {
				red_pixels->push_back({j / 3, i});
			}
		}
	}

	array_to_bmp("output.bmp", bmp, w, h);
}

//state: down = -1; down then up = 0; up = 1;
void post_left_click(POINT pt, int state = 0, HWND wnd = GetForegroundWindow()) {
	if (state == 0 || state < 0)
		PostMessage(wnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
	if (state == 0 || state > 0)
		PostMessage(wnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}

POINT find_bobber() {
	POINT ret = {0, 0};

	static int pic_count = 0;
	pic_count++;

	int w, h;
	BYTE* bmp = windowcapture(GetForegroundWindow(), &w, &h, false, "output" + int_to_str(pic_count) + ".bmp");
//	BYTE* bmp = bmp_to_array("C:\\Users\\Josh\\Documents\\ShareX\\Screenshots\\2021\\b.bmp", w, h);

  	std::vector<POINT> red_pixels;
 	isolate_red(bmp, w, h, &red_pixels);
	if (red_pixels.size()) {
		for (int i = 0; i < red_pixels.size(); i++) {
			ret.x += red_pixels[i].x;
			ret.y += red_pixels[i].y;
		}
		ret.x /= red_pixels.size();
		ret.y /= red_pixels.size();

		std::cout << ret.x << " " << ret.y << '\n';
	}
	
	delete[] bmp;

	return ret;
}

void click_bobber(POINT feather_pt) {
	feather_pt.x += rand() % 20 + 15;
	feather_pt.y += rand() % 20 + 20;
	this_thread::sleep_for(chrono::milliseconds(rand() % 50 + 40));
	move_mouse_to_poiont_safe(feather_pt);
	this_thread::sleep_for(chrono::milliseconds(rand() % 50 + 40));
	post_left_click(feather_pt);
	this_thread::sleep_for(chrono::milliseconds(rand() % 200 + 1000));
}

void cast() {
	//TODO: press 1 or something
}

void start_fishin() {
	cast();

	this_thread::sleep_for(chrono::milliseconds(rand() % 200 + 3000));
	POINT init_pos = find_bobber();

	bool bobber_dipped = false;
	while (!bobber_dipped) {
		POINT bobber = find_bobber();
		if (bobber.y > init_pos.y - 7) {
			bobber_dipped = true;
		}
		this_thread::sleep_for(chrono::milliseconds(rand() % 5 + 10));
	}

	click_bobber(init_pos);
}

HHOOK mhook;
LRESULT CALLBACK mhookProc(int code, WPARAM wParam, LPARAM lParam);

double timer = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*args*/, int /*iCmdShow*/) {
	HANDLE Mutex_handle = CreateMutexW(NULL, TRUE, L"THE MUTEX OF POWER");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("Error: Can only run one instance at a time"), TEXT("Error"), MB_OK);
		return 0;
	}

	timer = clock();
	mhook = SetWindowsHookEx(WH_MOUSE_LL, mhookProc, NULL, 0);

	SHOW_CONSOLE();

	srand(clock());

	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

	DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DialogProc, 0);
	
	UnhookWindowsHookEx(kbhook);

	ShowWindow(g_hwnd, SW_HIDE);
	CloseHandle(Mutex_handle);

	//return 0;
}
BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd)) {
		Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);
		Restore();
		return FALSE;
	}
	switch (message) {
		case WM_INITDIALOG: {
			g_hwnd = hwnd;

			//InitCommonControls();

			g_menu = CreatePopupMenu();

			InitNotifyIconData(hwnd);
			Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

			break;
		}
		case WM_CTLCOLORDLG: {
			HDC hDC = (HDC)wParam;

			RECT rc = getclientrect(hwnd);
			HBRUSH background = CreateSolidBrush(RGB(10,10,10));
			FillRect(hDC, &rc, background);
			DeleteObject(background);

			ReleaseDC(hwnd, hDC);

			break;
		}
		case WM_ERASEBKGND: {
			return 0;
		}
		case WM_SYSCOMMAND: {
			switch (wParam) {
				case SC_MINIMIZE: {
					Minimize();
					return TRUE;
				}
			}
			break;
		}
		case WM_TRAYICON: {
			switch (lParam) {
				case WM_LBUTTONUP: {
					if (ShowWindow(hwnd, SW_SHOW))
						Minimize();
					else
						Restore();
					break;
				}
				case WM_RBUTTONDOWN: {
					enum {
						close = 1
					};
					InsertMenu(g_menu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, close, TEXT("Exit"));
					POINT pt = getcursorpos();
					UINT clicked = TrackPopupMenu(g_menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, pt.x, pt.y, NULL, hwnd, NULL);
					switch (clicked) {
						case close: {
							PostQuitMessage(0);
						}
					}
					DeleteMenu(g_menu, 0, MF_BYPOSITION);

					break;
				}
			}
			break;
		}
		case WM_CLOSE:
		case WM_DESTROY: {
			Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
			DestroyMenu(g_menu);
			EndDialog(hwnd, 0);
			break;
		}
	}
	return FALSE;
}

LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT kbhs = *((KBDLLHOOKSTRUCT*)lParam);

	int key = GetKeyFromKBDLLHOOKSTRUCT(kbhs);

	enum {
		ALT = 256,
		CTRL = 512,
		SHIFT = 1024
	};

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		if (key == VK_SPACE) {
			system("cls");
			timer = clock();
	//		find_bobber();
		}
	}

	return CallNextHookEx(kbhook, code, wParam, lParam);
}

LRESULT CALLBACK mhookProc(int code, WPARAM wParam, LPARAM lParam) {
	MSLLHOOKSTRUCT mouse = *((MSLLHOOKSTRUCT*)lParam);

	if (wParam == WM_RBUTTONDOWN) {
		std::cout << clock() - timer << " down" << '\n';
		timer = clock();
	}
	else if (wParam == WM_RBUTTONUP) {
		std::cout << clock() - timer << " up" << '\n';
		timer = clock();
	}
	else if (wParam == WM_MOUSEMOVE) {
		//DO NOTHING
	}
	else {
	//	std::cout << wParam << '\n';
	}

	return CallNextHookEx(mhook, code, wParam, lParam);
}