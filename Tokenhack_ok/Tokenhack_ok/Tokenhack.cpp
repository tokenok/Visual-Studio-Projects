#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <thread>
#include <fstream>
#include <iostream>
#include <Shellapi.h>

#include "Shlwapi.h"
//lean and mean
#include <Commdlg.h>
#include <sapi.h>

#include "Tokenhack.h"
#include "customcontrols.h"
#include "autoroll.h"
#include "keypress.h"

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "winmm.lib")

HWND g_hwnd;

HWND g_statbox;
int g_statboxid = 100;

NOTIFYICONDATA g_notifyIconData;

HFONT font_a = NULL;

HMENU g_menu;

HHOOK kbhook;

HCURSOR idle_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(IDLECURSOR));
HCURSOR press_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(PRESSCURSOR));
HCURSOR loading_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(WAITCURSOR));
HCURSOR token_cursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(TOKENCURSOR));

ATOM customcontrolregatom1 = Button::RegisterCustomClass();
ATOM customcontrolregatom2 = CheckBox::RegisterCustomClass();
ATOM customcontrolregatom3 = RadioButton::RegisterCustomClass();
ATOM customcontrolregatom4 = StaticControl::RegisterCustomClass();
ATOM customcontrolregatom5 = ToggleButton::RegisterCustomClass();
ATOM customcontrolregatom6 = CustomTrackbar::RegisterCustomClass();

bool g_configState = Config::Load();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////Tokenhack functions//////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void WMC_PROC();
void CLOSE_PROC();
void NOPICKUP_PROC();
void ROLL_PROC();
void AUTOROLL_PROC();
void READSTATS_PROC();

tokenhackfunction WMC("Toggle Cursor Lock", 368, WMC_PROC);
tokenhackfunction CLOSE("Close", 369, CLOSE_PROC);
tokenhackfunction NOPICKUP("\"/nopickup\"", 118, NOPICKUP_PROC);
tokenhackfunction ROLL("Transmute", 121, ROLL_PROC);
tokenhackfunction AUTOROLL("Auto Roll (OCR)", 117, AUTOROLL_PROC);
tokenhackfunction READ("Read Stats (OCR)", 220, READSTATS_PROC);

