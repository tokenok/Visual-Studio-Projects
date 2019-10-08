#ifndef keypress
#define keypress

#include <Windows.h>
#include <string>
#include <iostream>
#include <sstream>

//state: down = -1; down then up = 0; up = 1;
void keydownup(UINT vk, int state = 0, HWND wnd = GetForegroundWindow(), bool bForceExtended = false, bool bAltDown = false) {
	//PostMessage(wnd, WM_CHAR, 0x000000A7, 0xC0380001);//§
	if (state == 0 || state < 0) {
		UINT lparam = 0x00000001 | (LPARAM)((MapVirtualKey(vk, MAPVK_VK_TO_VSC)) << 16);
		if (bForceExtended)
			lparam = lparam | 0x01000000;

		if (bAltDown || vk == VK_MENU)
			PostMessage(wnd, WM_SYSKEYDOWN, vk, lparam | 0x20000000);
		else
			PostMessage(wnd, WM_KEYDOWN, vk, lparam);
	}
	if (state == 0 || state > 0) {
		UINT lparam = 0xC0000001 | (LPARAM)((MapVirtualKey(vk, MAPVK_VK_TO_VSC)) << 16);
		if (bForceExtended)
			lparam = lparam | 0x01000000;

		if (bAltDown || vk == VK_MENU)
			PostMessage(wnd, WM_SYSKEYUP, vk, lparam | 0x20000000);
		else
			PostMessage(wnd, WM_KEYUP, vk, lparam);
	}
}

void sendkey(char ch, HWND wnd = GetForegroundWindow()) {
	PostMessage(wnd, WM_CHAR, ch, 0x00000001 | (LPARAM)((OemKeyScan(ch)) << 16));
}
void sendkeys(string s, HWND wnd = GetForegroundWindow(), UINT TIME = 0) {
	for (UINT i = 0; i < s.length(); i++) {		
		sendkey(s[i], wnd);
		if (TIME) 
			this_thread::sleep_for(chrono::milliseconds(TIME));
	}
}
void sendkeys(vector<string> &v, HWND wnd = GetForegroundWindow(), UINT TIME = 0) {
	for (UINT i = 0; i < v.size(); i++) {
		sendkeys(v[i], wnd, TIME);
	}
}
void d2type(string s, HWND wnd = GetForegroundWindow(), UINT entertime = 8, UINT keytime = 0) {
	if (entertime)
		this_thread::sleep_for(chrono::milliseconds(entertime));
	keydownup(VK_RETURN);
	if (entertime)
		this_thread::sleep_for(chrono::milliseconds(entertime));
	sendkeys(s, wnd, keytime);
	keydownup(VK_RETURN);
}
void d2type(vector<string> &v, HWND wnd = GetForegroundWindow(), UINT entertime = 8, UINT keytime = 0) {
	for (UINT i = 0; i < v.size(); i++) {
		d2type(v[i], wnd, entertime, keytime);
	}
}

//state: down = -1; down then up = 0; up = 1;
void post_left_click(POINT pt, int state = 0, HWND wnd = GetForegroundWindow()) {
	if (state == 0 || state < 0)
		PostMessage(wnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
	if (state == 0 || state > 0)
		PostMessage(wnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}
//state: down = -1; down then up = 0; up = 1;
void post_left_click(int state = 0) {
	post_left_click(getclientcursorpos(GetForegroundWindow()), state);
}
//state: down = -1; down then up = 0; up = 1;
void post_right_click(POINT pt, int state = 0, HWND wnd = GetForegroundWindow()) {
	if (state == 0 || state < 0)
		PostMessage(wnd, WM_RBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
	if (state == 0 || state > 0)
		PostMessage(wnd, WM_RBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}
//state: down = -1; down then up = 0; up = 1;
void post_right_click(int state = 0) {
	post_right_click(getclientcursorpos(GetForegroundWindow()), state);
}
//state: down = -1; down then up = 0; up = 1; 
//leftright: leftclick < 0; rightclick > 0;
void post_timer_click(POINT pt, UINT time_ms = 0, int state = 0, int leftright = -1, HWND wnd = GetForegroundWindow()) {
	PostMessage(wnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
	if (time_ms)
		this_thread::sleep_for(chrono::milliseconds(time_ms));
	if (leftright < 0)
		post_left_click();
	if (leftright > 0)
		post_right_click();
}

//state: down = -1; down then up = 0; up = 1;
void send_left_click(POINT pt, int state = 0, HWND wnd = GetForegroundWindow()) {
	if (state == 0 || state < 0)
		SendMessage(wnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
	if (state == 0 || state > 0)
		SendMessage(wnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}
//state: down = -1; down then up = 0; up = 1;
void send_left_click(int state = 0) {
	send_left_click(getclientcursorpos(GetForegroundWindow()), state);
}
//state: down = -1; down then up = 0; up = 1;
void send_right_click(POINT pt, int state = 0, HWND wnd = GetForegroundWindow()) {
	if (state == 0 || state < 0)
		SendMessage(wnd, WM_RBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
	if (state == 0 || state > 0)
		SendMessage(wnd, WM_RBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}
//state: down = -1; down then up = 0; up = 1;
void send_right_click(int state = 0) {
	send_right_click(getclientcursorpos(GetForegroundWindow()), state);
}
//state: down = -1; down then up = 0; up = 1; 
//leftright: leftclick < 0; rightclick > 0;
void send_timer_click(POINT pt, UINT time_ms = 0, int state = 0, int leftright = -1, HWND wnd = GetForegroundWindow()) {
	SendMessage(wnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
	if (time_ms)
		this_thread::sleep_for(chrono::milliseconds(time_ms));
	if (leftright < 0)
		send_left_click();
	if (leftright > 0)
		send_right_click();
}

void scroll(POINT pt, int delta = 120, HWND wnd = GetForegroundWindow()) {
	PostMessage(wnd, WM_MOUSEWHEEL, MAKEWPARAM(0, delta), MAKELPARAM(pt.x, pt.y));
}
void scroll(int delta = 120) {
	HWND wnd = GetForegroundWindow();
	scroll(getclientcursorpos(wnd), delta);
}

#endif

