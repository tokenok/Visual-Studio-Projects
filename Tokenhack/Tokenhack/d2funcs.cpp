#include <iostream>
#include <algorithm>
#include <time.h>
#include <thread>

#include "d2funcs.h"
#include "window.h"
#include "keypress.h"
#include "image.h"
#include "common.h"
#include "resource.h"
#include "ocr.h"

using namespace std;

int transmutebuttonx = -1;
int transmutebuttony = -1;
int goldbuttonx = -1;
int goldbuttony = -1;
int weapsonswitchkey = 0;
int g_clickdelay = 0;

string gamedir;
string gamepath = "";
string targetlines = "";

vector<string> spam_text_in_file;

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
	for (UINT i = 0; i < v.size(); i++) {
		d2type(v[i], wnd, entertime, keytime);
	}
}

bool get_transmute_pos(HWND wnd, LONG* x, LONG* y) {
	RECT client = getclientrect(wnd);
	if (TBTN_HOOKOUTPUT.toggle_state) {
		POINT client_cursor = getclientcursorpos(wnd);
		std::cout << "current client cursor pos: " << client_cursor.x << " " << client_cursor.y << '\n';
		std::cout << "current screen resolution: " << client.right << " " << client.bottom << '\n';
	}
	if (transmutebuttonx >= 0 && transmutebuttony >= 0) {
		*x = transmutebuttonx; *y = transmutebuttony;
	}
	else if (client.right == 640 && client.bottom == 480) {
		*x = 160; *y = 275;
	}
	else if (client.right == 800 && client.bottom == 600) {
		*x = 240; *y = 335;
	}
	else if (client.right == 1024 && client.bottom == 768) {
		*x = 160; *y = 445;
	}
	else if (client.right == 1280 && client.bottom == 1024) {
		*x = 165; *y = 560;
	}
	else
		return false;
	return true;
}
bool get_transmute_pos(HWND wnd, POINT* pt) {
	return get_transmute_pos(wnd, &pt->x, &pt->y);
}

bool get_gold_pos(HWND wnd, LONG* x, LONG* y) {
	RECT client = getclientrect(wnd);
	if (TBTN_HOOKOUTPUT.toggle_state) {
		POINT client_cursor = getclientcursorpos(wnd);
		std::cout << "current client cursor pos: " << client_cursor.x << " " << client_cursor.y << '\n';
		std::cout << "current screen resolution: " << client.right << " " << client.bottom << '\n';
	}
	if (goldbuttonx >= 0 && goldbuttony >= 0) {
		*x = goldbuttonx; *y = goldbuttony;
	}
	else if (client.right == 640 && client.bottom == 480) {
		*x = 411; *y = 401;
	}
	else if (client.right == 800 && client.bottom == 600) {
		*x = 496; *y = 458;
	}
	///////////////////////////////
	else if (client.right == 1024 && client.bottom == 768) {
		*x = 0; *y = 0;
	}
	else if (client.right == 1280 && client.bottom == 1024) {
		*x = 0; *y = 0;
	}
	else
		return false;
	return true;
}
bool get_gold_pos(HWND wnd, POINT* pt) {
	return get_gold_pos(wnd, &pt->x, &pt->y);
}

void switch_weapons() {
	if (weapsonswitchkey == -1)
		scroll(120);
	else if (weapsonswitchkey < -1)
		scroll(-120);
	else if (weapsonswitchkey)
		keydownup(weapsonswitchkey);
}

bool kill_all_OCR_auto_roll = false;