void WMC_PROC() {
	static bool trap = false;
	if (!trap) {
		trap = true;
		HWND wnd = GetForegroundWindow();
		if (getwindowtext(wnd) == "Program Manager") {
			RECT rcwindow = getwindowrect(GetDesktopWindow());
			ClipCursor(&rcwindow);
		}
		else {
			RECT rcmappedclient = getmappedclientrect(wnd);
			ClipCursor(&rcmappedclient);
		}
	}
	else {
		trap = false;
		ClipCursor(NULL);
	}
}
void CLOSE_PROC() {
	PostQuitMessage(0);
}
void NOPICKUP_PROC() {
	d2type("/nopickup");
}
void ROLL_PROC() {
	HWND wnd = GetForegroundWindow();
	POINT transmute_btn;
	if (!get_transmute_pos(wnd, &transmute_btn)) {
		cout << "Invalid window resolution: cound not find transmute button" << '\n';
		cout << "Compatible resolutions: (640, 480) | (800, 600) | (1024, 768) | (1280, 1024)" << '\n';
		cout << "open confix.txt to manually set the transmute button pos" << '\n';
		return;
	}
	SendMessage(wnd, WM_LBUTTONDOWN, 0, MAKELPARAM(transmute_btn.x, transmute_btn.y));
	SendMessage(wnd, WM_LBUTTONUP, 0, MAKELPARAM(transmute_btn.x, transmute_btn.y));

	if (ROLL.on == 2) {
		this_thread::sleep_for(chrono::milliseconds(200));

		int x, y;
		BYTE* newbuf = windowcapture(GetForegroundWindow(), "capture.bmp", &x, &y);
		BYTE* newbuf_ocr = new BYTE[x * 3 * y];
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < 3 * x; j += 3) {
				int pos = i * x * 3 + j;
				newbuf_ocr[pos + 0] = newbuf[pos + 0];
				newbuf_ocr[pos + 1] = newbuf[pos + 1];
				newbuf_ocr[pos + 2] = newbuf[pos + 2];
			}
		}
		vector<string> item_stats = ocr(newbuf_ocr, x, y, font16, true);
		process(newbuf_ocr, x, y, "(ocr)(processed)capture.bmp", true);
		process(newbuf, x, y, "(processed)capture.bmp", true);
		delete[] newbuf;
		delete[] newbuf_ocr;

		if (CTB_VOLUME.current_val > 0 && item_stats.size() == 0)
			PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);

		vector<string> stats_in_file = load_stats();
		vector<string> item_stats_cpy = item_stats;
		compare_item_stats_to_file_stats(item_stats_cpy, stats_in_file, false);
		for (UINT i = 0; i < item_stats_cpy.size(); i++) {
			if (item_stats_cpy[i][0] == '#') {
				item_stats[i].insert(0, "## ");
				item_stats[i] += " ##";
			}
		}

		string statbox_text = "";
		for (auto & i : item_stats) {
			statbox_text += '>' + i + "\r\n";
			cout << "stats: " << i << '\n';
		}

		if (item_stats.size() > 0) {
			SetWindowText(g_statbox, str_to_wstr(statbox_text).c_str());
			cout << "-----------------------------------------------" << '\n';
		}
	}
}
void AUTOROLL_PROC() {
	HWND wnd = GetForegroundWindow();

	vector<string> stats_in_file = load_stats();

	if (stats_in_file.size() == 0) {
		SetWindowText(g_statbox, TEXT("No stats found in file"));
		cout << "No stats found in file" << '\n';
		if (CTB_VOLUME.current_val > 0)
			PlaySound(MAKEINTRESOURCE(NOTHING), NULL, SND_ASYNC | SND_RESOURCE);
		return;
	}

	string searching = "Searching for:\r\n";
	for (auto & a : stats_in_file)
		searching += '>' + a + "\r\n";
	searching += '\0';

	SendMessage(g_statbox, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(searching).c_str());

	POINT item_pt = getclientcursorpos(wnd);

	if (layered_window_fix) {
		if (!(GetWindowLongPtr(wnd, GWL_EXSTYLE) & WS_EX_LAYERED))
			SetWindowLong(wnd, GWL_EXSTYLE, GetWindowLongPtr(wnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(wnd, NULL, 255, LWA_ALPHA);
	}

	kill_all_OCR_auto_roll = false;

	//TODO: FIX THIS ?>???????SDKLJHFL:KAEJF:LKSAJEDF
	//thread roll(OCR_auto_roll, wnd, font16, item_pt);
	//roll.detach();
}
void READSTATS_PROC() {
	int x, y;
	BYTE* newbuf = windowcapture(GetForegroundWindow(), "capture.bmp", &x, &y);
	BYTE* newbuf_ocr = new BYTE[x * 3 * y];
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < 3 * x; j += 3) {
			int pos = i * x * 3 + j;
			newbuf_ocr[pos + 0] = newbuf[pos + 0];
			newbuf_ocr[pos + 1] = newbuf[pos + 1];
			newbuf_ocr[pos + 2] = newbuf[pos + 2];
		}
	}
	vector<string> item_stats = ocr(newbuf_ocr, x, y, font16, true);
	process(newbuf_ocr, x, y, "(ocr)(processed)capture.bmp", true);
	process(newbuf, x, y, "(processed)capture.bmp", true);
	delete[] newbuf;
	delete[] newbuf_ocr;

	if (CTB_VOLUME.current_val > 0 && item_stats.size() == 0)
		PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);

	if (READ.on == 2) {
		vector<string> stats_in_file = load_stats();
		vector<string> item_stats_cpy = item_stats;
		compare_item_stats_to_file_stats(item_stats_cpy, stats_in_file, false);
		for (auto & a : item_stats_cpy) {
			if (a[0] == '#') {
				a.insert(0, "## ");
				a += " ##";
			}
		}
	}

	string statbox_text = "";
	for (auto & a : item_stats) {
		statbox_text += '>' + a + "\r\n";
		cout << "stats: " << a << '\n';
	}

	if (item_stats.size() > 0) {
		SetWindowText(g_statbox, str_to_wstr(statbox_text).c_str());
		cout << "-----------------------------------------------" << '\n';
	}
	else {
		if (CTB_VOLUME.current_val > 0)
			PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
		//	SetWindowText(statbox, L"no stats found");
		std::cout << "no stats found" << '\n';
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


BasicControl_colorscheme togglebutton_scheme{
	RGB(10, 10, 10), RGB(0, 255, 0), RGB(0, 255, 0),			//IDLE ON: background/border/text
	RGB(45, 45, 45), RGB(0, 255, 0), RGB(0, 255, 0),			//HOVER ON: background/border/text
	RGB(10, 10, 10), RGB(255, 0, 0), RGB(255, 0, 0),			//IDLE OFF: background/border/text
	RGB(45, 45, 45), RGB(255, 0, 0), RGB(255, 0, 0)				//HOVER OFF: background/border/text
};

TrackbarColorScheme volume_scheme{
	RGB(10, 10, 10), RGB(50, 50, 50), RGB(80, 80, 80), RGB(127, 127, 127),							// background/border/window name idle/highlight
	RGB(0, 255, 0), RGB(10, 10, 10), RGB(0, 255, 0), RGB(10, 10, 10),								// left/right channel left/right highlight
	RGB(127, 127, 127), RGB(127, 127, 127), RGB(127, 127, 127), 									// thumb background: idle/hover/selected
	RGB(10, 10, 10), RGB(10, 10, 10), RGB(10, 10, 10),			 									// thumb border: idle/hover/selected
	RGB(127, 127, 127), RGB(127, 127, 127), RGB(127, 127, 127)										// thumb text: idle/hover/selected
};

BasicControl_colorscheme normalbutton_scheme{
	RGB(10, 10, 10), RGB(0, 117, 255), RGB(0, 117, 255), 		//IDLE: background/border/text
	RGB(25, 25, 25), RGB(0, 117, 255), RGB(0, 117, 255)			//HOVER: background/border/text	
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////

int window_id = 1000;

void TBTN_CONSOLE_PROC(WPARAM /*wParam*/, LPARAM /*lParam*/) {
	if (Config::showconsole) {
		FreeConsole();
	}
	else {
		SHOW_CONSOLE();
		SetWindowPos(g_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}
void BTN_RELOAD_PROC(WPARAM /*wParam*/, LPARAM lParam) {
	ClipCursor(NULL);
	RECT rcclient = getclientrect(g_hwnd);
	InvalidateRect(g_hwnd, &rcclient, true);
	SendMessage(g_hwnd, WM_CREATE, NULL, NULL);
	SendMessage(g_hwnd, WM_SIZE, NULL, NULL);
	if (HIWORD(lParam) == 0) {
		cout << "\n-------------------RELOAD-------------------" << '\n';
		if (CTB_VOLUME.current_val > 0)
			PlaySound(MAKEINTRESOURCE(RELOAD), NULL, SND_ASYNC | SND_RESOURCE);
		//TODO: fix reload button drawing
//		BTN_RELOAD.mouse_in_client = false;
//		RedrawWindow(BTN_RELOAD.handle, NULL, NULL, RDW_INVALIDATE);
	}
}
void BTN_LAUNCH_PROC(WPARAM /*wParam*/, LPARAM lParam) {
	if (Config::gamepath.size() == 0 || lParam != 0) {
		OPENFILENAME ofn;
		char File[MAX_PATH];
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = g_hwnd;
		ofn.lpstrFile = (LPWSTR)File;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = TEXT("exe\0*.exe*\0All\0*.*\0Shortcut\0*.lnk*\0Text\0*.TXT\0");
		ofn.lpstrTitle = L"Select executable to be launched";
		ofn.nFilterIndex = 0;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_NODEREFERENCELINKS;

		GetOpenFileName(&ofn);

		if (wcslen(ofn.lpstrFile) > 0)
			Config::write_gamepath(wstr_to_str(ofn.lpstrFile));
		else
			return;
	}
	string gamedir = Config::gamepath.substr(0, Config::gamepath.rfind('\\'));
	string gamename = Config::gamepath.substr(Config::gamepath.rfind('\\') + 1, Config::gamepath.size());

	if (Config::gamepath.size() > 0 && lParam == 0)
		ShellExecute(g_hwnd, TEXT("open"), str_to_wstr(gamename).c_str(), str_to_wstr(Config::targetlines).c_str(), str_to_wstr(gamedir).c_str(), SW_SHOW);
}
void BTN_CONFIG_PROC(WPARAM wParam, LPARAM) {
	ShellExecute((HWND)wParam, TEXT("open"), g_config_path.c_str(), NULL, NULL, SW_SHOW);
}

//these are not created here, only initialized, create in WM_CREATE
int tokenhackfunctionsheight = tokenhackfunction::tokenhackfunctions.size() * tokenhackfunction::static_control_height;
CustomTrackbar CTB_VOLUME("volume", WS_VISIBLE | WS_CHILD, 16, 40 + tokenhackfunctionsheight, 68, 14, window_id++, 0x0000, 0xFFFF, 0x4444, 0x0CCF, 0x199A, 0, 0, CTB_BORDER | CTB_SMOOTH | CTB_STAY, volume_scheme, press_cursor, press_cursor, 0, 0);
ToggleButton TBTN_HOOKOUTPUT("kb", WS_VISIBLE | WS_CHILD, 240, 40 + tokenhackfunctionsheight, 17, 14, window_id++, nullptr, togglebutton_scheme, font_a, press_cursor, false, 0, 0);
ToggleButton TBTN_CONSOLE("console", WS_VISIBLE | WS_CHILD, 170, 40 + tokenhackfunctionsheight, 64, 14, window_id++, TBTN_CONSOLE_PROC, togglebutton_scheme, font_a, press_cursor, false, 0, 0);
ToggleButton TBTN_SHOWONSTART("show on start", WS_VISIBLE | WS_CHILD, 263, 40 + tokenhackfunctionsheight, 98, 14, window_id++, nullptr, togglebutton_scheme, font_a, press_cursor, false, 0, 0);
Button BTN_CONFIG("Open Config", WS_VISIBLE | WS_CHILD, 14, 5 + tokenhackfunctionsheight, 100, 30, window_id++, BTN_CONFIG_PROC, normalbutton_scheme, font_a, press_cursor, 0, 0);
Button BTN_RELOAD("Reload", WS_VISIBLE | WS_CHILD, 140, 5 + tokenhackfunctionsheight, 80, 30, window_id++, BTN_RELOAD_PROC, normalbutton_scheme, font_a, press_cursor, 0, 0);
Button BTN_LAUNCH("Launch", WS_VISIBLE | WS_CHILD, 246, 5 + tokenhackfunctionsheight, 100, 30, window_id++, BTN_LAUNCH_PROC, normalbutton_scheme, font_a, press_cursor, 0, 0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam);

ATOM init_register_class(HINSTANCE hInstance) {
	WNDCLASSEX wnd = { 0 };
	wnd.hInstance = hInstance;
	wnd.lpszClassName = className;
	wnd.lpfnWndProc = WndProc;
	wnd.style = NULL;
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.hIconSm = (HICON)LoadIcon(hInstance, MAKEINTRESOURCE(TOKENICO));
	return RegisterClassEx(&wnd);
}

void InitNotifyIconData() {
	memset(&g_notifyIconData, 0, sizeof(NOTIFYICONDATA));
	g_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	g_notifyIconData.hWnd = g_hwnd;
	g_notifyIconData.uID = 5000;
	g_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	g_notifyIconData.uCallbackMessage = WM_TRAYICON;
	g_notifyIconData.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(TOKENICO), IMAGE_ICON, 0, 0, LR_SHARED);
	g_notifyIconData.uTimeout = 0;
	g_notifyIconData.uVersion = NOTIFYICON_VERSION;
	memcpy(g_notifyIconData.szInfoTitle, VERSION, 64);
	memcpy(g_notifyIconData.szInfo, TEXT("Click icon to see menu"), 256);
	memcpy(g_notifyIconData.szTip, VERSION, 128);
	g_notifyIconData.dwInfoFlags = NIIF_INFO;
	Shell_NotifyIcon(NIM_MODIFY, &g_notifyIconData);
}

string ProcessKey(KBDLLHOOKSTRUCT *key) {
	wchar_t name[256];
	LPARAM lparam = 1;
	lparam += key->scanCode << 16;
	lparam += key->flags << 24;
	GetKeyNameText(lparam, (LPTSTR)name, 256);
	string sname = wstr_to_str((wstring)name);
	if ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0 && key->vkCode != VK_LMENU && key->vkCode != VK_RMENU) {
		key->vkCode += 256;
		sname.insert(0, "ALT+");
	}
	if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 && key->vkCode != VK_LCONTROL && key->vkCode != VK_RCONTROL) {
		key->vkCode += 256 * 2;
		sname.insert(0, "CTRL+");
	}
	if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0 && key->vkCode != VK_LSHIFT && key->vkCode != VK_RSHIFT) {
		key->vkCode += 256 * 4;
		sname.insert(0, "SHIFT+");
	}
	if ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 && key->vkCode != VK_LWIN) {
		key->vkCode += 256 * 8;
		sname.insert(0, "LEFT WINDOWKEY+");
	}
	if ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0 && key->vkCode != VK_RWIN) {
		key->vkCode += 256 * 8;
		sname.insert(0, "RIGHT WINDOWKEY+");
	}
	return sname;
}

