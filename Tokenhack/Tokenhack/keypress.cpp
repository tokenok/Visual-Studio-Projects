#include <thread>

#include "keypress.h"
#include "window.h"
#include "common.h"

using namespace std;

//state: down = -1; down then up = 0; up = 1;
void keydownup(UINT vk, int state, HWND wnd, bool bForceExtended, bool bAltDown) {
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

void sendkey(char ch, HWND wnd) {
	PostMessage(wnd, WM_CHAR, ch, 0x00000001 | (LPARAM)((OemKeyScan(ch)) << 16));
}
void sendkeys(string s, HWND wnd, UINT TIME) {
	for (UINT i = 0; i < s.length(); i++) {
		sendkey(s[i], wnd);
		if (TIME)
			this_thread::sleep_for(chrono::milliseconds(TIME));
	}
}
void sendkeys(vector<string> &v, HWND wnd, UINT TIME) {
	for (UINT i = 0; i < v.size(); i++) {
		sendkeys(v[i], wnd, TIME);
	}
}

void post_left_click(POINT pt, int state, HWND wnd) {
	if (state == 0 || state < 0)
		PostMessage(wnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
	if (state == 0 || state > 0)
		PostMessage(wnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}
void post_left_click(int state) {
	post_left_click(getclientcursorpos(GetForegroundWindow()), state);
}

void post_right_click(POINT pt, int state, HWND wnd) {
	if (state == 0 || state < 0)
		PostMessage(wnd, WM_RBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
	if (state == 0 || state > 0)
		PostMessage(wnd, WM_RBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}
void post_right_click(int state) {
	post_right_click(getclientcursorpos(GetForegroundWindow()), state);
}

void post_timer_click(POINT pt, UINT time_ms, int state, int leftright, HWND wnd) {
	PostMessage(wnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
	if (time_ms)
		this_thread::sleep_for(chrono::milliseconds(time_ms));
	if (leftright < 0)
		post_left_click();
	if (leftright > 0)
		post_right_click();
}

void send_left_click(POINT pt, int state, HWND wnd) {
	if (state == 0 || state < 0)
		SendMessage(wnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
	if (state == 0 || state > 0)
		SendMessage(wnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}
void send_left_click(int state) {
	send_left_click(getclientcursorpos(GetForegroundWindow()), state);
}

void send_right_click(POINT pt, int state, HWND wnd) {
	if (state == 0 || state < 0)
		SendMessage(wnd, WM_RBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
	if (state == 0 || state > 0)
		SendMessage(wnd, WM_RBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}
void send_right_click(int state) {
	send_right_click(getclientcursorpos(GetForegroundWindow()), state);
}

void send_timer_click(POINT pt, UINT time_ms, int state, int leftright, HWND wnd) {
	SendMessage(wnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
	if (time_ms)
		this_thread::sleep_for(chrono::milliseconds(time_ms));
	if (leftright < 0)
		send_left_click();
	if (leftright > 0)
		send_right_click();
}

void scroll(POINT pt, int delta, HWND wnd) {
	PostMessage(wnd, WM_MOUSEWHEEL, MAKEWPARAM(0, delta), MAKELPARAM(pt.x, pt.y));
}
void scroll(int delta) {
	scroll(getclientcursorpos(GetForegroundWindow()), delta);
}