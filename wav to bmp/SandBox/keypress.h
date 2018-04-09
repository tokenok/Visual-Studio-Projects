#ifndef keypress_header_guard
#define keypress_header_guard

#include <Windows.h>
#include <string>
#include <vector>

//state: down = -1; down then up = 0; up = 1;
void keydownup(UINT vk, int state = 0, HWND wnd = GetForegroundWindow(), bool bForceExtended = false, bool bAltDown = false);

void sendkey(char ch, HWND wnd = GetForegroundWindow());
void sendkeys(std::string s, HWND wnd = GetForegroundWindow(), UINT TIME = 0);
void sendkeys(std::vector<std::string> &v, HWND wnd = GetForegroundWindow(), UINT TIME = 0);

//state: down = -1; down then up = 0; up = 1;
void post_left_click(POINT pt, int state = 0, HWND wnd = GetForegroundWindow());
//state: down = -1; down then up = 0; up = 1;
void post_left_click(int state = 0);
//state: down = -1; down then up = 0; up = 1;
void post_right_click(POINT pt, int state = 0, HWND wnd = GetForegroundWindow());
//state: down = -1; down then up = 0; up = 1;
void post_right_click(int state = 0);
//state: down = -1; down then up = 0; up = 1; 
//leftright: leftclick < 0; rightclick > 0;
void post_timer_click(POINT pt, UINT time_ms = 0, int state = 0, int leftright = -1, HWND wnd = GetForegroundWindow());

//state: down = -1; down then up = 0; up = 1;
void send_left_click(POINT pt, int state = 0, HWND wnd = GetForegroundWindow());
//state: down = -1; down then up = 0; up = 1;
void send_left_click(int state = 0);
//state: down = -1; down then up = 0; up = 1;
void send_right_click(POINT pt, int state = 0, HWND wnd = GetForegroundWindow());
//state: down = -1; down then up = 0; up = 1;
void send_right_click(int state = 0);
//state: down = -1; down then up = 0; up = 1; 
//leftright: leftclick < 0; rightclick > 0;
void send_timer_click(POINT pt, UINT time_ms = 0, int state = 0, int leftright = -1, HWND wnd = GetForegroundWindow());

void scroll(POINT pt, int delta = 120, HWND wnd = GetForegroundWindow());
void scroll(int delta = 120);

void sendinput(char c, HWND wnd = GetForegroundWindow());


#endif