void statbox_display(HWND hwnd, bool skiptostatsinfile /* = false*/) {
	if (READ.on || AUTOROLL.on) {
		if (!skiptostatsinfile) {
			g_statbox = CreateWindow(TEXT("EDIT"), TEXT(""),
				WS_CHILD | WS_VISIBLE | WS_BORDER /*| ES_WANTRETURN*/ | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL/* | ES_READONLY*/ | WS_VSCROLL | WS_HSCROLL,
				0, 0,
				0, 0,
				hwnd, (HMENU)g_statboxid, NULL, NULL);
		}
		if (AUTOROLL.on) {
			vector<string> stats_in_file = load_stats();
			string searching = "Stats in file:                                         asdf\r\n";
			for (UINT i = 0; i < 10; i++)
				for (auto & a : stats_in_file)
					searching += '>' + a + "\r\n";
			searching += '\0';
			SendMessage(g_statbox, WM_SETTEXT, NULL, (LPARAM)str_to_wstr(searching).c_str());
		}
	}
}

void d2type(string s, HWND wnd, UINT entertime, UINT keytime) {
	if (entertime)
		this_thread::sleep_for(chrono::milliseconds(entertime));
	keydownup(VK_RETURN);
	if (entertime)
		this_thread::sleep_for(chrono::milliseconds(entertime));
	sendkeys(s, wnd, keytime);
	keydownup(VK_RETURN);
}
void d2type(vector<string> &v, HWND wnd, UINT entertime, UINT keytime) {
	for (auto a : v) {
		d2type(a, wnd, entertime, keytime);
	}
}