void OCR_auto_roll(HWND wnd, POINT pt, vector<vector<BYTE>> &font) {
	double start_time =  (double)clock();

	HWND auto_roll_display = CreateWindowEx(WS_EX_TOPMOST, TEXT("STATIC"), TEXT("guns are cool"), WS_VISIBLE | WS_CHILD, 0, 0, 200, 50, wnd, (HMENU)NULL, NULL, NULL);
	
	LONG transx, transy;
	if (!get_transmute_pos(wnd, &transx, &transy)) {
		if (TBTN_SOUND.toggle_state)
			PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
		std::cout << "transmute button location not found" << '\n';
		return;
	}

	RECT rcstartpos = getwindowrect(wnd);
	RECT rcdesktop = getclientrect(GetDesktopWindow());

	/*HHOOK dllhook;
	HHOOK dllhook2;
	HHOOK dllhook3;
	if (!InjectDll(DLL_NAME, wnd, &dllhook, &dllhook2, &dllhook3)) {
		MessageBox(NULL, L"Dll injection failed", L"Error", MB_OK);
		PostQuitMessage(0);
	}*/

	/*LONG winLong = GetWindowLong(wnd, GWL_EXSTYLE);
	SetWindowLong(wnd, GWL_EXSTYLE, winLong | WS_EX_LAYERED);
	SetLayeredWindowAttributes(wnd, 0, 1, LWA_ALPHA);*/

	if (AUTOROLL.on == 2)
		MoveWindow(wnd, rcdesktop.right, rcdesktop.bottom, rcstartpos.right - rcstartpos.left, rcstartpos.bottom - rcstartpos.top, true);
	if (AUTOROLL.on == 3)
		MoveWindow(wnd, rcstartpos.left, rcstartpos.top, (rcstartpos.right - rcstartpos.left) - (int)((rcstartpos.right - rcstartpos.left) * .4), rcstartpos.bottom - rcstartpos.top, true);

	vector<string> stats_in_file_copy = stats_in_file;

	bool match = false;

	while (!match) {
		WINDOWPLACEMENT wp;
		GetWindowPlacement(wnd, &wp);
		bool minimized = false;
		if (wp.showCmd == SW_MINIMIZE || wp.showCmd == SW_FORCEMINIMIZE || wp.showCmd == SW_SHOWMINIMIZED) 
			minimized = true;
		if (kill_all_OCR_auto_roll || !IsWindow(wnd) || minimized) {
			if (minimized)
				ShowWindow(wnd, SW_SHOWNOACTIVATE);
			if (TBTN_SOUND.toggle_state)
				PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
			std::cout << "exited rolling" << '\n';
			break;
		}
		int x, y;
		BYTE* newbuf = windowcapture(wnd, &x, &y);
		vector<string> item_stats = ocr(newbuf, x, y, font);
		delete[] newbuf;
		if (item_stats.size() == 0) {
			if (TBTN_SOUND.toggle_state)
				PlaySound(MAKEINTRESOURCE(NOTFOUND), NULL, SND_ASYNC | SND_RESOURCE);
			std::cout << "no stats found" << '\n';
			break;
		}
		////////////////////////////////////////////////////////////////////
		//implement OR(|), NOT(!), WILDCARD(?), WILDCARD SEQUENCE(?\), by character OR (<,>)
		//item sets/ stat groups
		for (UINT i = 0; i < stats_in_file_copy.size(); i++) {
			match = false;
			for (UINT j = 0; j < item_stats.size(); j++) {
				transform(item_stats[j].begin(), item_stats[j].end(), item_stats[j].begin(), tolower);
				item_stats[j].erase(remove(item_stats[j].begin(), item_stats[j].end(), ' '), item_stats[j].end());
				if (item_stats[j].find(stats_in_file_copy[i]) != string::npos) {
					match = true;
					break;
				}
			}
			if (!match) {
				send_left_click({transx, transy}, 0, wnd);
				this_thread::sleep_for(chrono::milliseconds(200));
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////
	}
	if (match && TBTN_SOUND.toggle_state) {
		if (TBTN_SOUND.toggle_state)
			PlaySound(MAKEINTRESOURCE(WTF), NULL, SND_ASYNC | SND_RESOURCE);
		std::cout << "match found" << '\n';
	}

	//UnhookWindowsHookEx(dllhook);
	//UnhookWindowsHookEx(dllhook2);
	//UnhookWindowsHookEx(dllhook3);

	//SetWindowLong(wnd, GWL_EXSTYLE, winLong);
	if (AUTOROLL.on == 2 || AUTOROLL.on == 3)
		MoveWindow(wnd, rcstartpos.left, rcstartpos.top, rcstartpos.right - rcstartpos.left, rcstartpos.bottom - rcstartpos.top, true);

	double end_time = (double)clock();

	std::cout << "time spent rolling: " << milliseconds_to_hms((int)(end_time - start_time)) << '\n';
}

