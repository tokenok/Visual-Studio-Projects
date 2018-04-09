#ifndef d2funcs_header_guard
#define d2funcs_header_guard

#include <Windows.h>
#include <vector>
#include <string>
#include <memory>

extern int transmutebuttonx;
extern int transmutebuttony;
extern int goldbuttonx;
extern int goldbuttony;
extern int weapsonswitchkey;
extern int g_clickdelay;

extern std::string gamedir;
extern std::string gamepath;
extern std::string targetlines;

extern std::vector<std::string> spam_text_in_file;

void d2type(std::string s, HWND wnd = GetForegroundWindow(), UINT entertime = 8, UINT keytime = 0);
void d2type(std::vector<std::string> &v, HWND wnd = GetForegroundWindow(), UINT entertime = 8, UINT keytime = 0);

bool get_transmute_pos(HWND wnd, LONG* x, LONG* y);
bool get_transmute_pos(HWND wnd, POINT* pt);

bool get_gold_pos(HWND wnd, LONG* x, LONG* y);
bool get_gold_pos(HWND wnd, POINT* pt);
void switch_weapons();

extern bool kill_all_OCR_auto_roll;

void OCR_auto_roll(HWND wnd, POINT pt, std::vector<std::vector<BYTE>> &font);

#endif

