#ifndef include_header_guard
#define include_header_guard

#include <windows.h>
#include <string>
#include <Commctrl.h>
#include <iostream>

#include "Wave.h"
#include "image.h"
#include "window.h"
#include "resource.h"
#include "functions.h"
#include "keypress.h"
#include "common.h"
#include "hotkey.h"

#pragma comment(lib, "ComCtl32.Lib")

#define VERSION L"DisableKey_v1.7"

wchar_t szClassName[] = TEXT("enterblock");

WORD disabled_key = VK_RETURN;
std::string league_of_legends_directory = "C:\\Riot Games\\League of Legends\\lol.launcher.exe";
std::string targetwindow = "League of Legends (TM) Client";

const UINT WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

#define WM_TRAYICON (WM_USER + 0)

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK hotkeydlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MyButtonWndProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
LRESULT CALLBACK MyStaticWndProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
VOID CALLBACK timerProc(HWND, UINT, UINT_PTR, DWORD);
LRESULT CALLBACK kbhookProc(int, WPARAM, LPARAM);
LRESULT CALLBACK mhookProc(int, WPARAM, LPARAM);

HHOOK kbhook;
HHOOK mhook;

HWND g_hwnd;

HMENU g_menu;
NOTIFYICONDATA notifyicondata;
HWND tog_btn;
HWND tog_stat;
HWND diw;

HBRUSH g_tempbrush = NULL;
bool state = true;
UINT timer1 = 1;
UINT timer2 = 2;
UINT timer3 = 3;
UINT timer4 = 4;
bool clickblock = false;

bool showonstart = true;

bool dieweeb = false;


#endif