void Minimize() {
	if (CTB_VOLUME.current_val > 0)
		PlaySound(MAKEINTRESOURCE(TK), NULL, SND_ASYNC | SND_RESOURCE);
	ShowWindow(g_hwnd, SW_HIDE);
}
void Restore() {
	if (CTB_VOLUME.current_val > 0)
		PlaySound(MAKEINTRESOURCE(TELE), NULL, SND_ASYNC | SND_RESOURCE);
	//SetWindowPos(g_hwnd, HWND_TOP, 0, 0,
	//	(!READ.on && !AUTOROLL.on) ? windowwidth : windowwidth + 250/*180*/,
	//	functionnameheight*totalonfunctions + bottomheight + tokenhackfunctionwindowxposoffset,
	//	SWP_NOMOVE);
	ShowWindow(g_hwnd, SW_SHOW);
	SetForegroundWindow(g_hwnd);
}

void Resize() {
	SetWindowPos(g_hwnd, HWND_TOP, 0, 0, (!READ.on && !AUTOROLL.on) ? 400 : 400 + 250, tokenhackfunction::static_control_height * tokenhackfunction::tokenhackfunctions.size() + 140, SWP_NOMOVE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*args*/, int /*iCmdShow*/) {
	SHOW_CONSOLE();

	HANDLE Mutex_handle = CreateMutex(NULL, TRUE, L"Tokenhack_ok");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(NULL, TEXT("Error: Can only run one instance at a time"), TEXT("Error"), MB_OK);
		return 0;
	}

	kbhook = SetWindowsHookEx(WH_KEYBOARD_LL, kbhookProc, NULL, 0);

	init_register_class(hInstance);
	g_hwnd = CreateWindow(className, VERSION, WS_OVERLAPPEDWINDOW /*WS_POPUP|*/ | WS_CLIPCHILDREN, 350, 150, 400, 600, NULL, NULL, hInstance, NULL);

	if (Config::showonstart) ShowWindow(g_hwnd, SW_SHOW);

	InitNotifyIconData();
	Shell_NotifyIcon(NIM_ADD, &g_notifyIconData);

	if (CTB_VOLUME.current_val > 0)	PlayResourceSound(TELE);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteObject(font_a);

	UnhookWindowsHookEx(kbhook);
	Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);

	ClipCursor(NULL);

	ShowWindow(g_hwnd, SW_HIDE);
	CloseHandle(Mutex_handle);

	kill_all_OCR_auto_roll = true;
	for (auto a : original_window_titles)
		SetWindowText(a.first, str_to_wstr(a.second).c_str());
	if (original_window_titles.size() > 0)
		this_thread::sleep_for(chrono::milliseconds(100));

	cout << "EXITING" << '\n';
	if (CTB_VOLUME.current_val > 0)
		PlayResourceSound(BYE);

	return msg.wParam;
}

void btn_proc(WPARAM wParam, LPARAM lParam) {
	cout << "clicked\n";
}

Button btn("btn", WS_VISIBLE | WS_CHILD, 0, 400, 100, 40, 1, NULL, btn_proc);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (message == WM_TASKBARCREATED && !IsWindowVisible(g_hwnd)) {
		InitNotifyIconData();
		Minimize();
		return 0;
	}
	switch (message) {
		case WM_CREATE: {
			static bool firstrun = true;
			if (firstrun) {
				CTB_VOLUME.start = false;

				for (UINT i = 0; i < tokenhackfunction::tokenhackfunctions.size(); i++) {
					tokenhackfunction::tokenhackfunctions[i]->checkbox_button.Create(hwnd);
					tokenhackfunction::tokenhackfunctions[i]->hotkey_button.Create(hwnd);
					tokenhackfunction::tokenhackfunctions[i]->static_control.Create(hwnd);
				}

				CTB_VOLUME.Create(hwnd);
				TBTN_HOOKOUTPUT.Create(hwnd);
				TBTN_CONSOLE.Create(hwnd);
				TBTN_SHOWONSTART.Create(hwnd);
				BTN_CONFIG.Create(hwnd);
				BTN_RELOAD.Create(hwnd);
				BTN_LAUNCH.Create(hwnd);

				firstrun = false;
			}

			btn.client_cursor = press_cursor;
			btn.Create(hwnd);

			//TODO uncomment this line
	//		SHOW_CONSOLE(Config::showconsole);

			statbox_display(hwnd);

			INITCOMMONCONTROLSEX icex;
			icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
			icex.dwICC = ICC_HOTKEY_CLASS;
			InitCommonControlsEx(&icex);

			Resize();

			DestroyMenu(g_menu);
			g_menu = CreatePopupMenu();
			AppendMenu(g_menu, MF_STRING, 3002, TEXT("config"));
		//	AppendMenu(g_menu, MF_STRING, 3001, TEXT("Info"));
			AppendMenu(g_menu, MF_STRING, 3000, TEXT("Exit"));

			break;
		}
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(g_hwnd, &ps);

			//paint background
			HBRUSH hb_background = CreateSolidBrush(RGB(10, 10, 10));
			FillRect(hDC, &ps.rcPaint, hb_background);
			DeleteObject(hb_background);

			HPEN hpenOld = static_cast<HPEN>(SelectObject(hDC, GetStockObject(DC_PEN)));
			HBRUSH hbrushOld = static_cast<HBRUSH>(SelectObject(hDC, GetStockObject(NULL_BRUSH)));

			////border of tokenhackfunction tab
			//RECT tfb;
			//tfb.right = 5 + tokenhackfunction::static_control_width + 1;
			//tfb.left = 0;
			//tfb.top = tabbuttonheight - 1;
			//tfb.bottom = (functionnameheight * totalonfunctions) + tabbuttonheight + (tokenhackfunctionwindowxposoffset * 2) + 1 + 1;
			//SetDCPenColor(hDC, RGB(100, 100, 100));
			//Rectangle(hDC, tfb.left + 1, tfb.top, tfb.right, tfb.bottom);
			//Rectangle(hDC, tfb.left, tfb.top + 1, tfb.right + 1, tfb.bottom - 1);

			SelectObject(hDC, hpenOld);
			SelectObject(hDC, hbrushOld);

			EndPaint(g_hwnd, &ps);

			break;
		}
		case WM_ERASEBKGND: {
			return 1;
		}
		case WM_SIZE: {
			//TODO: fix this (rainbow and statbox sizing)
			//MoveWindow(g_statbox, statboxxoff, tabbuttonheight - 1, ((curwidth < minwidth) ? minwidth : curwidth), ((curheight < minheight) ? minheight : curheight), true);
			//
			//if (TBTN_RAINBOW.toggle_state && !is_drawing_static) {
			//	is_drawing_static = true;
			//	for (auto & a : tokenhackfunctions) {
			//		RedrawWindow(a->static_control.handle, NULL, NULL, RDW_INVALIDATE);
			//	}
			//}
			break;
		}
		case WM_CTLCOLORSTATIC: {
			static HBRUSH tempbrush = NULL;
			/*if (tempbrush != NULL) {
				DeleteObject(tempbrush);
				tempbrush = NULL;
			}

			HDC hdcStatic = (HDC)wParam;
			if ((HWND)lParam == statbox) {
				SetTextColor(hdcStatic, RGB(0, 118, 255));
				SetBkColor(hdcStatic, RGB(10, 10, 10));

				HDC dummy = GetDC(statbox);
				RECT rec = getclientrect(statbox);
				tempbrush = CreateSolidBrush(RGB(0, 118, 255));
				FrameRect(dummy, &rec, tempbrush);
				DeleteObject(tempbrush);
				tempbrush = NULL;
				ReleaseDC(statbox, dummy);

				tempbrush = CreateSolidBrush(RGB(10, 10, 10));
				return (INT_PTR)tempbrush;
			}

			if (getwindowclassname((HWND)lParam) != "Static")
				break;

			static int count = 0;
			count++;
			BYTE red1 = 255, gre1 = 0, blu1 = 105;

			if (TBTN_RAINBOW.toggle_state) {
				static int rainbowc = 1;
				static int rainbowc1 = 12;
				if (rainbowc > 12)
					rainbowc = 1;
				getRainbowColor(rainbowc);
				rainbowc++;
				if (count % totalonfunctions == 0) {
					if ((HWND)lParam == tokenhackfunctions[tokenhackfunctions.size() - 1]->static_control.handle)
						count = 0;
					count = 0;
					is_drawing_static = false;
					rainbowc = rainbowc1;
					rainbowc1--;
					if (rainbowc1 == 0)
						rainbowc1 = 12;
				}
			}

			for (auto & a : tokenhackfunctions) {
				if ((HWND)lParam == a->static_control.handle) {
					SetTextColor(hdcStatic, TBTN_RAINBOW.toggle_state ? RGB(red1, gre1, blu1) : a->static_control.color_scheme.text_idle_on);
					SetBkColor(hdcStatic, a->static_control.color_scheme.background_idle_on);
					tempbrush = CreateSolidBrush(a->static_control.color_scheme.border_idle_on);
					return (INT_PTR)tempbrush;
				}
			}
			vector<StaticControl*> staticcontrols = custom_controls.staticcontrols;
			for (auto & a : staticcontrols) {
				if ((HWND)lParam == a->handle) {
					SetTextColor(hdcStatic, TBTN_RAINBOW.toggle_state ? RGB(red1, gre1, blu1) : a->color_scheme.text_idle_on);
					SetBkColor(hdcStatic, a->color_scheme.background_idle_on);
					tempbrush = CreateSolidBrush(a->color_scheme.border_idle_on);
					return (INT_PTR)tempbrush;
				}
			}
			SetTextColor(hdcStatic, RGB(100, 100, 100));
			SetBkColor(hdcStatic, RGB(10, 10, 10));
			tempbrush = CreateSolidBrush(RGB(254, 33, 61));*/
			return (INT_PTR)tempbrush;
		}
		case WM_SETCURSOR: {
			if (getwindowclassname((HWND)wParam) == "Edit")
				return FALSE;
			switch (LOWORD(lParam)) {
			case HTSYSMENU:
			case HTREDUCE:
			case HTZOOM:
			case HTCLOSE:
			case HTMENU:
			case HTHSCROLL:
			case HTVSCROLL:
				SetCursor(press_cursor);
				return TRUE;
			case HTNOWHERE:
			case HTCLIENT:
			case HTCAPTION:
			case HTSIZE:
			case HTOBJECT:
			case HTHELP:
				SetCursor(idle_cursor);
				return TRUE;
			case CURSOR_LOADING:
				SetCursor(loading_cursor);
				return TRUE;
			}
			break;
		}
		case WM_TRAYICON: {
			switch (lParam) {
			case WM_LBUTTONUP:
				if (ShowWindow(g_hwnd, SW_SHOW))
					Minimize();
				else
					Restore();
				break;
			case WM_RBUTTONDOWN:
				POINT pt;
				GetCursorPos(&pt);
				SetForegroundWindow(hwnd);
				UINT clicked = TrackPopupMenu(g_menu, TPM_RETURNCMD | TPM_NONOTIFY | TPM_VERNEGANIMATION, pt.x, pt.y, 0, hwnd, NULL);
				if (clicked == 3000) {
					Shell_NotifyIcon(NIM_DELETE, &g_notifyIconData);
					PostQuitMessage(0);
				}
				if (clicked == 3001) {
					if (CTB_VOLUME.current_val > 0)
						PlaySound(MAKEINTRESOURCE(CAIN), NULL, SND_ASYNC | SND_RESOURCE);
					MessageBox(NULL, TEXT("Let Token Know"), TEXT("NOOOOOOOB"), MB_OK);
				}
				if (clicked == 3002) {
					ShellExecute(hwnd, TEXT("open"), TEXT("config.ini"), NULL, NULL, SW_SHOW);
				}
				if (clicked == 3003) {
					//				ShowWindow(BTN_MUSHROOM.handle, SW_SHOW);
					//				ShowWindow(BTN_COW.handle, SW_SHOW);
					PlaySound(MAKEINTRESOURCE(FRESHMEAT), NULL, SND_ASYNC | SND_RESOURCE);
				}
				break;
			}
			break;
		}
		/*case WM_NCACTIVATE:{
			if (wParam)
				mhook = SetWindowsHookEx(WH_MOUSE_LL, mhookProc, NULL, 0);
			else
				UnhookWindowsHookEx(mhook);
			break;
		}*/
		/*case WM_NCHITTEST:{
			UINT uHitTest = DefWindowProc(hwnd, WM_NCHITTEST, wParam, lParam);
			if (uHitTest == HTCLIENT)
				return HTCAPTION;
			else
				return uHitTest;
		}*/
		case WM_CLOSE:
		case WM_DESTROY: {
			EndDialog(hwnd, 0);
			PostQuitMessage(0);
			break;
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK hotkeydlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_INITDIALOG: {
			ClipCursor(NULL);
			SetWindowLongPtr(GetDlgItem(hwnd, HOTKEYBOX), DWLP_DLGPROC, (LONG_PTR)hotkeydlgProc);
			return TRUE;
		}
		case WM_SETCURSOR: {
			if (LOWORD(lParam) == HTCLOSE || LOWORD(lParam) == HTSYSMENU || GetDlgCtrlID((HWND)wParam) == BTN_OK) {
				SetCursor(press_cursor);
				return TRUE;
			}
			else {
				SetCursor(idle_cursor);
				return TRUE;
			}
			break;
		}
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case BTN_OK: {
					DWORD hotkey = (WORD)SendMessage(GetDlgItem(hwnd, KEYCONTROL), HKM_GETHOTKEY, 0, 0);
					if ((hotkey - (int)LOBYTE(hotkey)) / 256 == 1)
						hotkey += 768;
					else if ((hotkey - (DWORD)LOBYTE(hotkey)) / 256 == 4)
						hotkey -= 768;
					else if ((hotkey - (DWORD)LOBYTE(hotkey)) / 256 == 3)
						hotkey += 768;
					else if ((hotkey - (DWORD)LOBYTE(hotkey)) / 256 == 6)
						hotkey -= 768;
					else if ((hotkey - (DWORD)LOBYTE(hotkey)) / 256 == 6)
						hotkey -= 768;
					else if ((hotkey - (DWORD)LOBYTE(hotkey)) / 256 == 12)
						hotkey -= 256 * 11;
					else if ((hotkey - (DWORD)LOBYTE(hotkey)) / 256 == 14)
						hotkey -= 256 * 11;
					else if ((hotkey - (DWORD)LOBYTE(hotkey)) / 256 == 9)
						hotkey -= 256 * 5;
					else if ((hotkey - (DWORD)LOBYTE(hotkey)) / 256 == 11)
						hotkey -= 256 * 5;
					if (hotkey > ((256 * 7) + 255))
						hotkey -= 256 * 8;

					bool newkey = true;
					for (auto & a : tokenhackfunction::tokenhackfunctions) {
						if (hotkey == a->hotkey) {
							newkey = false;
							cout << "Key already being used as hotkey" << '\n';
							if (CTB_VOLUME.current_val > 0)
								PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
							SendMessage(GetDlgItem(hwnd, KEYCONTROL), HKM_SETHOTKEY, 0, 0);
							SetFocus(GetDlgItem(hwnd, KEYCONTROL));
							break;
						}
					}
					if (newkey)
						EndDialog(hwnd, hotkey);
					break;
				}
				}
				break;
			}
		case WM_CLOSE: 
		case WM_DESTROY: {
			EndDialog(hwnd, 0);
			break;
		}
	}
	return TRUE;
}

LRESULT CALLBACK kbhookProc(int code, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT key = *((KBDLLHOOKSTRUCT*)lParam);
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		string keyName = ProcessKey(&key);

		for (UINT i = 0; i < tokenhackfunction::tokenhackfunctions.size(); i++) {
			if (key.vkCode == tokenhackfunction::tokenhackfunctions[i]->hotkey && tokenhackfunction::tokenhackfunctions[i]->on) {
				tokenhackfunction::tokenhackfunctions[i]->Do();
			}
		}
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}
